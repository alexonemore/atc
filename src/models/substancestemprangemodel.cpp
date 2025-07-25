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

#include "substancestemprangemodel.h"
#include "utilities.h"
#include <QBrush>

const QStringList SubstancesTempRangeModel::substances_temprange_field_names = {
	QStringLiteral("T min"),
	QStringLiteral("T max"),
	QStringLiteral("H"),
	QStringLiteral("S"),
	QStringLiteral("f1"),
	QStringLiteral("f2"),
	QStringLiteral("f3"),
	QStringLiteral("f4"),
	QStringLiteral("f5"),
	QStringLiteral("f6"),
	QStringLiteral("f7"),
	QStringLiteral("Phase")
};

SubstancesTempRangeModel::SubstancesTempRangeModel(QObject *parent)
	: QAbstractTableModel(parent)
{

}

SubstancesTempRangeModel::~SubstancesTempRangeModel()
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
	if(role == Qt::BackgroundRole) {
		return QBrush{Qt::white};
	}
	auto col = index.column();
	if(role != Qt::DisplayRole || col >= data_.size()) {
		return QVariant{};
	}
	auto&& data_at = data_.at(col);
	switch(static_cast<SubstanceTempRangeFields>(index.row())) {
	case SubstanceTempRangeFields::T_min:	return QString::number(data_at.T_min, 'g', 10);
	case SubstanceTempRangeFields::T_max:	return QString::number(data_at.T_max, 'g', 10);
	case SubstanceTempRangeFields::H:		return QString::number(data_at.H, 'g', 10);
	case SubstanceTempRangeFields::S:		return QString::number(data_at.S, 'g', 10);
	case SubstanceTempRangeFields::f1:		return QString::number(data_at.f1, 'g', 10);
	case SubstanceTempRangeFields::f2:		return QString::number(data_at.f2, 'g', 10);
	case SubstanceTempRangeFields::f3:		return QString::number(data_at.f3, 'g', 10);
	case SubstanceTempRangeFields::f4:		return QString::number(data_at.f4, 'g', 10);
	case SubstanceTempRangeFields::f5:		return QString::number(data_at.f5, 'g', 10);
	case SubstanceTempRangeFields::f6:		return QString::number(data_at.f6, 'g', 10);
	case SubstanceTempRangeFields::f7:		return QString::number(data_at.f7, 'g', 10);
	case SubstanceTempRangeFields::phase:	return data_at.phase;
	}
	LOG("ERROR in SubstancesTempRangeModel::data")
	return QVariant{};
}

QVariant SubstancesTempRangeModel::headerData(int section,
								Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Vertical) {
			return substances_temprange_field_names.at(section);
		} else {
			return section+1;
		}
	} else {
		return QVariant{};
	}
}
