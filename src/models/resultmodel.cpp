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

#include "resultmodel.h"
#include "utilities.h"
#include "thermodynamics.h"
#include <QBrush>

namespace ResultFields {
const QStringList col_names{
	QT_TR_NOOP("ID"),
	QT_TR_NOOP("Name"),
	QT_TR_NOOP("Show")
};
const int col_names_size = static_cast<int>(col_names.size());

const QStringList row_names{
	QT_TR_NOOP("T %1"),
	QT_TR_NOOP("T initial"),
	QT_TR_NOOP("H initial"),
	QT_TR_NOOP("H equilibrium"),
	QT_TR_NOOP("c equilibrium"),
	QT_TR_NOOP("Sum")
};
const int row_names_size = static_cast<int>(row_names.size());

const QStringList detail_row_names_single{
	QT_TR_NOOP("T units"),
	QT_TR_NOOP("T %1"),
	QT_TR_NOOP("T initial"),
	QT_TR_NOOP("H initial"),
	QT_TR_NOOP("H equilibrium"),
	QT_TR_NOOP("c equilibrium"),
	QT_TR_NOOP("Sum units"),
	QT_TR_NOOP("Sum")
};
const int detail_row_names_single_size = static_cast<int>(detail_row_names_single.size());

const QStringList detail_row_names_1d{
	QT_TR_NOOP("%1"),
	QT_TR_NOOP("T %1"),
	QT_TR_NOOP("T initial"),
	QT_TR_NOOP("H initial"),
	QT_TR_NOOP("H equilibrium"),
	QT_TR_NOOP("c equilibrium"),
	QT_TR_NOOP("Sum")
};
const int detail_row_names_1d_size = static_cast<int>(detail_row_names_1d.size());

const QStringList detail_row_names_2d{
	QT_TR_NOOP("T initial"),
	QT_TR_NOOP("Composition"),
	QT_TR_NOOP("T %1"),
	QT_TR_NOOP("H initial"),
	QT_TR_NOOP("H equilibrium"),
	QT_TR_NOOP("c equilibrium"),
	QT_TR_NOOP("Sum")
};
const int detail_row_names_2d_size = static_cast<int>(detail_row_names_2d.size());

const QColor green{109, 255, 109};
const QColor red{255, 109, 109};
const QColor red2{230, 186, 192};
} // namespace ResultFields

ResultModel::ResultModel(QObject *parent)
	: QAbstractTableModel{parent}
{

}

ResultModel::~ResultModel()
{

}

void ResultModel::SetNewData(const SubstanceWeights* vec,
							 const ParametersNS::Parameters& params)
{
	beginResetModel();
	items = vec;
	row_count = items->size() + ResultFields::row_names_size;
	parameters = params;
	checked.clear();
	for(int i = 0; i != row_count; ++i) {
		checked.emplace(i, Cell{});
	}
	endResetModel();
}

void ResultModel::Clear()
{
	beginResetModel();
	row_count = 0;
	items = nullptr;
	checked.clear();
	endResetModel();
}

void ResultModel::UpdateParameters(const ParametersNS::Parameters& params)
{
	LOG()
	beginResetModel();
	parameters.temperature_result_unit = params.temperature_result_unit;
	parameters.composition_result_unit = params.composition_result_unit;
	parameters.show_initial_in_result = params.show_initial_in_result;
	endResetModel();
}

int ResultModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int ResultModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return ResultFields::col_names_size;
	}
}

