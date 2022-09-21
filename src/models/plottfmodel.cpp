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

const QStringList PlotTFModel::plot_TF_model_field_names = {
	QStringLiteral("ID"),
	QStringLiteral("Formula"),
	QStringLiteral("G"),
	QStringLiteral("H"),
	QStringLiteral("F"),
	QStringLiteral("S"),
	QStringLiteral("Cp"),
	QStringLiteral("c")
};

PlotTFModel::PlotTFModel(QObject *parent)
	: QAbstractTableModel(parent)
{

}

PlotTFModel::~PlotTFModel()
{

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
	auto col = static_cast<PlotTFModelFields>(index.column());
	auto&& data_name = data_names.at(index.row());
	if(role == Qt::DisplayRole) {
		switch(col) {
		case PlotTFModelFields::ID:			return data_name.id;
		case PlotTFModelFields::Formula:	return data_name.formula;
		default:							return QVariant{};
		}
	}
	auto&& at_id = data_tf.at(data_name.id);
	if(role == Qt::CheckStateRole) {
		switch(col) {
		case PlotTFModelFields::ID:
		case PlotTFModelFields::Formula:	return QVariant{};
		case PlotTFModelFields::G_kJ:		return at_id.G.checked;
		case PlotTFModelFields::H_kJ:		return at_id.H.checked;
		case PlotTFModelFields::F_J:		return at_id.F.checked;
		case PlotTFModelFields::S_J:		return at_id.S.checked;
		case PlotTFModelFields::Cp_J:		return at_id.Cp.checked;
		case PlotTFModelFields::c:			return at_id.c.checked;
		}
	}
	if(role == Qt::BackgroundRole || role == Qt::EditRole) {
		switch(col) {
		case PlotTFModelFields::ID:
		case PlotTFModelFields::Formula:	return QVariant{};
		case PlotTFModelFields::G_kJ:		return QBrush(at_id.G.color);
		case PlotTFModelFields::H_kJ:		return QBrush(at_id.H.color);
		case PlotTFModelFields::F_J:		return QBrush(at_id.F.color);
		case PlotTFModelFields::S_J:		return QBrush(at_id.S.color);
		case PlotTFModelFields::Cp_J:		return QBrush(at_id.Cp.color);
		case PlotTFModelFields::c:			return QBrush(at_id.c.color);
		}
	}
	return QVariant{};
}

bool PlotTFModel::setData(const QModelIndex& index, const QVariant& value,
						  int role)
{
	if(!CheckIndexValidParent(index)) return false;
	auto&& substance = data_names.at(index.row());
	auto column = static_cast<PlotTFModelFields>(index.column());
	auto&& cell = GetCell(substance.id, column);
	auto graph_id = MakeGraphId(substance.id, column);
	if(role == Qt::CheckStateRole) {
		if(cell.color == Qt::white && cell.checked != Qt::Checked) {
			cell.color = GetRandomColor();
		}
		cell.checked = value.value<Qt::CheckState>();
		if(cell.checked == Qt::CheckState::Checked) {
			auto name = MakeGraphName(substance.formula, column);
			LOG(name)
			emit AddGraph(graph_id, name, cell.color);
		} else {
			cell.color = Qt::white;
			emit RemoveGraph(graph_id);
		}
	} else if(role == Qt::EditRole) {
		cell.color = value.value<QColor>();
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
	switch(static_cast<PlotTFModelFields>(index.column())) {
	case PlotTFModelFields::ID:
	case PlotTFModelFields::Formula:
		return flags;
	default:
		flags |= Qt::ItemIsUserCheckable;
		flags |= Qt::ItemIsEditable;
		flags ^= Qt::ItemIsSelectable;
		return  flags;
	}
}

bool PlotTFModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}

PlotTFModel::Cell& PlotTFModel::GetCell(const int id,
										const PlotTFModelFields column)
{
	auto&& at_id = data_tf.at(id);
	switch(column) {
	case PlotTFModelFields::ID:
	case PlotTFModelFields::Formula: {
#ifndef NDEBUG
		LOG(id, column)
		assert(false && "Try to get cell to ID or Formula column");
		static Cell error;
		return error;
#else
		throw std::runtime_error("Try to get cell to ID or Formula column");
#endif
	}
	case PlotTFModelFields::G_kJ:	return at_id.G;
	case PlotTFModelFields::H_kJ:	return at_id.H;
	case PlotTFModelFields::F_J:	return at_id.F;
	case PlotTFModelFields::S_J:	return at_id.S;
	case PlotTFModelFields::Cp_J:	return at_id.Cp;
	case PlotTFModelFields::c:		return at_id.c;
	}
}

GraphId PlotTFModel::MakeGraphId(const int id,
								 const PlotTFModelFields column) const
{
	static_assert(std::is_same_v<GraphId, int>, "");
	assert(plot_TF_model_field_names.size() < 10);
	constexpr int n = 10; // if size above > 10 then 100
	return (id * n + static_cast<int>(column));
}

QString PlotTFModel::MakeGraphName(const QString& formula,
								   const PlotTFModelFields column) const
{
	return (formula + QStringLiteral(" <") +
			plot_TF_model_field_names.at(static_cast<int>(column)) +
			QStringLiteral(">"));
}
