/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2023 Alexandr Shchukin
 * Corresponding email: alexonemoreemail@gmail.com
 *
 * ATC (Adiabatic Temperature Calculator) is free software:
 * you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * ATC (Adiabatic Temperature Calculator) is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ATC (Adiabatic Temperature Calculator).
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "resultmodel.h"
#include "utilities.h"
#include <QBrush>

namespace ResultFields {
const QStringList names{
	QT_TR_NOOP("ID"),
	QT_TR_NOOP("Name"),
	QT_TR_NOOP("Show")
};
const QStringList row_equilibrium{
	QT_TR_NOOP("T equilibrium"),
};
const QStringList row_adiabatic{
	QT_TR_NOOP("T adiabatic"),
};
const QStringList row_all{
	QT_TR_NOOP("T initial"),
	QT_TR_NOOP("H initial"),
	QT_TR_NOOP("H equilibrium"),
	QT_TR_NOOP("c equilibrium"),
	QT_TR_NOOP("Sum mol"),
	QT_TR_NOOP("Sum gram"),
	QT_TR_NOOP("Sum at.%"),
	QT_TR_NOOP("Sum wt.%")
};
const QStringList row_equilibrium_names = row_equilibrium + row_all;
const QStringList row_adiabatic_names = row_adiabatic + row_all;

}

ResultModel::ResultModel(QObject *parent)
	: QAbstractTableModel{parent}
	, col_count{static_cast<int>(ResultFields::names.size())}
	, row_offset{static_cast<int>(ResultFields::row_equilibrium_names.size())}
{

}

ResultModel::~ResultModel()
{

}

void ResultModel::SetNewData(SubstanceNames&& vec, ParametersNS::Parameters par)
{
	beginResetModel();
	items = std::move(vec);
	row_count = items.size() + row_offset;
	parameters = par;
	checked.clear();
	switch(parameters.target) {
	case ParametersNS::Target::Equilibrium:
		checked[static_cast<int>(ResultFields::RowEquilibrium::c_equilibrium)] =
				Cell{GetRandomColor(), Qt::CheckState::Checked};
		break;
	case ParametersNS::Target::AdiabaticTemperature:
		checked[static_cast<int>(ResultFields::RowAdiabatic::T_adiabatic)] =
				Cell{GetRandomColor(), Qt::CheckState::Checked};
		break;
	}
	endResetModel();
}

int ResultModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int ResultModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
}

QVariant ResultModel::data(const QModelIndex& index, int role) const
{
	if(!CheckIndexValidParent(index)) return QVariant{};
	switch (parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		return dataSingle(index, role);
	case ParametersNS::Workmode::TemperatureRange:
		return dataTemperatureRange(index, role);
	case ParametersNS::Workmode::CompositionRange:
		return dataCompositionRange(index, role);
	case ParametersNS::Workmode::TemperatureCompositionRange:
		return dataTempCompRange(index, role);
	}
	return QVariant{};
}

bool ResultModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(!CheckIndexValidParent(index)) return false;
	switch (parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		return setDataSingle(index, value, role);
	case ParametersNS::Workmode::TemperatureRange:
		return setDataTemperatureRange(index, value, role);
	case ParametersNS::Workmode::CompositionRange:
		return setDataCompositionRange(index, value, role);
	case ParametersNS::Workmode::TemperatureCompositionRange:
		return setDataTempCompRange(index, value, role);
	}
	return false;
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			return ResultFields::names.at(section);
		} else {
			return section;
		}
	} else {
		return QVariant{};
	}
}

Qt::ItemFlags ResultModel::flags(const QModelIndex& index) const
{
	if(!CheckIndexValidParent(index)) return Qt::ItemFlags{};
	Qt::ItemFlags flags = QAbstractTableModel::flags(index);
	auto col = static_cast<ResultFields::ColNames>(index.column());
	switch(col) {
	case ResultFields::ColNames::Name:
		flags |= Qt::ItemIsUserCheckable;
		flags |= Qt::ItemIsEditable;
		flags ^= Qt::ItemIsSelectable;
		return  flags;
	default:
		return flags;
	}
}

bool ResultModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}

QVariant ResultModel::dataSingle(const QModelIndex& index, int role) const
{
	auto col = static_cast<ResultFields::ColNames>(index.column());
	auto row = index.row();

	if(role == Qt::CheckStateRole) {
		switch(col) {
		case ResultFields::ColNames::Show: {
			auto find = checked.find(row);
			if(find != checked.cend()) {
				return find->second.checked;
			} else {
				return Qt::CheckState::Unchecked;
			}
		}
		default:
			break;
		}
	} else if(role == Qt::DisplayRole) {
		switch(col) {
		case ResultFields::ColNames::ID:
			if(row >= row_offset) {
				return items.at(row - row_offset).id;
			}
			break;
		case ResultFields::ColNames::Name:
			if(row < row_offset) {
				switch(parameters.target) {
				case ParametersNS::Target::Equilibrium:
					return ResultFields::row_equilibrium_names.at(row);
				case ParametersNS::Target::AdiabaticTemperature:
					return ResultFields::row_adiabatic_names.at(row);
				}
			} else {
				return items.at(row - row_offset).formula;
			}
			break;
		default:
			break;
		}
	} else if(role == Qt::BackgroundRole) {
		switch(col) {
		case ResultFields::ColNames::Show: {
			auto find = checked.find(row);
			if(find != checked.cend()) {
				return find->second.color;
			}
		}
		default:
			break;
		}
		return QBrush{Qt::white};
	}

	return QVariant();
}

QVariant ResultModel::dataTemperatureRange(const QModelIndex& index, int role) const
{
	return QVariant();
}

QVariant ResultModel::dataCompositionRange(const QModelIndex& index, int role) const
{
	return QVariant();
}

QVariant ResultModel::dataTempCompRange(const QModelIndex& index, int role) const
{
	return QVariant();
}

bool ResultModel::setDataSingle(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

bool ResultModel::setDataTemperatureRange(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

bool ResultModel::setDataCompositionRange(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

bool ResultModel::setDataTempCompRange(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}