QVariant ResultModel::data(const QModelIndex& index, int role) const
{
	if(!CheckIndexValidParent(index)) return QVariant{};
	auto col = static_cast<ResultFields::ColNames>(index.column());
	auto row = index.row();

	if(role == Qt::BackgroundRole || role == Qt::EditRole) {
		switch(col) {
		case ResultFields::ColNames::Show: {
			return checked.at(row).color;
		}
		default:
			break;
		}
		return QBrush{Qt::white};
	}

	switch (col) {
	case ResultFields::ColNames::ID:
		if(role == Qt::DisplayRole) {
			if(row >= ResultFields::row_names_size) {
				return items->at(row - ResultFields::row_names_size).id;
			}
		}
		break;
	case ResultFields::ColNames::Name:
		if(role == Qt::DisplayRole) {
			if(row < ResultFields::row_names_size) {
				if(row == 0) {
					switch(parameters.target) {
					case ParametersNS::Target::Equilibrium:
						return ResultFields::row_names.at(row).arg(tr("equilibrium"));
					case ParametersNS::Target::AdiabaticTemperature:
						return ResultFields::row_names.at(row).arg(tr("adiabatic"));
					}
				} else {
					return ResultFields::row_names.at(row);
				}
			} else {
				return items->at(row - ResultFields::row_names_size).formula;
			}
		}
		break;
	case ResultFields::ColNames::Show:
		if(role == Qt::CheckStateRole) {
			return checked.at(row).checked;
		}
		break;
	}

	return QVariant{};
}

bool ResultModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	LOG()
	if(!CheckIndexValidParent(index)) return false;
	if(parameters.workmode == ParametersNS::Workmode::SinglePoint) {
		return false;
	}
	auto row = index.row();
	auto&& cell = checked.at(row);
	auto graph_id = RowToGraphId(row);
	if(parameters.workmode == ParametersNS::Workmode::TemperatureCompositionRange) {
		if(role == Qt::CheckStateRole) {
			QVector<GraphId> ids;
			for(auto i = checked.begin(), end = checked.end(); i != end; ++i) {
				if(i->second.checked == Qt::Checked) {
					ids.push_back(RowToGraphId(i->first));
				}
				i->second = Cell{};
			}
			cell.color = GetRandomColor();
			cell.checked = value.value<Qt::CheckState>();
			if(cell.checked == Qt::CheckState::Checked) {
				auto name = MakeGraphName(row);
				LOG("AddGraph", name, cell.checked, cell.color)
				emit AddGraph(graph_id, name, cell.color);
				emit RemoveGraphs(ids);
			} else {
				cell.color = Qt::white;
				LOG("RemoveGraph")
				emit RemoveGraph(graph_id);
			}
		} else {
			return false;
		}
	} else {
		if(role == Qt::CheckStateRole) {
			if(cell.color == Qt::white && cell.checked != Qt::Checked) {
				cell.color = GetRandomColor();
			}
			cell.checked = value.value<Qt::CheckState>();
			if(cell.checked == Qt::CheckState::Checked) {
				auto name = MakeGraphName(row);
				LOG("AddGraph", name, cell.checked, cell.color)
				emit AddGraph(graph_id, name, cell.color);
			} else {
				cell.color = Qt::white;
				LOG("RemoveGraph")
				emit RemoveGraph(graph_id);
			}
		} else if(role == Qt::EditRole) {
			cell.color = value.value<QColor>();
			LOG("ChangeColorGraph", cell.color)
			emit ChangeColorGraph(graph_id, cell.color);
		} else {
			return false;
		}
	}
	QModelIndex tl = QAbstractTableModel::index(0, index.column());
	QModelIndex br = QAbstractTableModel::index(row_count, index.column());
	emit dataChanged(tl, br);
//	emit dataChanged(index, index);
	return true;
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			return ResultFields::col_names.at(section);
		} else {
			return section;
		}
	} else {
		return QVariant{};
	}
}

Qt::ItemFlags ResultModel::flags(const QModelIndex& index) const
{
	if(!CheckIndexValidParent(index)) return Qt::ItemFlags{};
	Qt::ItemFlags flags = QAbstractTableModel::flags(index);
	auto col = static_cast<ResultFields::ColNames>(index.column());
	switch(col) {
	case ResultFields::ColNames::Show:
		flags |= Qt::ItemIsUserCheckable;
		flags |= Qt::ItemIsEditable;
		flags ^= Qt::ItemIsSelectable;
		return  flags;
	default:
		return flags;
	}
}

