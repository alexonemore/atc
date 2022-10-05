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

#include "plottfmodel.h"
#include "utilities.h"
#include <QBrush>

namespace PlotTFModelFields {
const QStringList names{
	QT_TR_NOOP("ID"),
	QT_TR_NOOP("Formula")
};
};

const QStringList PlotTFModel::plot_TF_model_field_names =
		PlotTFModelFields::names + ParametersNS::thermodynamic_function;


PlotTFModel::PlotTFModel(QObject *parent)
	: QAbstractTableModel(parent)
{

}

PlotTFModel::~PlotTFModel()
{

}

void PlotTFModel::SetDatabase(const ParametersNS::Database new_database)
{
	database = new_database;
}

void PlotTFModel::SetNewData(SubstanceNames&& data)
{
	beginResetModel();
	data_names = std::move(data);
	for(auto i = data_names.cbegin(), end = data_names.cend(); i != end; ++i) {
		data_tf_new.emplace(i->id, Row{});
	}
	assert(data_names.size() == static_cast<int>(data_tf_new.size()));

	if(data_tf_new.size() > data_tf.size()) {
		data_tf.merge(data_tf_new);
	} else {
		data_tf_new.merge(data_tf);
	}
	assert(data_names.size() == static_cast<int>(data_tf.size()));
	data_tf_new.clear();
	row_count = data_names.size();
	endResetModel();
}

void PlotTFModel::Clear()
{
	beginResetModel();
	data_names.clear();
	data_tf.clear();
	row_count = 0;
	endResetModel();
}

int PlotTFModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int PlotTFModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
}

QVariant PlotTFModel::data(const QModelIndex& index, int role) const
{
	if(!CheckIndexValidParent(index)) return QVariant{};
	auto col = index.column();
	auto&& data_name = data_names.at(index.row());
	if(col < field_names_size) {
		if(role == Qt::DisplayRole) {
			switch(static_cast<PlotTFModelFields::Names>(col)) {
			case PlotTFModelFields::Names::ID:		return data_name.id;
			case PlotTFModelFields::Names::Formula:	return data_name.formula;
			}
		}
	} else {
		auto&& at_id = data_tf.at(data_name.id);
		auto tf = static_cast<PlotTFModelFields::TF>(col - field_names_size);
		if(role == Qt::CheckStateRole) {
			switch(tf) {
			case PlotTFModelFields::TF::G_kJ:	return at_id.G.checked;
			case PlotTFModelFields::TF::H_kJ:	return at_id.H.checked;
			case PlotTFModelFields::TF::F_J:	return at_id.F.checked;
			case PlotTFModelFields::TF::S_J:	return at_id.S.checked;
			case PlotTFModelFields::TF::Cp_J:	return at_id.Cp.checked;
			case PlotTFModelFields::TF::c:		return at_id.c.checked;
			}
		}
		if(role == Qt::BackgroundRole || role == Qt::EditRole) {
			switch(tf) {
			case PlotTFModelFields::TF::G_kJ:	return QBrush(at_id.G.color);
			case PlotTFModelFields::TF::H_kJ:	return QBrush(at_id.H.color);
			case PlotTFModelFields::TF::F_J:	return QBrush(at_id.F.color);
			case PlotTFModelFields::TF::S_J:	return QBrush(at_id.S.color);
			case PlotTFModelFields::TF::Cp_J:	return QBrush(at_id.Cp.color);
			case PlotTFModelFields::TF::c:		return QBrush(at_id.c.color);
			}
		}
	}
	return QVariant{};
}

bool PlotTFModel::setData(const QModelIndex& index, const QVariant& value,
						  int role)
{
	if(!CheckIndexValidParent(index)) return false;
	auto&& substance = data_names.at(index.row());
	auto tf = static_cast<PlotTFModelFields::TF>(index.column() -
												 field_names_size);
	auto&& cell = GetCell(substance.id, tf);
	auto graph_id = MakeGraphId(substance.id, tf);
	if(role == Qt::CheckStateRole) {
		if(cell.color == Qt::white && cell.checked != Qt::Checked) {
			cell.color = GetRandomColor();
		}
		cell.checked = value.value<Qt::CheckState>();
		if(cell.checked == Qt::CheckState::Checked) {
			auto name = MakeGraphName(substance.formula, tf);
			LOG(name)
			emit AddGraph(graph_id, name, cell.color);
		} else {
			cell.color = Qt::white;
			emit RemoveGraph(graph_id);
		}
	} else if(role == Qt::EditRole) {
		cell.color = value.value<QColor>();
		LOG(cell.color)
		emit ChangeColorGraph(graph_id, cell.color);
	} else {
		return false;
	}
	emit dataChanged(index, index);
	return true;
}

