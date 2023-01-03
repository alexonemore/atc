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

#ifndef SUBSTANCESTABLEMODEL_H
#define SUBSTANCESTABLEMODEL_H

#include <QAbstractTableModel>
#include "database.h"

enum class SubstanceFields {
	ID,
	Formula,
	Name,
	Weight,
	T_min,
	T_max
};

class SubstancesTableModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(SubstancesTableModel)
private:
	static const QStringList substances_field_names;
	SubstancesData data_;
	int row_count{0};
	const int col_count{static_cast<int>(substances_field_names.size())};
public:
	explicit SubstancesTableModel(QObject *parent = nullptr);
	~SubstancesTableModel() override;
	void SetNewData(SubstancesData&& new_data);
	void Clear();

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role) const override;
};

#endif // SUBSTANCESTABLEMODEL_H