bool ResultModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}

int ResultModel::GraphIdToRow(const GraphId& graph_id) const
{
	if(graph_id.substance_id > 0) {
		assert(graph_id.option == -1);
		auto find = std::find_if(items->cbegin(), items->cend(),
								 [id = graph_id.substance_id](SubstanceWeights::const_reference sub){
			return sub.id == id;
		});
		assert(find != items->cend());
		return std::distance(items->cbegin(), find) + ResultFields::row_names_size;
	} else {
		assert(graph_id.option >= 0);
		return graph_id.option;
	}
}

GraphId ResultModel::RowToGraphId(const int row) const
{
	assert(row < row_count);
	GraphId graph_id;
	if(row < ResultFields::row_names_size) {
		graph_id.substance_id = -1;
		graph_id.option = row;
		graph_id.database = static_cast<int>(parameters.database);
	} else {
		graph_id.substance_id = items->at(row - ResultFields::row_names_size).id;
		graph_id.option = -1;
		graph_id.database = static_cast<int>(parameters.database);
	}
	return graph_id;
}

QString ResultModel::MakeGraphName(const int row) const
{
	auto res = QStringLiteral("%1, %2");
	if(row < ResultFields::row_names_size) {
		QString s1;
		if(row == 0) {
			switch(parameters.target) {
			case ParametersNS::Target::Equilibrium:
				s1 = ResultFields::row_names.at(row).arg(tr("equilibrium"));
				break;
			case ParametersNS::Target::AdiabaticTemperature:
				s1 = ResultFields::row_names.at(row).arg(tr("adiabatic"));
				break;
			}
		} else {
			s1 = ResultFields::row_names.at(row);
		}
		return res.arg(s1);
	} else {
		return res.arg(items->at(row - ResultFields::row_names_size).formula);
	}
}

void ResultModel::SlotRemoveAllGraphs()
{
	beginResetModel();
	for(auto&& i : checked) {
		i.second = Cell{};
	}
	endResetModel();
}

void ResultModel::SlotRemoveOneGraph(const GraphId id)
{
	auto row = GraphIdToRow(id);
	checked.at(row).checked = Qt::Unchecked;
	auto ind = index(row, static_cast<int>(ResultFields::ColNames::Show));
	emit dataChanged(ind, ind);
}

void ResultModel::SlotRemoveGraphs(const QVector<GraphId>& ids)
{
	for(auto&& id : ids) {
		SlotRemoveOneGraph(id);
	}
}

void ResultModel::SlotChangeColorGraph(const GraphId id, const QColor& color)
{
	auto row = GraphIdToRow(id);
	checked.at(row).color = color;
	auto ind = index(row, static_cast<int>(ResultFields::ColNames::Show));
	emit dataChanged(ind, ind);
}

ResultDetailModel::ResultDetailModel(QObject* parent)
	: QAbstractTableModel{parent}
	, col_count{ResultFields::detail_row_names_single_size}
{

}

ResultDetailModel::~ResultDetailModel()
{

}

void ResultDetailModel::SetNewData(const Optimization::OptimizationVector* vec,
								   const ParametersNS::Parameters& params,
								   const int x_size, const int y_size)
{
	LOG()
	beginResetModel();
	items = vec;
	parameters = params;
	row_count = items->cbegin()->number.substances;
	switch (parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		assert(vec->size() == x_size);
		assert(vec->size() == 1);
		row_count += ResultFields::detail_row_names_single_size;
		col_count = ParametersNS::composition_units.size();
		break;
	case ParametersNS::Workmode::TemperatureRange:
	case ParametersNS::Workmode::CompositionRange:
		assert(vec->size() == x_size);
		row_count += ResultFields::detail_row_names_1d_size;
		col_count = 1 + x_size; // +1 for Units
		break;
	case ParametersNS::Workmode::TemperatureCompositionRange:
		assert(vec->size() == x_size * y_size);
		row_count += ResultFields::detail_row_names_2d_size;
		col_count = x_size * y_size + 1; // +1 for Units
		break;
	}
	endResetModel();
}