QVariant PlotTFModel::headerData(int section, Qt::Orientation orientation,
								 int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			return plot_TF_model_field_names.at(section);
		} else {
			return section;
		}
	} else {
		return QVariant{};
	}
}

Qt::ItemFlags PlotTFModel::flags(const QModelIndex& index) const
{
	if(!CheckIndexValidParent(index)) return Qt::ItemFlags{};
	Qt::ItemFlags flags = QAbstractTableModel::flags(index);
	auto col = index.column();
	if(col < field_names_size) {
		return flags;
	} else {
		flags |= Qt::ItemIsUserCheckable;
		flags |= Qt::ItemIsEditable;
		flags ^= Qt::ItemIsSelectable;
		return  flags;
	}
}

void PlotTFModel::SlotRemoveAllGraphs()
{
	beginResetModel();
	data_tf.clear();
	endResetModel();
}

void PlotTFModel::SlotRemoveOneGraph(const GraphId id)
{
	if(id.database != database) return;
	auto row = data_tf.find(id.substance_id);
	if(row == data_tf.end()) return;
	auto&& cell = row->second;
	switch(id.thermodynamic_function) {
	case PlotTFModelFields::TF::G_kJ:	cell.G = Cell{}; break;
	case PlotTFModelFields::TF::H_kJ:	cell.H = Cell{}; break;
	case PlotTFModelFields::TF::F_J:	cell.F = Cell{}; break;
	case PlotTFModelFields::TF::S_J:	cell.S = Cell{}; break;
	case PlotTFModelFields::TF::Cp_J:	cell.Cp = Cell{}; break;
	case PlotTFModelFields::TF::c:		cell.c = Cell{}; break;
	}
	auto index = GetIndex(id);
	assert(CheckIndexValidParent(index));
	emit dataChanged(index, index);
}

void PlotTFModel::SlotRemoveGraphs(const QVector<GraphId>& ids)
{
	for(auto&& id : ids) {
		SlotRemoveOneGraph(id);
	}
}

void PlotTFModel::SlotChangeColotGraph(const GraphId id, const QColor& color)
{
	if(id.database != database) return;
	auto row = data_tf.find(id.substance_id);
	if(row == data_tf.end()) return;
	auto&& cell = row->second;
	switch(id.thermodynamic_function) {
	case PlotTFModelFields::TF::G_kJ:	cell.G.color = color; break;
	case PlotTFModelFields::TF::H_kJ:	cell.H.color = color; break;
	case PlotTFModelFields::TF::F_J:	cell.F.color = color; break;
	case PlotTFModelFields::TF::S_J:	cell.S.color = color; break;
	case PlotTFModelFields::TF::Cp_J:	cell.Cp.color = color; break;
	case PlotTFModelFields::TF::c:		cell.c.color = color; break;
	}
	auto index = GetIndex(id);
	assert(CheckIndexValidParent(index));
	emit dataChanged(index, index);
}

bool PlotTFModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}

PlotTFModel::Cell& PlotTFModel::GetCell(const int id,
										const PlotTFModelFields::TF tf)
{
	auto&& at_id = data_tf.at(id);
	switch(tf) {
	case PlotTFModelFields::TF::G_kJ:	return at_id.G;
	case PlotTFModelFields::TF::H_kJ:	return at_id.H;
	case PlotTFModelFields::TF::F_J:	return at_id.F;
	case PlotTFModelFields::TF::S_J:	return at_id.S;
	case PlotTFModelFields::TF::Cp_J:	return at_id.Cp;
	case PlotTFModelFields::TF::c:		return at_id.c;
	}
	throw std::runtime_error("PlotTFModel::GetCell");
}

GraphId PlotTFModel::MakeGraphId(const int id,
								 const PlotTFModelFields::TF tf) const
{
	return GraphId{id, tf, database};
}

QString PlotTFModel::MakeGraphName(const QString& formula,
								   const PlotTFModelFields::TF tf) const
{
	return (formula + QStringLiteral(" <") +
			ParametersNS::thermodynamic_function.at(static_cast<int>(tf)) +
			QStringLiteral("> {") +
			ParametersNS::databases.at(static_cast<int>(database)) +
			QStringLiteral("}"));
}

QModelIndex PlotTFModel::GetIndex(const GraphId& id) const
{
	auto find = std::find_if(data_names.cbegin(), data_names.cend(),
				 [id](const auto& i){
		return i.id == id.substance_id;
	});
	auto row = std::distance(data_names.cbegin(), find);
	auto col = static_cast<int>(id.thermodynamic_function) + field_names_size;
	return index(row, col);
}
