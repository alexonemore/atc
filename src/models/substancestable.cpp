/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2022 Alexandr Shchukin
 * Corresponding email: shchukin@ism.ac.ru
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

#include "substancestable.h"

SubstancesTable::SubstancesTable(QObject *parent)
	: QAbstractTableModel(parent)
{

}

void SubstancesTable::SetNewData(SubstancesData&& new_data)
{

	data_ = std::move(new_data);
	row_count = data_.size();
	auto tl = QAbstractTableModel::index(0, 0);
	auto br = QAbstractTableModel::index(row_count, col_count);
	emit dataChanged(tl, br);
	emit headerDataChanged(Qt::Horizontal, 0, col_count);
}

int SubstancesTable::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int SubstancesTable::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
}

QVariant SubstancesTable::data(const QModelIndex& index, int role) const
{
	return QVariant{};
#if 0
	if(role == Qt::DisplayRole) {
#ifndef NDEBUG
		if(!data_.CheckIndex(index)) {
			return QVariant{};
		}
#endif
		return data_.AtIndex(index);
	} else {
		return QVariant{};
	}
#endif
}

QVariant SubstancesTable::headerData(int section, Qt::Orientation orientation,
									 int role) const
{
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		return SQL::substances_field_names.at(section);
	} else {
		return QVariant{};
	}
}

