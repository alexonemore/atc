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

#ifndef PLOTTFMODEL_H
#define PLOTTFMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include "database.h"
#include "plots.h"
#include "parameters.h"

namespace PlotTFModelFields {
enum class Names {
	ID,
	Formula
};
extern const QStringList names;
using TF = ParametersNS::ThermodynamicFunction;
};

class PlotTFModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(PlotTFModel)
private:
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
	const int col_count;
	ParametersNS::Database database;
private:
	QStringList plot_TF_model_field_names;
	const int field_names_size{static_cast<int>(PlotTFModelFields::names.size())};
public:
	explicit PlotTFModel(QObject *parent = nullptr);
	~PlotTFModel() override;
	void SetDatabase(const ParametersNS::Database new_database);
	void SetNewData(SubstanceNames&& data);
	void Clear();

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

public slots:
	void SlotRemoveAllGraphs();
	void SlotRemoveOneGraph(const GraphId id);
	void SlotRemoveGraphs(const QVector<GraphId>& ids);
	void SlotChangeColotGraph(const GraphId id, const QColor& color);

signals:
	void AddGraph(const GraphId id, const QString& name, const QColor& color);
	void RemoveGraph(const GraphId id);
	void ChangeColorGraph(const GraphId id, const QColor& color);

private:
	bool CheckIndexValidParent(const QModelIndex& index) const;
	Cell& GetCell(const int id, const PlotTFModelFields::TF tf) &;
	GraphId MakeGraphId(const int id, const PlotTFModelFields::TF tf) const;
	QString MakeGraphName(const QString& formula,
						  const PlotTFModelFields::TF tf) const;
	QModelIndex GetIndex(const GraphId& id) const;
};


#endif // PLOTTFMODEL_H