void ResultDetailModel::UpdateParameters(const ParametersNS::Parameters& params)
{
	LOG()
	beginResetModel();
	parameters.temperature_result_unit = params.temperature_result_unit;
	parameters.composition_result_unit = params.composition_result_unit;
	parameters.show_initial_in_result = params.show_initial_in_result;
	endResetModel();
}

void ResultDetailModel::Clear()
{
	LOG()
	beginResetModel();
	row_count = 0;
	col_count = 0;
	items = nullptr;
	endResetModel();
}

bool ResultDetailModel::CheckIndexValidParent(const QModelIndex& index) const
{
	return checkIndex(index,
					  QAbstractItemModel::CheckIndexOption::IndexIsValid |
					  QAbstractItemModel::CheckIndexOption::ParentIsInvalid);
}

int ResultDetailModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return row_count;
	}
}

int ResultDetailModel::columnCount(const QModelIndex& parent) const
{
	if(parent.isValid()) {
		return 0;
	} else {
		return col_count;
	}
}

QVariant ResultDetailModel::data(const QModelIndex& index, int role) const
{
	if(!CheckIndexValidParent(index)) return QVariant{};
	if(items == nullptr) return QVariant{};
	switch (parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		return DataSingle(index, role);
	case ParametersNS::Workmode::TemperatureRange:
	case ParametersNS::Workmode::CompositionRange:
		return Data1D(index, role);
	case ParametersNS::Workmode::TemperatureCompositionRange:
		return Data2D(index, role);
	}
	return QVariant{};
}

QVariant ResultDetailModel::DataSingle(const QModelIndex& index, int role) const
{
	auto col = index.column();
	auto row = index.row();
	if(role == Qt::BackgroundRole) {
		switch (static_cast<ResultFields::DetailRowNamesSingle>(row)) {
		case ResultFields::DetailRowNamesSingle::T_units:
			break;
		case ResultFields::DetailRowNamesSingle::T_result:
			return QBrush{ResultFields::green};
		case ResultFields::DetailRowNamesSingle::T_initial:
		case ResultFields::DetailRowNamesSingle::H_initial:
		case ResultFields::DetailRowNamesSingle::H_equilibrium:
		case ResultFields::DetailRowNamesSingle::c_equilibrium:
			break;
		case ResultFields::DetailRowNamesSingle::Sum_units:
		case ResultFields::DetailRowNamesSingle::Sum_value:
			return QBrush{Qt::lightGray};
		default:
			break;
		}
		return QBrush{Qt::white};
	}
	if(role == Qt::DisplayRole) {
		switch (static_cast<ResultFields::DetailRowNamesSingle>(row)) {
		case ResultFields::DetailRowNamesSingle::T_units:
			switch (col) {
			case 0: return tr("K");
			case 1: return tr("C");
			case 2: return tr("F");
			default: break;
			}
			break;
		case ResultFields::DetailRowNamesSingle::T_result:
			switch (col) {
			case 0:
				return items->cbegin()->temperature_K_current;
			case 1:
				return Thermodynamics::FromKelvin(items->cbegin()->temperature_K_current,
												  ParametersNS::TemperatureUnit::Celsius);
			case 2:
				return Thermodynamics::FromKelvin(items->cbegin()->temperature_K_current,
												  ParametersNS::TemperatureUnit::Fahrenheit);
			}
			break;
		case ResultFields::DetailRowNamesSingle::T_initial:
			switch (col) {
			case 0:
				return items->cbegin()->temperature_K_initial;
			case 1:
				return Thermodynamics::FromKelvin(items->cbegin()->temperature_K_initial,
												  ParametersNS::TemperatureUnit::Celsius);
			case 2:
				return Thermodynamics::FromKelvin(items->cbegin()->temperature_K_initial,
												  ParametersNS::TemperatureUnit::Fahrenheit);
			}
			break;
		case ResultFields::DetailRowNamesSingle::H_initial:
			switch (col) {
			case 0: return items->cbegin()->H_initial;
			case 1: return tr("[kJ/mol]");
			}
			break;
		case ResultFields::DetailRowNamesSingle::H_equilibrium:
			switch (col) {
			case 0: return items->cbegin()->H_current;
			case 1: return tr("[kJ/mol]");
			}
			break;
		case ResultFields::DetailRowNamesSingle::c_equilibrium:
			switch (col) {
			case 0: return items->cbegin()->result_of_optimization;
			case 1: return tr("[G/RT]");
			}
			break;
		case ResultFields::DetailRowNamesSingle::Sum_units:
			switch (col) {
			case 0: return tr("mol");
			case 1: return tr("gram");
			case 2: return tr("at.%");
			case 3: return tr("wt.%");
			default: break;
			}
			break;
		case ResultFields::DetailRowNamesSingle::Sum_value:
			const auto& sum = parameters.show_initial_in_result
					? items->cbegin()->sum_of_initial
					: items->cbegin()->sum_of_equilibrium;
			switch (col) {
			case 0: return sum.sum_mol;
			case 1: return sum.sum_gram;
			case 2: return sum.sum_atpct;
			case 3: return sum.sum_wtpct;
			}
			break;
		}
		if(row >= ResultFields::detail_row_names_single_size) {
			auto i = row - ResultFields::detail_row_names_single_size;
			auto first = items->cbegin();
			auto id = first->weights.at(i).id;
			const auto& val = parameters.show_initial_in_result
					? first->amounts.at(id)
					: first->amounts_of_equilibrium.at(id);
			switch (col) {
			case 0: return val.sum_mol;
			case 1: return val.sum_gram;
			case 2: return val.sum_atpct;
			case 3: return val.sum_wtpct;
			}
		}
	}
	return QVariant{};
}

