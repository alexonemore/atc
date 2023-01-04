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

ResultModel::ResultModel(QObject *parent)
	: QAbstractTableModel{parent}
{

}

ResultModel::~ResultModel()
{

}


int ResultModel::rowCount(const QModelIndex& parent) const
{
}

int ResultModel::columnCount(const QModelIndex& parent) const
{
}

QVariant ResultModel::data(const QModelIndex& index, int role) const
{
}

bool ResultModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
}

Qt::ItemFlags ResultModel::flags(const QModelIndex& index) const
{
}
