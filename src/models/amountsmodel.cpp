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
	QT_TR_NOOP("Group 1\nmol"),
	QT_TR_NOOP("Group 1\ngram"),
	QT_TR_NOOP("Group 2\nmol"),
	QT_TR_NOOP("Group 2\ngram"),
	QT_TR_NOOP("Sum\nmol"),
	QT_TR_NOOP("Sum\ngram"),
	QT_TR_NOOP("Sum\natpct"),
	QT_TR_NOOP("Sum\nwtpct"),
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
	if(!CheckIndexValidParent(index)) return QVariant{};
	auto col = static_cast<AmountsModelFields::Names>(index.column());
	auto row = index.row();
	if(row == 0) {
		// TODO
		return QVariant{};
	}
	auto&& weight = weights.at(row-1); // -1 for Sum row
	auto id = weight.id;
	auto&& amount = amounts.at(id);
	switch(col) {
	case AmountsModelFields::Names::ID:
		if(role == Qt::DisplayRole) return id; break;
	case AmountsModelFields::Names::Formula:
		if(role == Qt::DisplayRole) return weight.formula; break;
	case AmountsModelFields::Names::Weight:
		if(role == Qt::DisplayRole) return weight.weight; break;
	case AmountsModelFields::Names::Group_1_mol:
		if(role == Qt::DisplayRole) return amount.group_1_mol; break;
	case AmountsModelFields::Names::Group_1_gram:
		if(role == Qt::DisplayRole) return amount.group_1_gram; break;
	case AmountsModelFields::Names::Group_2_mol:
		if(role == Qt::DisplayRole) return amount.group_2_mol; break;
	case AmountsModelFields::Names::Group_2_gram:
		if(role == Qt::DisplayRole) return amount.group_2_gram; break;
	case AmountsModelFields::Names::Sum_mol:
		if(role == Qt::DisplayRole) return amount.sum_mol; break;
	case AmountsModelFields::Names::Sum_gram:
		if(role == Qt::DisplayRole) return amount.sum_gram; break;
	case AmountsModelFields::Names::Sum_atpct:
		if(role == Qt::DisplayRole) return amount.sum_atpct; break;
	case AmountsModelFields::Names::Sum_wtpct:
		if(role == Qt::DisplayRole) return amount.sum_wtpct; break;
	case AmountsModelFields::Names::Included:
		if(role == Qt::CheckStateRole)
			return excluded.count(id) ? Qt::Unchecked : Qt::Checked;
		break;
	}

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
	if(!CheckIndexValidParent(index)) return Qt::ItemFlags{};
	Qt::ItemFlags flags = QAbstractTableModel::flags(index);
	auto col = static_cast<AmountsModelFields::Names>(index.column());
	switch(col) {
	case AmountsModelFields::Names::ID:
	case AmountsModelFields::Names::Formula:
	case AmountsModelFields::Names::Weight:
		return flags;
	case AmountsModelFields::Names::Group_1_mol:
	case AmountsModelFields::Names::Group_1_gram:
	case AmountsModelFields::Names::Group_2_mol:
	case AmountsModelFields::Names::Group_2_gram:
	case AmountsModelFields::Names::Sum_mol:
	case AmountsModelFields::Names::Sum_gram:
		flags |= Qt::ItemIsEditable;
		return flags;
	case AmountsModelFields::Names::Sum_atpct:
	case AmountsModelFields::Names::Sum_wtpct:
		return flags;
	case AmountsModelFields::Names::Included:
		flags |= Qt::ItemIsUserCheckable;
		return flags;
	}
}

bool AmountsModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}