QVariant ResultDetailModel::Data1D(const QModelIndex& index, int role) const
{
	auto col = index.column();
	auto row = index.row();
	if(role == Qt::BackgroundRole) {
		switch (static_cast<ResultFields::DetailRowNames1D>(row)) {
		case ResultFields::DetailRowNames1D::X_Axis_values:
			return QBrush{ResultFields::red};
		case ResultFields::DetailRowNames1D::T_result:
			return QBrush{ResultFields::green};
		case ResultFields::DetailRowNames1D::T_initial:
		case ResultFields::DetailRowNames1D::H_initial:
		case ResultFields::DetailRowNames1D::H_equilibrium:
		case ResultFields::DetailRowNames1D::c_equilibrium:
			break;
		case ResultFields::DetailRowNames1D::Sum_value:
			return QBrush{Qt::lightGray};
		default:
			break;
		}
		return QBrush{Qt::white};
	}
	if(role == Qt::DisplayRole) {
		switch (static_cast<ResultFields::DetailRowNames1D>(row)) {
		case ResultFields::DetailRowNames1D::X_Axis_values:
			if(col == 0) {
				switch (parameters.workmode) {
				case ParametersNS::Workmode::TemperatureRange:
					return parameters.GetTemperatureResultUnit();
				case ParametersNS::Workmode::CompositionRange:
					return parameters.GetCompositionRangeUnit();
				default:
					break;
				}
			}
			if(col >= 1) {
				auto i = col - 1;
				switch (parameters.workmode) {
				case ParametersNS::Workmode::TemperatureRange:
					return Thermodynamics::FromKelvin(items->at(i).temperature_K_initial,
													  parameters.temperature_result_unit);
				case ParametersNS::Workmode::CompositionRange:
					return items->at(i).composition_variable;
				default:
					break;
				}
			}
			break;
		case ResultFields::DetailRowNames1D::T_result:
			if(col == 0) {
				return parameters.GetTemperatureResultUnit();
			}
			if(col >= 1) {
				auto i = col - 1;
				return Thermodynamics::FromKelvin(items->at(i).temperature_K_current,
												  parameters.temperature_result_unit);
			}
			break;
		case ResultFields::DetailRowNames1D::T_initial:
			if(col == 0) {
				return parameters.GetTemperatureResultUnit();
			}
			if(col >= 1) {
				auto i = col - 1;
				return Thermodynamics::FromKelvin(items->at(i).temperature_K_initial,
												  parameters.temperature_result_unit);
			}
			break;
		case ResultFields::DetailRowNames1D::H_initial:
			if(col == 0) {
				return tr("[kJ/mol]");
			}
			if(col >= 1) {
				auto i = col - 1;
				return items->at(i).H_initial;
			}
			break;
		case ResultFields::DetailRowNames1D::H_equilibrium:
			if(col == 0) {
				return tr("[kJ/mol]");
			}
			if(col >= 1) {
				auto i = col - 1;
				return items->at(i).H_current;
			}
			break;
		case ResultFields::DetailRowNames1D::c_equilibrium:
			if(col == 0) {
				return tr("[G/RT]");
			}
			if(col >= 1) {
				auto i = col - 1;
				return items->at(i).result_of_optimization;
			}
			break;
		case ResultFields::DetailRowNames1D::Sum_value:
			if(col == 0) {
				return parameters.GetCompositionResultUnit();
			}
			if(col >= 1) {
				auto j = col - 1;
				const auto& sum = parameters.show_initial_in_result
						? items->at(j).sum_of_initial
						: items->at(j).sum_of_equilibrium;
				switch (parameters.composition_result_unit) {
				case ParametersNS::CompositionUnit::AtomicPercent:
					return sum.sum_atpct;
				case ParametersNS::CompositionUnit::WeightPercent:
					return sum.sum_wtpct;
				case ParametersNS::CompositionUnit::Mol:
					return sum.sum_mol;
				case ParametersNS::CompositionUnit::Gram:
					return sum.sum_gram;
				}
			}
			break;
		}
		if(row >= ResultFields::detail_row_names_1d_size) {
			auto i = row - ResultFields::detail_row_names_1d_size;
			auto first = items->cbegin();
			if(col == 0) {
				return parameters.GetCompositionResultUnit();
			}
			if(col >= 1) {
				auto j = col - 1;
				auto id = first->weights.at(i).id;
				const auto& val = parameters.show_initial_in_result
						? items->at(j).amounts.at(id)
						: items->at(j).amounts_of_equilibrium.at(id);
				switch (parameters.composition_result_unit) {
				case ParametersNS::CompositionUnit::AtomicPercent:
					return val.sum_atpct;
				case ParametersNS::CompositionUnit::WeightPercent:
					return val.sum_wtpct;
				case ParametersNS::CompositionUnit::Mol:
					return val.sum_mol;
				case ParametersNS::CompositionUnit::Gram:
					return val.sum_gram;
				}
			}
		}
	}
	return QVariant{};
}

