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
#include <array>

namespace SubstancesTabulatedTFFields {
const QStringList names {
	QStringLiteral("T [%1]")
};
constexpr std::array names_{
	MakeQLatin1String("T [%1]")
};
static constexpr auto names_size = static_cast<int>(names_.size());
}

SubstancesTabulatedTFModel::SubstancesTabulatedTFModel(QObject *parent)
	: QAbstractTableModel(parent)
	, col_count{static_cast<int>(SubstancesTabulatedTFFields::names_size +
				ParametersNS::thermodynamic_function_full.size())}
{
	substance_tabulated_tf_field_names = SubstancesTabulatedTFFields::names +
			ParametersNS::thermodynamic_function_full;

}

SubstancesTabulatedTFModel::~SubstancesTabulatedTFModel()
{

}

void SubstancesTabulatedTFModel::SetNewData(SubstancesTabulatedTFData&& new_data)
{
	beginResetModel();
	data_ = std::move(new_data);
	row_count = data_.temperatures.size();
	endResetModel();
}

void SubstancesTabulatedTFModel::Clear()
{
	beginResetModel();
	data_ = SubstancesTabulatedTFData{};
	row_count = 0;
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
	if(!index.isValid()) return QVariant{};
	if(role != Qt::DisplayRole) {
		return QVariant{};
	}
	auto col = index.column();
	auto row = index.row();
	if(col < SubstancesTabulatedTFFields::names_size) {
		switch(static_cast<SubstancesTabulatedTFFields::Names>(col)) {
		case SubstancesTabulatedTFFields::Names::T: return data_.temperatures.at(row);
		}
	} else {
		switch(static_cast<SubstancesTabulatedTFFields::TF>(col-1)) {
		case SubstancesTabulatedTFFields::TF::G_kJ:	return data_.G_kJ.at(row);
		case SubstancesTabulatedTFFields::TF::H_kJ:	return data_.H_kJ.at(row);
		case SubstancesTabulatedTFFields::TF::F_J:	return data_.F_J.at(row);
		case SubstancesTabulatedTFFields::TF::S_J:	return data_.S_J.at(row);
		case SubstancesTabulatedTFFields::TF::Cp_J:	return data_.Cp_J.at(row);
		case SubstancesTabulatedTFFields::TF::c:	return data_.c.at(row);
		}
	}
	LOG("ERROR in SubstancesTabulatedTFModel::data, index:", index)
	assert(false && "ERROR in SubstancesTabulatedTFModel::data");
	return QVariant{};
}

QVariant SubstancesTabulatedTFModel::headerData(int section,
							Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole) {
		return QVariant{};
	}
	if(orientation != Qt::Horizontal) {
		return section;
	}
	if(section < SubstancesTabulatedTFFields::names_size) {
		switch(static_cast<SubstancesTabulatedTFFields::Names>(section)) {
		case SubstancesTabulatedTFFields::Names::T:
			return substance_tabulated_tf_field_names.at(section).
					arg(ParametersNS::temperature_units.at(
							static_cast<int>(data_.temperature_unit)));
		}
	} else {
		return substance_tabulated_tf_field_names.at(section);
	}
	return QVariant{};
}
