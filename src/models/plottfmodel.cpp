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

#include "plottfmodel.h"

PlotTFModel::PlotTFModel(QObject *parent)
	: QAbstractTableModel(parent)
{

}

PlotTFModel::~PlotTFModel()
{

}

void PlotTFModel::SetNewData(SubstanceNames&& data)
{
	beginResetModel();
	data_ = std::move(data);
	row_count = data_.size();
	endResetModel();
}

void PlotTFModel::Clear()
{
	beginResetModel();
	data_.clear();
	endResetModel();
}

int PlotTFModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int PlotTFModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
}

QVariant PlotTFModel::data(const QModelIndex& index, int role) const
{
	return QVariant();
}

QVariant PlotTFModel::headerData(int section, Qt::Orientation orientation,
								 int role) const
{
	return QVariant();
}