QVariant ResultDetailModel::Data2D(const QModelIndex& index, int role) const
{
	auto col = index.column();
	auto row = index.row();
	if(role == Qt::BackgroundRole) {
		switch (static_cast<ResultFields::DetailRowNames2D>(row)) {
		case ResultFields::DetailRowNames2D::X_Axis_values_T_initial:
			return QBrush{ResultFields::red};
		case ResultFields::DetailRowNames2D::Y_Axis_values_Composition:
			return QBrush{ResultFields::red2};
		case ResultFields::DetailRowNames2D::T_result:
			return QBrush{ResultFields::green};
		case ResultFields::DetailRowNames2D::H_initial:
		case ResultFields::DetailRowNames2D::H_equilibrium:
		case ResultFields::DetailRowNames2D::c_equilibrium:
			break;
		case ResultFields::DetailRowNames2D::Sum_value:
			return QBrush{Qt::lightGray};
		default:
			break;
		}
		return QBrush{Qt::white};
	}
	if(role == Qt::DisplayRole) {
		switch (static_cast<ResultFields::DetailRowNames2D>(row)) {
		case ResultFields::DetailRowNames2D::X_Axis_values_T_initial:
			if(col == 0) {
				return parameters.GetTemperatureResultUnit();
			}
			if(col >= 1) {
				auto i = col - 1;
				return Thermodynamics::FromKelvin(items->at(i).temperature_K_initial,
												  parameters.temperature_result_unit);
			}
			break;
		case ResultFields::DetailRowNames2D::Y_Axis_values_Composition:
			if(col == 0) {
				return parameters.GetCompositionRangeUnit();
			}
			if(col >= 1) {
				auto i = col - 1;
				return items->at(i).composition_variable;
			}
			break;
		case ResultFields::DetailRowNames2D::T_result:
			if(col == 0) {
				return parameters.GetTemperatureResultUnit();
			}
			if(col >= 1) {
				auto i = col - 1;
				return Thermodynamics::FromKelvin(items->at(i).temperature_K_current,
												  parameters.temperature_result_unit);
			}
			break;
		case ResultFields::DetailRowNames2D::H_initial:
			if(col == 0) {
				return tr("[kJ/mol]");
			}
			if(col >= 1) {
				auto i = col - 1;
				return items->at(i).H_initial;
			}
			break;
		case ResultFields::DetailRowNames2D::H_equilibrium:
			if(col == 0) {
				return tr("[kJ/mol]");
			}
			if(col >= 1) {
				auto i = col - 1;
				return items->at(i).H_current;
			}
			break;
		case ResultFields::DetailRowNames2D::c_equilibrium:
			if(col == 0) {
				return tr("[G/RT]");
			}
			if(col >= 1) {
				auto i = col - 1;
				return items->at(i).result_of_optimization;
			}
			break;
		case ResultFields::DetailRowNames2D::Sum_value:
			if(col == 0) {
				return parameters.GetCompositionResultUnit();
			}
			if(col >= 1) {
				auto j = col - 1;
				const auto& sum = parameters.show_initial_in_result
						? items->at(j).sum_of_initial
						: items->at(j).sum_of_equilibrium;
				switch (parameters.composition_result_unit) {
				case ParametersNS::CompositionUnit::AtomicPercent:
					return sum.sum_atpct;
				case ParametersNS::CompositionUnit::WeightPercent:
					return sum.sum_wtpct;
				case ParametersNS::CompositionUnit::Mol:
					return sum.sum_mol;
				case ParametersNS::CompositionUnit::Gram:
					return sum.sum_gram;
				}
			}
			break;
		}
		if(row >= ResultFields::detail_row_names_2d_size) {
			auto i = row - ResultFields::detail_row_names_2d_size;
			auto first = items->cbegin();
			if(col == 0) {
				return parameters.GetCompositionResultUnit();
			}
			if(col >= 1) {
				auto j = col - 1;
				auto id = first->weights.at(i).id;
				const auto& val = parameters.show_initial_in_result
						? items->at(j).amounts.at(id)
						: items->at(j).amounts_of_equilibrium.at(id);
				switch (parameters.composition_result_unit) {
				case ParametersNS::CompositionUnit::AtomicPercent:
					return val.sum_atpct;
				case ParametersNS::CompositionUnit::WeightPercent:
					return val.sum_wtpct;
				case ParametersNS::CompositionUnit::Mol:
					return val.sum_mol;
				case ParametersNS::CompositionUnit::Gram:
					return val.sum_gram;
				}
			}
		}
	}
	return QVariant{};
}

