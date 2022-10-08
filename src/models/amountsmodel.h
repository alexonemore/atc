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

#ifndef AMOUNTSMODEL_H
#define AMOUNTSMODEL_H

#include <QAbstractTableModel>
#include <unordered_map>
#include <set>
#include "database.h"

namespace AmountsModelFields {
enum class Names {
	ID,
	Formula,
	Weight,
	Group_1_mol,
	Group_1_gram,
	Group_2_mol,
	Group_2_gram,
	Sum_mol,
	Sum_gram,
	Sum_atpct,
	Sum_wtpct,
	Included
};
extern const QStringList names;
}

struct Amounts {
	double group_1_mol{0};
	double group_1_gram{0};
	double group_2_mol{0};
	double group_2_gram{0};
	double sum_mol{0};
	double sum_gram{0};
	double sum_atpct{0};
	double sum_wtpct{0};
};
using Composition = std::unordered_map<int, Amounts>;

class AmountsModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(AmountsModel)
private:
	SubstanceWeights weights;
	Composition amounts;
	Composition amounts_new;
	std::set<int> excluded;
	Amounts sum{};
	int row_count{1};
	const int col_count{static_cast<int>(AmountsModelFields::names.size())};
public:
	explicit AmountsModel(QObject *parent = nullptr);
	~AmountsModel() override;
	void SetNewData(SubstanceWeights&& new_weights);
	void Clear();

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
	bool CheckIndexValidParent(const QModelIndex& index) const;
	void Recalculate();
	void RecalculateAndUpdate();
};

#endif // AMOUNTSMODEL_H
