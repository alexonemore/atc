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
const int col_names_size{static_cast<int>(col_names.size())};
const int row_names_size{static_cast<int>(row_names.size())};
}

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

void ResultDetailModel::SetNewData(const Optimization::OptimizationVector* vec)
{
	beginResetModel();
	items = vec;
	row_count = items->cbegin()->number.substances + ResultFields::row_names_size;
	col_count = items->size();
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
	if(role == Qt::DisplayRole) {
		return QString{QString::number(index.row())+'/'+QString::number(index.column())};
	}
	return QVariant{};
}
