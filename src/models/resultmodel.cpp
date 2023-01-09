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
	QT_TR_NOOP("Sum mol"),
	QT_TR_NOOP("Sum gram"),
	QT_TR_NOOP("Sum at.%"),
	QT_TR_NOOP("Sum wt.%")
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

const QColor green{109, 255, 109};
const QColor red{255, 109, 109};
} // namespace ResultFields

ResultModel::ResultModel(QObject *parent)
	: QAbstractTableModel{parent}
{

}

ResultModel::~ResultModel()
{

}

void ResultModel::SetNewData(const SubstanceWeights* vec, ParametersNS::Target tar)
{
	beginResetModel();
	items = vec;
	row_count = items->size() + ResultFields::row_names_size;
	target = tar;
	checked.clear();
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

	if(role == Qt::BackgroundRole) {
		switch(col) {
		case ResultFields::ColNames::Show: {
			auto find = checked.find(row);
			if(find != checked.cend()) {
				return find->second.color;
			}
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
					switch(target) {
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
			auto find = checked.find(row);
			if(find != checked.cend()) {
				return find->second.checked;
			} else {
				return Qt::CheckState::Unchecked;
			}
		}
		break;
	}

	return QVariant{};
}

bool ResultModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	LOG()
	if(!CheckIndexValidParent(index)) return false;

	// TODO

	return false;
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
	case ResultFields::ColNames::Name:
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

ResultDetailModel::ResultDetailModel(QObject* parent)
	: QAbstractTableModel{parent}
{

}

ResultDetailModel::~ResultDetailModel()
{

}

void ResultDetailModel::SetNewData(const Optimization::OptimizationVector* vec,
								   const ParametersNS::Parameters params,
								   const int x_size, const int y_size)
{
	beginResetModel();
	items = vec;
	parameters = params;
	row_count = items->cbegin()->number.substances;
	col_count = x_size;
	switch (parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		row_count += ResultFields::detail_row_names_single_size;
		col_count += ParametersNS::composition_units.size();
		break;
	case ParametersNS::Workmode::TemperatureRange:
	case ParametersNS::Workmode::CompositionRange:
		row_count += ResultFields::detail_row_names_1d_size;
		col_count += 2; // Names, Units
		break;
	case ParametersNS::Workmode::TemperatureCompositionRange:
		row_count = y_size + 1;
		col_count += 1;
		break;
		break;
	}
	endResetModel();
}

void ResultDetailModel::UpdateParameters(const ParametersNS::Parameters& params)
{
	beginResetModel();
	parameters.temperature_result_unit = params.temperature_result_unit;
	parameters.composition_result_unit = params.composition_result_unit;
	endResetModel();
}

void ResultDetailModel::Clear()
{
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
	switch (parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		return DataSingle(index, role);
	case ParametersNS::Workmode::TemperatureRange:
	case ParametersNS::Workmode::CompositionRange:
		return Data1D(index, role);
	case ParametersNS::Workmode::TemperatureCompositionRange:
		break;
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
			case 0: return ResultFields::detail_row_names_single.at(row);
			case 1: return tr("K");
			case 2: return tr("C");
			case 3: return tr("F");
			default: break;
			}
			break;
		case ResultFields::DetailRowNamesSingle::T_result:
			switch (col) {
			case 0:
				switch (parameters.target) {
				case ParametersNS::Target::Equilibrium:
					return ResultFields::detail_row_names_single.at(row).arg(tr("equilibrium"));
				case ParametersNS::Target::AdiabaticTemperature:
					return ResultFields::detail_row_names_single.at(row).arg(tr("adiabatic"));
				}
				break;
			case 1:
				return items->cbegin()->temperature_K_current;
			case 2:
				return Thermodynamics::FromKelvin(items->cbegin()->temperature_K_current,
												  ParametersNS::TemperatureUnit::Celsius);
			case 3:
				return Thermodynamics::FromKelvin(items->cbegin()->temperature_K_current,
												  ParametersNS::TemperatureUnit::Fahrenheit);
			}
			break;
		case ResultFields::DetailRowNamesSingle::T_initial:
			switch (col) {
			case 0:
				return ResultFields::detail_row_names_single.at(row);
			case 1:
				return items->cbegin()->temperature_K_initial;
			case 2:
				return Thermodynamics::FromKelvin(items->cbegin()->temperature_K_initial,
												  ParametersNS::TemperatureUnit::Celsius);
			case 3:
				return Thermodynamics::FromKelvin(items->cbegin()->temperature_K_initial,
												  ParametersNS::TemperatureUnit::Fahrenheit);
			}
			break;
		case ResultFields::DetailRowNamesSingle::H_initial:
			switch (col) {
			case 0: return ResultFields::detail_row_names_single.at(row);
			case 1: return items->cbegin()->H_initial;
			case 2: return tr("[kJ/mol]");
			}
			break;
		case ResultFields::DetailRowNamesSingle::H_equilibrium:
			switch (col) {
			case 0: return ResultFields::detail_row_names_single.at(row);
			case 1: return items->cbegin()->H_current;
			case 2: return tr("[kJ/mol]");
			}
			break;
		case ResultFields::DetailRowNamesSingle::c_equilibrium:
			switch (col) {
			case 0: return ResultFields::detail_row_names_single.at(row);
			case 1: return items->cbegin()->result_of_optimization;
			case 2: return tr("[G/RT]");
			}
			break;
		case ResultFields::DetailRowNamesSingle::Sum_units:
			switch (col) {
			case 0: return ResultFields::detail_row_names_single.at(row);
			case 1: return tr("mol");
			case 2: return tr("gram");
			case 3: return tr("at.%");
			case 4: return tr("wt.%");
			default: break;
			}
			break;
		case ResultFields::DetailRowNamesSingle::Sum_value:
			switch (col) {
			case 0: return ResultFields::detail_row_names_single.at(row);
			case 1: return items->cbegin()->sum_of_equilibrium.sum_mol;
			case 2: return items->cbegin()->sum_of_equilibrium.sum_gram;
			case 3: return items->cbegin()->sum_of_equilibrium.sum_atpct;
			case 4: return items->cbegin()->sum_of_equilibrium.sum_wtpct;
			}
			break;
		}
		if(row >= ResultFields::detail_row_names_single_size) {
			auto i = row - ResultFields::detail_row_names_single_size;
			auto first = items->cbegin();
			switch (col) {
			case 0:
				return first->weights.at(i).formula;
			case 1:
				return first->amounts_of_equilibrium.at(
							first->weights.at(i).id).sum_mol;
			case 2:
				return first->amounts_of_equilibrium.at(
							first->weights.at(i).id).sum_gram;
			case 3:
				return first->amounts_of_equilibrium.at(
							first->weights.at(i).id).sum_atpct;
			case 4:
				return first->amounts_of_equilibrium.at(
							first->weights.at(i).id).sum_wtpct;
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
			switch (col) {
			case 0:
				switch (parameters.workmode) {
				case ParametersNS::Workmode::TemperatureRange:
					return ResultFields::detail_row_names_1d.at(row).arg(tr("T initial"));
				case ParametersNS::Workmode::CompositionRange:
					return ResultFields::detail_row_names_1d.at(row).arg(tr("Composition"));
				default:
					break;
				}
				break;
			case 1:
				switch (parameters.workmode) {
				case ParametersNS::Workmode::TemperatureRange:
					return ParametersNS::temperature_units.at(
								static_cast<int>(parameters.temperature_result_unit));
				case ParametersNS::Workmode::CompositionRange:
					return ParametersNS::composition_units.at(
								static_cast<int>(parameters.composition_range_unit));
				default:
					break;
				}
				break;
			}
			if(col >= 2) {
				auto i = col - 2;
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
			switch (col) {
			case 0:
				switch (parameters.target) {
				case ParametersNS::Target::Equilibrium:
					return ResultFields::detail_row_names_1d.at(row).arg(tr("equilibrium"));
				case ParametersNS::Target::AdiabaticTemperature:
					return ResultFields::detail_row_names_1d.at(row).arg(tr("adiabatic"));
				}
				break;
			case 1:
				return ParametersNS::temperature_units.at(
							static_cast<int>(parameters.temperature_result_unit));
			}
			if(col >= 2) {
				auto i = col - 2;
				return Thermodynamics::FromKelvin(items->at(i).temperature_K_current,
												  parameters.temperature_result_unit);
			}
			break;
		case ResultFields::DetailRowNames1D::T_initial:
			switch (col) {
			case 0:
				return ResultFields::detail_row_names_1d.at(row);
			case 1:
				return ParametersNS::temperature_units.at(
							static_cast<int>(parameters.temperature_result_unit));
			}
			if(col >= 2) {
				auto i = col - 2;
				return Thermodynamics::FromKelvin(items->at(i).temperature_K_initial,
												  parameters.temperature_result_unit);
			}
			break;
		case ResultFields::DetailRowNames1D::H_initial:
			switch (col) {
			case 0: return ResultFields::detail_row_names_1d.at(row);
			case 1: return tr("[kJ/mol]");
			}
			if(col >= 2) {
				auto i = col - 2;
				return items->at(i).H_initial;
			}
			break;
		case ResultFields::DetailRowNames1D::H_equilibrium:
			switch (col) {
			case 0: return ResultFields::detail_row_names_1d.at(row);
			case 1: return tr("[kJ/mol]");
			}
			if(col >= 2) {
				auto i = col - 2;
				return items->at(i).H_current;
			}
			break;
		case ResultFields::DetailRowNames1D::c_equilibrium:
			switch (col) {
			case 0: return ResultFields::detail_row_names_1d.at(row);
			case 1: return tr("[G/RT]");
			}
			if(col >= 2) {
				auto i = col - 2;
				return items->at(i).result_of_optimization;
			}
			break;
		case ResultFields::DetailRowNames1D::Sum_value:
			switch (col) {
			case 0: return ResultFields::detail_row_names_1d.at(row);
			case 1:
				return ParametersNS::composition_units.at(
							static_cast<int>(parameters.composition_result_unit));
			}
			if(col >= 2) {
				auto j = col - 2;
				switch (parameters.composition_result_unit) {
				case ParametersNS::CompositionUnit::AtomicPercent:
					return items->at(j).sum_of_equilibrium.sum_atpct;
				case ParametersNS::CompositionUnit::WeightPercent:
					return items->at(j).sum_of_equilibrium.sum_wtpct;
				case ParametersNS::CompositionUnit::Mol:
					return items->at(j).sum_of_equilibrium.sum_mol;
				case ParametersNS::CompositionUnit::Gram:
					return items->at(j).sum_of_equilibrium.sum_gram;
				}
			}
			break;
		}
		if(row >= ResultFields::detail_row_names_1d_size) {
			auto i = row - ResultFields::detail_row_names_1d_size;
			auto first = items->cbegin();
			switch (col) {
			case 0:
				return first->weights.at(i).formula;
			case 1:
				return ParametersNS::composition_units.at(
							static_cast<int>(parameters.composition_result_unit));
			}
			if(col >= 2) {
				auto j = col - 2;
				const auto& val = items->at(j).amounts_of_equilibrium.at(first->weights.at(i).id);
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
