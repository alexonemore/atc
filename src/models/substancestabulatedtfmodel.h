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

#ifndef SUBSTANCESTABULATEDTFMODEL_H
#define SUBSTANCESTABULATEDTFMODEL_H

#include <QAbstractTableModel>
#include "database.h"

namespace SubstancesTabulatedTFFields {
enum class Names {
	T
};
extern const QStringList names;
using TF = ParametersNS::ThermodynamicFunction;
}

class SubstancesTabulatedTFModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(SubstancesTabulatedTFModel)
private:
	SubstancesTabulatedTFData data_;
	int row_count{0};
	int col_count;
private:
	QStringList substance_tabulated_tf_field_names;
public:
	explicit SubstancesTabulatedTFModel(QObject *parent = nullptr);
	~SubstancesTabulatedTFModel() override;
	void SetNewData(SubstancesTabulatedTFData&& new_data);
	void Clear();

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role) const override;
};

#endif // SUBSTANCESTABULATEDTFMODEL_H
