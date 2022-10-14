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
#include <QBrush>
#include <QKeyEvent>
#include <QTableView>
#include "utilities.h"

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
	QT_TR_NOOP("Sum\nat.%"),
	QT_TR_NOOP("Sum\nwt.%"),
	QT_TR_NOOP("Included")
};
const QColor group_1_color{171, 211, 255};
const QColor group_2_color{255, 215, 171};
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
	Recalculate();
	endResetModel();
}

void AmountsModel::Clear()
{
	beginResetModel();
	weights.clear();
	amounts.clear();
	row_count = 1;
	Recalculate();
	endResetModel();
}

CompositionData AmountsModel::GetCompositionData() const
{
	SubstanceWeights new_weights;
	Composition new_amounts;
	for(const auto& sub : weights) {
		auto id = sub.id;
		if(!excluded.count(id)) {
			new_weights.push_back(sub);
			new_amounts[id] = amounts.at(id);
		}
	}
	return CompositionData{new_weights, new_amounts};
}

void AmountsModel::Delete(const QModelIndexList& selected)
{
	for(auto&& index : selected) {
		auto row = index.row();
		if(row > 0) {
			amounts.at(weights.at(row-1).id) = Amounts{};
		}
	}
	RecalculateAndUpdate();
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
		if(role == Qt::BackgroundRole) {
			return QBrush{Qt::lightGray};
		} else if(role == Qt::DisplayRole) {
			switch(col) {
			case AmountsModelFields::Names::ID:
				break;
			case AmountsModelFields::Names::Formula:
				return tr("Sum");
			case AmountsModelFields::Names::Weight:
				break;
			case AmountsModelFields::Names::Group_1_mol:
				return sum.group_1_mol;
			case AmountsModelFields::Names::Group_1_gram:
				return sum.group_1_gram;
			case AmountsModelFields::Names::Group_2_mol:
				return sum.group_2_mol;
			case AmountsModelFields::Names::Group_2_gram:
				return sum.group_2_gram;
			case AmountsModelFields::Names::Sum_mol:
				return sum.sum_mol;
			case AmountsModelFields::Names::Sum_gram:
				return sum.sum_gram;
			case AmountsModelFields::Names::Sum_atpct:
				return sum.sum_atpct;
			case AmountsModelFields::Names::Sum_wtpct:
				return sum.sum_wtpct;
			case AmountsModelFields::Names::Included: {
				auto all = amounts.size();
				auto ex = excluded.size();
				return QString::number(all-ex) + " " + tr("of") + " " +
						QString::number(all);
			}
			}
		}
	} else {
		auto&& weight = weights.at(row-1); // -1 for Sum row
		auto&& amount = amounts.at(weight.id);
		auto zero = [](double d)->QVariant {return d > 0 ? d : QVariant{};};
		switch(col) {
		case AmountsModelFields::Names::ID:
			if(role == Qt::DisplayRole)
				return weight.id;
			else if(role == Qt::BackgroundRole)
				return QBrush{Qt::white};
			break;
		case AmountsModelFields::Names::Formula:
			if(role == Qt::DisplayRole)
				return weight.formula;
			else if(role == Qt::BackgroundRole)
				return QBrush{Qt::white};
			break;
		case AmountsModelFields::Names::Weight:
			if(role == Qt::DisplayRole)
				return weight.weight;
			else if(role == Qt::BackgroundRole)
				return QBrush{Qt::white};
			break;
		case AmountsModelFields::Names::Group_1_mol:
			if(role == Qt::DisplayRole)
				return zero(amount.group_1_mol);
			else if(role == Qt::BackgroundRole)
				return QBrush{AmountsModelFields::group_1_color};
			break;
		case AmountsModelFields::Names::Group_1_gram:
			if(role == Qt::DisplayRole)
				return zero(amount.group_1_gram);
			else if(role == Qt::BackgroundRole)
				return QBrush{AmountsModelFields::group_1_color};
			break;
		case AmountsModelFields::Names::Group_2_mol:
			if(role == Qt::DisplayRole)
				return zero(amount.group_2_mol);
			else if(role == Qt::BackgroundRole)
				return QBrush{AmountsModelFields::group_2_color};
			break;
		case AmountsModelFields::Names::Group_2_gram:
			if(role == Qt::DisplayRole)
				return zero(amount.group_2_gram);
			else if(role == Qt::BackgroundRole)
				return QBrush{AmountsModelFields::group_2_color};
			break;
		case AmountsModelFields::Names::Sum_mol:
			if(role == Qt::DisplayRole)
				return zero(amount.sum_mol);
			else if(role == Qt::BackgroundRole)
				return QBrush{Qt::white};
			break;
		case AmountsModelFields::Names::Sum_gram:
			if(role == Qt::DisplayRole)
				return zero(amount.sum_gram);
			else if(role == Qt::BackgroundRole)
				return QBrush{Qt::white};
			break;
		case AmountsModelFields::Names::Sum_atpct:
			if(role == Qt::DisplayRole)
				return zero(amount.sum_atpct);
			else if(role == Qt::BackgroundRole)
				return QBrush{Qt::white};
			break;
		case AmountsModelFields::Names::Sum_wtpct:
			if(role == Qt::DisplayRole)
				return zero(amount.sum_wtpct);
			else if(role == Qt::BackgroundRole)
				return QBrush{Qt::white};
			break;
		case AmountsModelFields::Names::Included:
			if(role == Qt::CheckStateRole)
				return excluded.count(weight.id) ? Qt::Unchecked : Qt::Checked;
			else if(role == Qt::DisplayRole)
				return excluded.count(weight.id) ? "-" : "+";
			else if(role == Qt::BackgroundRole)
				return QBrush{Qt::white};
			break;
		}
	}
	return QVariant{};
}

