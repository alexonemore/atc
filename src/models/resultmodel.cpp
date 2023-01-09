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
const QStringList col_names{
	QT_TR_NOOP("ID"),
	QT_TR_NOOP("Name"),
	QT_TR_NOOP("Show")
};
const int col_names_size = static_cast<int>(col_names.size());

const QStringList row_names{
	QT_TR_NOOP("T %1"),
	QT_TR_NOOP("T initial"),
	QT_TR_NOOP("H initial"),
	QT_TR_NOOP("H equilibrium"),
	QT_TR_NOOP("c equilibrium"),
	QT_TR_NOOP("Sum mol"),
	QT_TR_NOOP("Sum gram"),
	QT_TR_NOOP("Sum at.%"),
	QT_TR_NOOP("Sum wt.%")
};
const int row_names_size = static_cast<int>(row_names.size());

const QStringList detail_row_names_single{
	QT_TR_NOOP("T units"),
	QT_TR_NOOP("T %1"),
	QT_TR_NOOP("T initial"),
	QT_TR_NOOP("H initial"),
	QT_TR_NOOP("H equilibrium"),
	QT_TR_NOOP("c equilibrium"),
	QT_TR_NOOP("Sum units"),
	QT_TR_NOOP("Sum")
};
const int detail_row_names_single_size = static_cast<int>(detail_row_names_single.size());

const QStringList detail_row_names_1d{
	QT_TR_NOOP("%1"),
	QT_TR_NOOP("T %1"),
	QT_TR_NOOP("T initial"),
	QT_TR_NOOP("H initial"),
	QT_TR_NOOP("H equilibrium"),
	QT_TR_NOOP("c equilibrium"),
	QT_TR_NOOP("Sum")
};
const int detail_row_names_1d_size = static_cast<int>(detail_row_names_1d.size());


} // namespace ResultFields

ResultModel::ResultModel(QObject *parent)
	: QAbstractTableModel{parent}
{

}

ResultModel::~ResultModel()
{

}

void ResultModel::SetNewData(const SubstanceWeights* vec, ParametersNS::Target tar)
{
	beginResetModel();
	items = vec;
	row_count = items->size() + ResultFields::row_names_size;
	target = tar;
	checked.clear();
	endResetModel();
}

void ResultModel::Clear()
{
	beginResetModel();
	row_count = 0;
	items = nullptr;
	checked.clear();
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
		return ResultFields::col_names_size;
	}
}

QVariant ResultModel::data(const QModelIndex& index, int role) const
{
	if(!CheckIndexValidParent(index)) return QVariant{};
	auto col = static_cast<ResultFields::ColNames>(index.column());
	auto row = index.row();

	if(role == Qt::BackgroundRole) {
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

	switch (col) {
	case ResultFields::ColNames::ID:
		if(role == Qt::DisplayRole) {
			if(row >= ResultFields::row_names_size) {
				return items->at(row - ResultFields::row_names_size).id;
			}
		}
		break;
	case ResultFields::ColNames::Name:
		if(role == Qt::DisplayRole) {
			if(row < ResultFields::row_names_size) {
				if(row == 0) {
					switch(target) {
					case ParametersNS::Target::Equilibrium:
						return ResultFields::row_names.at(row).arg(tr("equilibrium"));
					case ParametersNS::Target::AdiabaticTemperature:
						return ResultFields::row_names.at(row).arg(tr("adiabatic"));
					}
				} else {
					return ResultFields::row_names.at(row);
				}
			} else {
				return items->at(row - ResultFields::row_names_size).formula;
			}
		}
		break;
	case ResultFields::ColNames::Show:
		if(role == Qt::CheckStateRole) {
			auto find = checked.find(row);
			if(find != checked.cend()) {
				return find->second.checked;
			} else {
				return Qt::CheckState::Unchecked;
			}
		}
		break;
	}

	return QVariant{};
}

bool ResultModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	LOG()
	if(!CheckIndexValidParent(index)) return false;

	// TODO

	return false;
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			return ResultFields::col_names.at(section);
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

ResultDetailModel::ResultDetailModel(QObject* parent)
	: QAbstractTableModel{parent}
{

}

ResultDetailModel::~ResultDetailModel()
{

}

void ResultDetailModel::SetNewData(const Optimization::OptimizationVector* vec,
								   const int x_size, const int y_size)
{
	beginResetModel();
	items = vec;
	workmode = items->cbegin()->parameters.workmode;
	target = items->cbegin()->parameters.target;
	row_count = items->cbegin()->number.substances;
	col_count = x_size;
	switch (workmode) {
	case ParametersNS::Workmode::SinglePoint:
		row_count += ResultFields::detail_row_names_single_size;
		col_count += ParametersNS::composition_units.size();
		break;
	case ParametersNS::Workmode::TemperatureRange:
	case ParametersNS::Workmode::CompositionRange:
		row_count += ResultFields::detail_row_names_1d_size;
		col_count += 2; // Names, Units
		break;
	case ParametersNS::Workmode::TemperatureCompositionRange:
		row_count = y_size + 1;
		col_count += 1;
		break;
		break;
	}
	endResetModel();
}

void ResultDetailModel::Clear()
{
	beginResetModel();
	row_count = 0;
	col_count = 0;
	items = nullptr;
	endResetModel();
}

bool ResultDetailModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}

int ResultDetailModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int ResultDetailModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
}

QVariant ResultDetailModel::data(const QModelIndex& index, int role) const
{
	if(!CheckIndexValidParent(index)) return QVariant{};
	auto col = index.column();
	auto row = index.row();
	if(role == Qt::BackgroundRole) {
		return QBrush{Qt::white};
	}
	if(role == Qt::DisplayRole) {
		switch (workmode) {
		case ParametersNS::Workmode::SinglePoint:
			return DataSingle(row, col);
		case ParametersNS::Workmode::TemperatureRange:
			break;
		case ParametersNS::Workmode::CompositionRange:
			break;
		case ParametersNS::Workmode::TemperatureCompositionRange:
			break;
		}
	}
	return QVariant{};
}

QVariant ResultDetailModel::DataSingle(const int row, const int col) const
{
	switch (static_cast<ResultFields::DetailRowNamesSingle>(row)) {
	case ResultFields::DetailRowNamesSingle::T_units:
		switch (col) {
		case 0: return tr("T units");
		case 1: return tr("K");
		case 2: return tr("C");
		case 3: return tr("F");
		default: break;
		}
		break;
	case ResultFields::DetailRowNamesSingle::T_result:

	case ResultFields::DetailRowNamesSingle::T_initial:
	case ResultFields::DetailRowNamesSingle::H_initial:
	case ResultFields::DetailRowNamesSingle::H_equilibrium:
	case ResultFields::DetailRowNamesSingle::c_equilibrium:
	case ResultFields::DetailRowNamesSingle::Sum_units:
	case ResultFields::DetailRowNamesSingle::Sum_value:
		break;
	}
	return QVariant{};
}

