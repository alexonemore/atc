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

void AmountsModel::SetNewData(SubstanceWeights&& new_weights)
{
	beginResetModel();
	weights = std::move(new_weights);
	for(auto i = weights.cbegin(), end = weights.cend(); i != end; ++i) {
		amounts_new.emplace(i->id, Amounts{});
	}
	assert(weights.size() == amounts_new.size());

	if(amounts_new.size() > amounts.size()) {
		amounts.merge(amounts_new);
	} else {
		amounts_new.merge(amounts);
	}
	assert(weights.size() == amounts.size());
	amounts_new.clear();
	row_count = weights.size()+1;
	endResetModel();
}

void AmountsModel::Clear()
{
	beginResetModel();
	weights.clear();
	amounts.clear();
	row_count = 1;
	endResetModel();
}

int AmountsModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int AmountsModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
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
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			return AmountsModelFields::names.at(section);
		} else {
			return section;
		}
	} else {
		return QVariant{};
	}
}

Qt::ItemFlags AmountsModel::flags(const QModelIndex& index) const
{
	return QAbstractTableModel::flags(index);
}

bool AmountsModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}