bool AmountsModel::setData(const QModelIndex& index, const QVariant& value,
						   int role)
{
	if(!CheckIndexValidParent(index)) return false;
	auto col = static_cast<AmountsModelFields::Names>(index.column());
	auto row = index.row();
	if(row == 0) { // Sum row
		if(role != Qt::EditRole) {
			return false;
		} else {
			switch(col) {
			case AmountsModelFields::Names::ID:
			case AmountsModelFields::Names::Formula:
			case AmountsModelFields::Names::Weight:
				break;
			case AmountsModelFields::Names::Group_1_mol: {
				if(sum.group_1_mol > 0.0) {
					auto val = std::abs(value.toDouble());
					auto coef = val / sum.group_1_mol;
					sum.group_1_mol = val;
					for(const auto& weight : weights) {
						auto id = weight.id;
						auto w = weight.weight;
						auto&& amount = amounts.at(id);
						amount.group_1_mol *= coef;
						amount.group_1_gram = amount.group_1_mol * w;
						amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
						amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
					}
					RecalculateAndUpdate();
					return true;
				}
			} break;
			case AmountsModelFields::Names::Group_1_gram: {
				if(sum.group_1_gram > 0.0) {
					auto val = std::abs(value.toDouble());
					auto coef = val / sum.group_1_gram;
					sum.group_1_gram = val;
					for(const auto& weight : weights) {
						auto id = weight.id;
						auto w = weight.weight;
						auto&& amount = amounts.at(id);
						amount.group_1_gram *= coef;
						amount.group_1_mol = amount.group_1_gram / w;
						amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
						amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
					}
					RecalculateAndUpdate();
					return true;
				}
			} break;
			case AmountsModelFields::Names::Group_2_mol: {
				if(sum.group_2_mol > 0.0) {
					auto val = std::abs(value.toDouble());
					auto coef = val / sum.group_2_mol;
					sum.group_2_mol = val;
					for(const auto& weight : weights) {
						auto id = weight.id;
						auto w = weight.weight;
						auto&& amount = amounts.at(id);
						amount.group_2_mol *= coef;
						amount.group_2_gram = amount.group_2_mol * w;
						amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
						amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
					}
					RecalculateAndUpdate();
					return true;
				}
			} break;
			case AmountsModelFields::Names::Group_2_gram: {
				if(sum.group_2_gram > 0.0) {
					auto val = std::abs(value.toDouble());
					auto coef = val / sum.group_2_gram;
					sum.group_2_gram = val;
					for(const auto& weight : weights) {
						auto id = weight.id;
						auto w = weight.weight;
						auto&& amount = amounts.at(id);
						amount.group_2_gram *= coef;
						amount.group_2_mol = amount.group_2_gram / w;
						amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
						amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
					}
					RecalculateAndUpdate();
					return true;
				}
			} break;
			case AmountsModelFields::Names::Sum_mol: {
				if(sum.sum_mol > 0.0) {
					auto val = std::abs(value.toDouble());
					auto coef = val / sum.sum_mol;
					sum.sum_mol = val;
					for(const auto& weight : weights) {
						auto id = weight.id;
						auto w = weight.weight;
						auto&& amount = amounts.at(id);
						amount.sum_mol *= coef;
						amount.group_1_mol *= coef;
						amount.group_2_mol *= coef;
						amount.group_1_gram = amount.group_1_mol * w;
						amount.group_2_gram = amount.group_2_mol * w;
						amount.sum_gram = amount.group_2_gram + amount.group_2_gram;
					}
					RecalculateAndUpdate();
					return true;
				}
			} break;
			case AmountsModelFields::Names::Sum_gram: {
				if(sum.sum_gram > 0.0) {
					auto val = std::abs(value.toDouble());
					auto coef = val / sum.sum_gram;
					sum.sum_gram = val;
					for(const auto& weight : weights) {
						auto id = weight.id;
						auto w = weight.weight;
						auto&& amount = amounts.at(id);
						amount.sum_gram *= coef;
						amount.group_1_gram *= coef;
						amount.group_2_gram *= coef;
						amount.group_1_mol = amount.group_1_gram / w;
						amount.group_2_mol = amount.group_2_gram / w;
						amount.sum_mol= amount.group_2_mol+ amount.group_2_mol;
					}
					RecalculateAndUpdate();
					return true;
				}
			} break;
			case AmountsModelFields::Names::Sum_atpct:
			case AmountsModelFields::Names::Sum_wtpct:
			case AmountsModelFields::Names::Included:
				break;
			}
		}
	} else {
		auto&& substance_weight = weights.at(row-1); // -1 for Sum row
		auto&& amount = amounts.at(substance_weight.id);
		if(excluded.count(substance_weight.id) && role == Qt::EditRole) {
			amount = Amounts{};
			RecalculateAndUpdate();
			return false;
		}
		switch(col) {
		case AmountsModelFields::Names::ID:
		case AmountsModelFields::Names::Formula:
		case AmountsModelFields::Names::Weight:
			break;
		case AmountsModelFields::Names::Group_1_mol:
			if(role == Qt::EditRole) {
				auto weight = substance_weight.weight;
				amount.group_1_mol = std::abs(value.toDouble());
				amount.group_1_gram = amount.group_1_mol * weight;
				amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
				amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
				RecalculateAndUpdate();
				return true;
			} break;
		case AmountsModelFields::Names::Group_1_gram:
			if(role == Qt::EditRole) {
				auto weight = substance_weight.weight;
				amount.group_1_gram = std::abs(value.toDouble());
				amount.group_1_mol = amount.group_1_gram / weight;
				amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
				amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
				RecalculateAndUpdate();
				return true;
			} break;
		case AmountsModelFields::Names::Group_2_mol:
			if(role == Qt::EditRole) {
				auto weight = substance_weight.weight;
				amount.group_2_mol = std::abs(value.toDouble());
				amount.group_2_gram = amount.group_2_mol * weight;
				amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
				amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
				RecalculateAndUpdate();
				return true;
			} break;
		case AmountsModelFields::Names::Group_2_gram:
			if(role == Qt::EditRole) {
				auto weight = substance_weight.weight;
				amount.group_2_gram = std::abs(value.toDouble());
				amount.group_2_mol = amount.group_2_gram / weight;
				amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
				amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
				RecalculateAndUpdate();
				return true;
			} break;
		case AmountsModelFields::Names::Sum_mol:
			if(role == Qt::EditRole) {
				if(amount.sum_mol > 0.0) {
					auto weight = substance_weight.weight;
					auto val = std::abs(value.toDouble());
					auto coef = val / amount.sum_mol;
					amount.sum_mol = val;
					amount.group_1_mol *= coef;
					amount.group_2_mol *= coef;
					amount.group_1_gram = amount.group_1_mol * weight;
					amount.group_2_gram = amount.group_2_mol * weight;
					amount.sum_gram = amount.group_1_gram + amount.group_2_gram;
					RecalculateAndUpdate();
					return true;
				}
			} break;
		case AmountsModelFields::Names::Sum_gram:
			if(role == Qt::EditRole) {
				if(amount.sum_gram > 0.0) {
					auto weight = substance_weight.weight;
					auto val = std::abs(value.toDouble());
					auto coef = val / amount.sum_gram;
					amount.sum_gram = val;
					amount.group_1_gram *= coef;
					amount.group_2_gram *= coef;
					amount.group_1_mol = amount.group_1_gram / weight;
					amount.group_2_mol = amount.group_2_gram / weight;
					amount.sum_mol = amount.group_1_mol + amount.group_2_mol;
					RecalculateAndUpdate();
					return true;
				}
			} break;
		case AmountsModelFields::Names::Sum_atpct:
		case AmountsModelFields::Names::Sum_wtpct:
			break;
		case AmountsModelFields::Names::Included:
			if(role == Qt::CheckStateRole) {
				auto id = substance_weight.id;
				auto check = value.value<Qt::CheckState>();
				if(check == Qt::Unchecked) {
					excluded.insert(id);
					amount = Amounts{};
				} else {
					excluded.erase(id);
				}
				RecalculateAndUpdate();
				return true;
			}
			break;
		}
	}
	return false;
}

