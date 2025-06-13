/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2025 Alexandr Shchukin
 * Corresponding email: shchukin.aleksandr.sergeevich@gmail.com
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

#include "substancestablemodel.h"
#include "utilities.h"
#include <QBrush>

const QStringList SubstancesTableModel::substances_field_names = {
	QStringLiteral("ID"),
	QStringLiteral("Formula"),
	QStringLiteral("Name"),
	QStringLiteral("Weight"),
	QStringLiteral("T min"),
	QStringLiteral("T max")
};

SubstancesTableModel::SubstancesTableModel(QObject *parent)
	: QAbstractTableModel(parent)
{

}

SubstancesTableModel::~SubstancesTableModel()
{

}

void SubstancesTableModel::SetNewData(SubstancesData&& new_data)
{
	beginResetModel();
	data_ = std::move(new_data);
	row_count = data_.size();
	endResetModel();
}

void SubstancesTableModel::Clear()
{
	beginResetModel();
	data_.clear();
	row_count = 0;
	endResetModel();
}

int SubstancesTableModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int SubstancesTableModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
}

QVariant SubstancesTableModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) return QVariant{};
	if(role == Qt::BackgroundRole) {
		return QBrush{Qt::white};
	}
	if(role != Qt::DisplayRole) {
		return QVariant{};
	}
	auto&& data_at = data_.at(index.row());
	switch(static_cast<SubstanceFields>(index.column())) {
	case SubstanceFields::ID:		return data_at.id;
	case SubstanceFields::Formula:	return data_at.formula;
	case SubstanceFields::Name:		return data_at.name;
	case SubstanceFields::Weight:	return data_at.weight;
	case SubstanceFields::T_min:	return data_at.T_min;
	case SubstanceFields::T_max:	return data_at.T_max;
	}
	LOG("ERROR in SubstancesTableModel::data")
	return QVariant{};
}

QVariant SubstancesTableModel::headerData(int section,
							Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			return substances_field_names.at(section);
		} else {
			return section;
		}
	} else {
		return QVariant{};
	}
}


