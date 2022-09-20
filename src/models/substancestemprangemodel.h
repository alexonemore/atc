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

#ifndef SUBSTANCESTEMPRANGEMODEL_H
#define SUBSTANCESTEMPRANGEMODEL_H

#include <QAbstractTableModel>
#include "database.h"

enum class SubstanceTempRangeFields {
	T_min, T_max,
	H, S,
	f1, f2, f3, f4, f5, f6, f7,
	phase
};

class SubstancesTempRangeModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(SubstancesTempRangeModel)
private:
	static const QStringList substances_temprange_field_names;
	SubstanceTempRangeData data_;
	const int row_count{substances_temprange_field_names.size()};
	int col_count{0};
public:
	explicit SubstancesTempRangeModel(QObject *parent = nullptr);
	~SubstancesTempRangeModel() override;
	void SetNewData(SubstanceTempRangeData&& new_data);
	void Clear();

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role) const override;
};

#endif // SUBSTANCESTEMPRANGEMODEL_H