QVariant AmountsModel::headerData(int section, Qt::Orientation orientation,
								  int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			return AmountsModelFields::names.at(section);
		} else {
			return section;
		}
	}
	return QVariant{};
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
		if(index.row() == 0) {
			return flags;
		} else {
			flags |= Qt::ItemIsUserCheckable;
			return flags;
		}
	}
	return flags;
}

bool AmountsModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}

Amounts SumComposition(const Composition& amounts)
{
	auto sum = Amounts{};
	for(const auto& [_, amount] : amounts) {
		sum.group_1_mol		+= amount.group_1_mol;
		sum.group_1_gram	+= amount.group_1_gram;
		sum.group_2_mol		+= amount.group_2_mol;
		sum.group_2_gram	+= amount.group_2_gram;
		sum.sum_mol			+= amount.sum_mol;
		sum.sum_gram		+= amount.sum_gram;
	}
	return sum;
}

void SumRecalculate(Composition& amounts)
{
	auto sum = SumComposition(amounts);
	for(auto&& [_, amount] : amounts) {
		amount.sum_atpct = sum.sum_mol > 0.0 ?
					(100 * amount.sum_mol / sum.sum_mol) : 0.0;
		amount.sum_wtpct = sum.sum_gram > 0.0 ?
					(100 * amount.sum_gram / sum.sum_gram) : 0.0;
	}
}

void AmountsModel::Recalculate()
{
	SumRecalculate(amounts);
	for(const auto& [_, amount] : amounts) {
		sum.sum_atpct += amount.sum_atpct;
		sum.sum_wtpct += amount.sum_wtpct;
	}
}

void AmountsModel::RecalculateAndUpdate()
{
	Recalculate();
	QModelIndex tl = QAbstractTableModel::index
			(0, static_cast<int>(AmountsModelFields::Names::Group_1_mol));
	QModelIndex br = QAbstractTableModel::index
			(row_count, static_cast<int>(AmountsModelFields::Names::Included));
	emit dataChanged(tl, br);
}

