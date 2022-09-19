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

#include "substancestemprangemodel.h"
#include "utilities.h"

SubstancesTempRangeModel::SubstancesTempRangeModel(QObject *parent)
	: QAbstractTableModel(parent)
{

}

void SubstancesTempRangeModel::SetNewData(SubstanceTempRangeData&& new_data)
{
	beginResetModel();
	data_ = std::move(new_data);
	col_count = data_.size();
	endResetModel();
}

void SubstancesTempRangeModel::Clear()
{
	beginResetModel();
	data_.clear();
	col_count = 0;
	endResetModel();
}

int SubstancesTempRangeModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int SubstancesTempRangeModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count == 0 ? 1 : col_count;
	}
}

QVariant SubstancesTempRangeModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) return QVariant{};
	auto col = index.column();
	if(role != Qt::DisplayRole || col >= data_.size()) {
		return QVariant{};
	}
	auto&& data_at = data_.at(col);
	switch(static_cast<Models::SubstanceTempRangeFields>(index.row())) {
	case Models::SubstanceTempRangeFields::T_min:	return data_at.T_min;
	case Models::SubstanceTempRangeFields::T_max:	return data_at.T_max;
	case Models::SubstanceTempRangeFields::H:		return data_at.H;
	case Models::SubstanceTempRangeFields::S:		return data_at.S;
	case Models::SubstanceTempRangeFields::f1:		return data_at.f1;
	case Models::SubstanceTempRangeFields::f2:		return data_at.f2;
	case Models::SubstanceTempRangeFields::f3:		return data_at.f3;
	case Models::SubstanceTempRangeFields::f4:		return data_at.f4;
	case Models::SubstanceTempRangeFields::f5:		return data_at.f5;
	case Models::SubstanceTempRangeFields::f6:		return data_at.f6;
	case Models::SubstanceTempRangeFields::f7:		return data_at.f7;
	case Models::SubstanceTempRangeFields::phase:
		return Models::phases_names.at(static_cast<int>(data_at.phase));
	}
	LOG("ERROR in SubstancesTempRangeModel::data")
	return QVariant{};
}

QVariant SubstancesTempRangeModel::headerData(int section,
								Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Vertical) {
			return Models::substances_temprange_field_names.at(section);
		} else {
			return section+1;
		}
	} else {
		return QVariant{};
	}
}
