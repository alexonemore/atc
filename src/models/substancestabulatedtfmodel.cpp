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

#include "substancestabulatedtfmodel.h"
#include "utilities.h"

SubstancesTabulatedTFModel::SubstancesTabulatedTFModel(QObject *parent)
	: QAbstractTableModel(parent)
{

}

void SubstancesTabulatedTFModel::SetNewData(SubstancesTabulatedTFData&& new_data)
{
	beginResetModel();
	data_ = std::move(new_data);
	row_count = data_.temperatures.size();
	endResetModel();
}

int SubstancesTabulatedTFModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int SubstancesTabulatedTFModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
}

QVariant SubstancesTabulatedTFModel::data(const QModelIndex& index,
										  int role) const
{
	if(role != Qt::DisplayRole) {
		return QVariant{};
	}
	auto col = index.column();
	auto row = index.row();
	switch(static_cast<Models::SubstancesTabulatedTFFields>(col)) {
	case Models::SubstancesTabulatedTFFields::T:	return data_.temperatures.at(row);
	case Models::SubstancesTabulatedTFFields::G_kJ:	return data_.G_kJ.at(row);
	case Models::SubstancesTabulatedTFFields::H_kJ:	return data_.H_kJ.at(row);
	case Models::SubstancesTabulatedTFFields::F_J:	return data_.F_J.at(row);
	case Models::SubstancesTabulatedTFFields::S_J:	return data_.S_J.at(row);
	case Models::SubstancesTabulatedTFFields::Cp_J:	return data_.Cp_J.at(row);
	case Models::SubstancesTabulatedTFFields::c:	return data_.c.at(row);
	}
	LOG("ERROR in SubstancesTable::data")
	return QVariant{};
}

QVariant SubstancesTabulatedTFModel::headerData(int section,
							Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			switch(static_cast<Models::SubstancesTabulatedTFFields>(section)) {
			case Models::SubstancesTabulatedTFFields::T:
				return Models::substance_tabulated_tf_field_names.at(section).
						arg(ParametersNS::temperature_units.at(
								static_cast<int>(data_.temperature_unit)));
			default:
				return Models::substance_tabulated_tf_field_names.at(section);
			}
		} else {
			return section;
		}
	} else {
		return QVariant{};
	}
}
