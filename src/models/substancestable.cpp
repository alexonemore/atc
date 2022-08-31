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
	beginResetModel();
	data_ = std::move(new_data);
	row_count = data_.size();
	endResetModel();
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
	if(role != Qt::DisplayRole) {
		return QVariant{};
	}
	auto&& data_at = data_.at(index.row());
	switch(static_cast<Models::SubstanceFields>(index.column())) {
	case Models::SubstanceFields::ID:
		return data_at.id;
	case Models::SubstanceFields::Formula:
		return data_at.formula;
	case Models::SubstanceFields::Name:
		return data_at.name;
	case Models::SubstanceFields::T_min:
		return data_at.T_min;
	case Models::SubstanceFields::T_max:
		return data_at.T_max;
	}
}

QVariant SubstancesTable::headerData(int section, Qt::Orientation orientation,
									 int role) const
{
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		return Models::substances_field_names.at(section);
	} else {
		return QVariant{};
	}
}


