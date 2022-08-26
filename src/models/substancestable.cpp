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

void SubstancesTable::SetNewData(Data&& new_data) {
	data_ = std::move(new_data);
	col_count = data_.size();
	row_count = col_count < 1 ? 0 : data_.at(0).size();
	emit dataChanged(index(0, 0), index(row_count, col_count));
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
	if(role == Qt::DisplayRole) {
		if(row_count < 1) {
			return QVariant{};
		} else {
			return data_.at(index.row()).at(index.column());
		}
	} else {
		return QVariant{};
	}
}
