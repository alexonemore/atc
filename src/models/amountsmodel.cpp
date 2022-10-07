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

#include "amountsmodel.h"

namespace AmountsModelFields {
extern const QStringList names{
	QT_TR_NOOP("ID"),
	QT_TR_NOOP("Formula"),
	QT_TR_NOOP("Weight"),
	QT_TR_NOOP("Group_1_mol"),
	QT_TR_NOOP("Group_1_gram"),
	QT_TR_NOOP("Group_2_mol"),
	QT_TR_NOOP("Group_2_gram"),
	QT_TR_NOOP("Sum_mol"),
	QT_TR_NOOP("Sum_gram"),
	QT_TR_NOOP("Sum_atpct"),
	QT_TR_NOOP("Sum_wtpct"),
	QT_TR_NOOP("Included")
};
}

AmountsModel::AmountsModel(QObject *parent)
	: QAbstractTableModel{parent}
{

}

AmountsModel::~AmountsModel()
{

}

int AmountsModel::rowCount(const QModelIndex& parent) const
{
	return 0;
}

int AmountsModel::columnCount(const QModelIndex& parent) const
{
	return 0;
}

QVariant AmountsModel::data(const QModelIndex& index, int role) const
{
	return QVariant{};
}

bool AmountsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

QVariant AmountsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant{};
}

Qt::ItemFlags AmountsModel::flags(const QModelIndex& index) const
{
	return QAbstractTableModel::flags(index);
}
