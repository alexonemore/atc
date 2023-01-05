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
	QT_TR_NOOP("Formula"),
	QT_TR_NOOP("Mol"),
	QT_TR_NOOP("Gram"),
	QT_TR_NOOP("At.%"),
	QT_TR_NOOP("Wt.%")
};
}

ResultModel::ResultModel(QObject *parent)
	: QAbstractTableModel{parent}
	, col_count{static_cast<int>(ResultFields::names.size())}
{

}

ResultModel::~ResultModel()
{

}

void ResultModel::SetNewData(Optimization::OptimizationVector& vec)
{
	LOG()
	beginResetModel();
	items = std::move(vec);
	row_count = items.cbegin()->number.substances + 1;



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
	auto col = static_cast<ResultFields::ColNames>(index.column());
	auto row = index.row();
	if(row == 0) {
		if(role == Qt::BackgroundRole) {
			return QBrush{Qt::lightGray};
		} else if(role == Qt::DisplayRole) {
			switch(col) {
			case ResultFields::ColNames::ID:
				break;
			case ResultFields::ColNames::Formula:
				return tr("Sum");
			case ResultFields::ColNames::Mol:
				return sum.sum_mol;
			case ResultFields::ColNames::Gram:
				return sum.sum_gram;
			case ResultFields::ColNames::AtPct:
				return sum.sum_atpct;
			case ResultFields::ColNames::WtPct:
				return sum.sum_wtpct;
			}
		}
	} else {
		switch(col) {
		case ResultFields::ColNames::ID:

		case ResultFields::ColNames::Formula:
			return tr("Sum");
		case ResultFields::ColNames::Mol:
		case ResultFields::ColNames::Gram:
		case ResultFields::ColNames::AtPct:
		case ResultFields::ColNames::WtPct:
			break;
		}
	}
	return QVariant();
}

bool ResultModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
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
	case ResultFields::ColNames::Formula:
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


