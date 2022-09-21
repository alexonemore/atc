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

#ifndef PLOTTFMODEL_H
#define PLOTTFMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include "database.h"

enum class PlotTFModelFields {
	ID,
	Formula,
	G_kJ,
	H_kJ,
	F_J,
	S_J,
	Cp_J,
	c
};

class PlotTFModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(PlotTFModel)
private:
	static const QStringList plot_TF_model_field_names;
	struct Cell {
		QColor color{Qt::white};
		Qt::CheckState checked{Qt::Unchecked};
	};
	struct Row {
		Cell G, H, F, S, Cp, c;
	};
private:
	SubstanceNames data_names;
	// using id_type = decltype(SubstanceNames::value_type::id);
	std::unordered_map<int, Row> data_tf; // int = id from data_names
	std::unordered_map<int, Row> data_tf_new;
	int row_count{0};
	const int col_count{plot_TF_model_field_names.size()};
public:
	explicit PlotTFModel(QObject *parent = nullptr);
	~PlotTFModel() override;
	void SetNewData(SubstanceNames&& data);
	void Clear();

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role) const override;
};

#endif // PLOTTFMODEL_H