QVariant ResultDetailModel::headerData(int section, Qt::Orientation orientation,
									   int role) const
{
	if(section >= row_count || items == nullptr) {
		return QVariant{};
	}
	if(role == Qt::DisplayRole && orientation == Qt::Vertical) {
		switch (parameters.workmode) {
		case ParametersNS::Workmode::SinglePoint:
			switch (static_cast<ResultFields::DetailRowNamesSingle>(section)) {
			case ResultFields::DetailRowNamesSingle::T_result:
				switch (parameters.target) {
				case ParametersNS::Target::Equilibrium:
					return ResultFields::detail_row_names_single.at(section).arg(tr("equilibrium"));
				case ParametersNS::Target::AdiabaticTemperature:
					return ResultFields::detail_row_names_single.at(section).arg(tr("adiabatic"));
				}
			case ResultFields::DetailRowNamesSingle::T_units:
			case ResultFields::DetailRowNamesSingle::T_initial:
			case ResultFields::DetailRowNamesSingle::H_initial:
			case ResultFields::DetailRowNamesSingle::H_equilibrium:
			case ResultFields::DetailRowNamesSingle::c_equilibrium:
			case ResultFields::DetailRowNamesSingle::Sum_units:
			case ResultFields::DetailRowNamesSingle::Sum_value:
				return ResultFields::detail_row_names_single.at(section);
			}
			if(section >= ResultFields::detail_row_names_single_size) {
				auto i = section - ResultFields::detail_row_names_single_size;
				return items->cbegin()->weights.at(i).formula;
			}
			break;
		case ParametersNS::Workmode::TemperatureRange:
		case ParametersNS::Workmode::CompositionRange:
			switch (static_cast<ResultFields::DetailRowNames1D>(section)) {
			case ResultFields::DetailRowNames1D::X_Axis_values:
				switch (parameters.workmode) {
				case ParametersNS::Workmode::TemperatureRange:
					return ResultFields::detail_row_names_1d.at(section).arg(tr("T initial"));
				case ParametersNS::Workmode::CompositionRange:
					return ResultFields::detail_row_names_1d.at(section).arg(tr("Composition"));
				default:
					break;
				}
				break;
			case ResultFields::DetailRowNames1D::T_result:
				switch (parameters.target) {
				case ParametersNS::Target::Equilibrium:
					return ResultFields::detail_row_names_1d.at(section).arg(tr("equilibrium"));
				case ParametersNS::Target::AdiabaticTemperature:
					return ResultFields::detail_row_names_1d.at(section).arg(tr("adiabatic"));
				}
				break;
			case ResultFields::DetailRowNames1D::T_initial:
			case ResultFields::DetailRowNames1D::H_initial:
			case ResultFields::DetailRowNames1D::H_equilibrium:
			case ResultFields::DetailRowNames1D::c_equilibrium:
			case ResultFields::DetailRowNames1D::Sum_value:
				return ResultFields::detail_row_names_1d.at(section);
			}
			if(section >= ResultFields::detail_row_names_1d_size) {
				auto i = section - ResultFields::detail_row_names_1d_size;
				return items->cbegin()->weights.at(i).formula;
			}
			break;
		case ParametersNS::Workmode::TemperatureCompositionRange:
			switch (static_cast<ResultFields::DetailRowNames2D>(section)) {
			case ResultFields::DetailRowNames2D::T_result:
				switch (parameters.target) {
				case ParametersNS::Target::Equilibrium:
					return ResultFields::detail_row_names_2d.at(section).arg(tr("equilibrium"));
				case ParametersNS::Target::AdiabaticTemperature:
					return ResultFields::detail_row_names_2d.at(section).arg(tr("adiabatic"));
				}
				break;
			case ResultFields::DetailRowNames2D::X_Axis_values_T_initial:
			case ResultFields::DetailRowNames2D::Y_Axis_values_Composition:
			case ResultFields::DetailRowNames2D::H_initial:
			case ResultFields::DetailRowNames2D::H_equilibrium:
			case ResultFields::DetailRowNames2D::c_equilibrium:
			case ResultFields::DetailRowNames2D::Sum_value:
				return ResultFields::detail_row_names_2d.at(section);
			}
			if(section >= ResultFields::detail_row_names_2d_size) {
				auto i = section - ResultFields::detail_row_names_2d_size;
				return items->cbegin()->weights.at(i).formula;
			}
			break;
		}
	}
	return QVariant{};
}
