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

#include "coreapplication.h"

#include <QThread>
#include <QPair>
#include <QAbstractItemView>
#include <QStringListModel>
#include <QProgressDialog>
#include "utilities.h"
#include "thermodynamics.h"

CoreApplication::CoreApplication(MainWindow *const gui, QObject *parent)
	: QObject{parent}
{
	LOG()

	qRegisterMetaType<QVector<int>>("QVector<int>");
	qRegisterMetaType<ParametersNS::Parameters>("ParametersNS::Parameters");
	qRegisterMetaType<ParametersNS::TemperatureUnit>("ParametersNS::TemperatureUnit");
	qRegisterMetaType<GraphId>("GraphId");
	qRegisterMetaType<QVector<GraphId>>("QVector<GraphId>&");
	qRegisterMetaType<QVector<double>>("QVector<double>&");
	qRegisterMetaType<QVector<QVector<double>>>("QVector<QVector<double>>&");
	qRegisterMetaType<Optimization::OptimizationVector>("Optimization::OptimizationVector&");

	// GUI methods should be called only in this constructor,
	// but not in any other CoreApplication methods,
	// because after construct this object will be moved to another thread.
	// All communication with the GUI must be done through signals.

	// create models
	model_substances = new SubstancesTableModel(this);
	model_substances_temp_range = new SubstancesTempRangeModel(this);
	model_substances_tabulated_tf = new SubstancesTabulatedTFModel(this);
	model_plot_tf = new PlotTFModel(this);
	model_amounts = new AmountsModel(this);
	model_result = new ResultModel(this);
	model_detail_result = new ResultDetailModel(this);

	// set models
	gui->SetSubstancesTableModel(model_substances);
	gui->SetSubstancesTempRangeModel(model_substances_temp_range);
	gui->SetSubstancesTabulatedModel(model_substances_tabulated_tf);
	gui->SetTFPlotModel(model_plot_tf);
	gui->SetAmountsModel(model_amounts);
	gui->SetResultModel(model_result);
	gui->SetResultDetailModel(model_detail_result);
	gui->Initialize(); // must be called after set models

	// connects
	connect(gui, &MainWindow::SignalUpdate,
			this, &CoreApplication::SlotUpdate);
	connect(gui, &MainWindow::SignalUpdateButtonClicked,
			this, &CoreApplication::SlotUpdateButtonHandler);
	connect(this, &CoreApplication::SignalSetAvailableElements,
			gui, &MainWindow::SlotSetAvailableElements);
	connect(gui, &MainWindow::SignalSubstancesTableSelection,
			this, &CoreApplication::SlotSubstancesTableSelectionHandler);
	connect(this, &CoreApplication::SignalSetSelectedSubstanceLabel,
			gui, &MainWindow::SlotSetSelectedSubstanceLabel);
	connect(gui, &MainWindow::SignalStartCalculate,
			this, &CoreApplication::SlotStartCalculations);
	connect(gui, &MainWindow::SignalSendResult,
			this, &CoreApplication::SlotResieveResult);

	// model plot TF
	connect(model_plot_tf, &PlotTFModel::AddGraph,
			this, &CoreApplication::SlotAddGraphPlotTF);
	connect(model_plot_tf, &PlotTFModel::RemoveGraph,
			this, &CoreApplication::SlotRemoveGraphPlotTF);
	connect(model_plot_tf, &PlotTFModel::ChangeColorGraph,
			this, &CoreApplication::SlotChangeColorGraphPlotTF);
	connect(this, &CoreApplication::SignalAddGraphPlotTF,
			gui, &MainWindow::SlotAddGraphPlotTF);
	connect(this, &CoreApplication::SignalRemoveGraphPlotTF,
			gui, &MainWindow::SlotRemoveGraphPlotTF);
	connect(this, &CoreApplication::SignalChangeColorGraphPlotTF,
			gui, &MainWindow::SlotChangeColorGraphPlotTF);

	connect(gui, &MainWindow::SignalAllGraphsRemovedPlotTF,
			this, &CoreApplication::SlotAllGraphsRemovedPlotTFVtM);
	connect(gui, &MainWindow::SignalGraphColorChangedPlotTF,
			this, &CoreApplication::SlotGraphColorChangedPlotTFVtM);
	connect(gui, &MainWindow::SignalGraphRemovedPlotTF,
			this, &CoreApplication::SlotGraphRemovedPlotTFVtM);
	connect(gui, &MainWindow::SignalGraphsRemovedPlotTF,
			this, &CoreApplication::SlotGraphsRemovedPlotTFVtM);

	connect(this, &CoreApplication::SignalSetPlotXAxisUnit,
			gui, &MainWindow::SlotSetPlotXAxisUnit);

	// result
	connect(model_result, &ResultModel::AddGraph,
			this, &CoreApplication::SlotAddGraphPlotResult);
	connect(model_result, &ResultModel::RemoveGraph,
			this, &CoreApplication::SlotRemoveGraphPlotResult);
	connect(model_result, &ResultModel::RemoveGraphs,
			this, &CoreApplication::SlotGraphsRemovedPlotResultVtM);
	connect(model_result, &ResultModel::ChangeColorGraph,
			this, &CoreApplication::SlotChangeColorGraphPlotResult);
	connect(this, &CoreApplication::SignalAddGraphPlotResult,
			gui, &MainWindow::SlotAddGraphPlotResult);
	connect(this, &CoreApplication::SignalAddHeatmapPlotResult,
			gui, &MainWindow::SlotAddHeatmapPlotResult);
	connect(this, &CoreApplication::SignalAdd3DGraphPlotResult,
			gui, &MainWindow::SlotAdd3DGraphPlotResult);

	connect(this, &CoreApplication::SignalRemoveGraphPlotResult,
			gui, &MainWindow::SlotRemoveGraphPlotResult);
	connect(this, &CoreApplication::SignalChangeColorGraphPlotResult,
			gui, &MainWindow::SlotChangeColorGraphPlotResult);

	connect(gui, &MainWindow::SignalAllGraphsRemovedPlotResult,
			this, &CoreApplication::SlotAllGraphsRemovedPlotResultVtM);
	connect(gui, &MainWindow::SignalGraphColorChangedPlotResult,
			this, &CoreApplication::SlotGraphColorChangedPlotResultVtM);
	connect(gui, &MainWindow::SignalGraphRemovedPlotResult,
			this, &CoreApplication::SlotGraphRemovedPlotResultVtM);
	connect(gui, &MainWindow::SignalGraphsRemovedPlotResult,
			this, &CoreApplication::SlotGraphsRemovedPlotResultVtM);

	connect(this, &CoreApplication::SignalSetPlotResultAxisUnit,
			gui, &MainWindow::SlotSetPlotResultAxisUnit);

	// amounts
	connect(gui, &MainWindow::SignalAmountsTableDelete,
			model_amounts, &AmountsModel::Delete);

	// calculations
	connect(this, &CoreApplication::SignalStartCalculations,
			gui, &MainWindow::SlotStartCalculations);

}

CoreApplication::~CoreApplication()
{
	LOG()
}

void CoreApplication::Initialize()
{
	LOG()
	// must invoke after this was moved to another (non GUI) thread
	// Databases
	databases.reserve(ParametersNS::database_filenames.size());
	databases.push_back(std::make_shared<DatabaseThermo>(ParametersNS::database_filenames.at(0)));
	databases.push_back(std::make_shared<DatabaseHSC>(ParametersNS::database_filenames.at(1)));
	// initial parameters
	auto db = databases.at(static_cast<int>(parameters_.database));
	emit SignalSetAvailableElements(db->GetAvailableElements());
}

auto CoreApplication::CurrentDatabase()
{
	return databases.at(static_cast<int>(parameters_.database));
}

auto CoreApplication::Database(ParametersNS::Database database)
{
	return databases.at(static_cast<int>(database));
}

void CoreApplication::SlotUpdate(const ParametersNS::Parameters parameters)
{
	LOG()
	if(parameters.database != parameters_.database) {
		is_selected = false;
		model_plot_tf->Clear();
		model_amounts->Clear();
	}
	parameters_ = std::move(parameters);
	auto db = CurrentDatabase();
	auto data = db->GetSubstancesData(parameters_);

	SubstanceNames names(data.size());
	SubstanceWeights weights(data.size());
#if 1 // structure slicing
	std::copy(data.cbegin(), data.cend(), weights.begin());
	std::copy(data.cbegin(), data.cend(), names.begin());
#else // no slicing
	std::transform(data.cbegin(), data.cend(), names.begin(),
		[](const SubstanceData& i){ return SubstanceFormula{i.id, i.formula}; });
	std::transform(data.cbegin(), data.cend(), weights.begin(),
				   [](const SubstanceData& i){
		return SubstanceWeight{i.id, i.formula, i.weight}; });
#endif
	model_substances->SetNewData(std::move(data));
	model_plot_tf->SetDatabase(parameters_.database);
	model_plot_tf->SetNewData(std::move(names));
	model_amounts->SetNewData(std::move(weights));
	model_result->UpdateParameters(parameters_);
	model_detail_result->UpdateParameters(parameters_);
	UpdateRangeTabulatedModels();
	emit SignalSetAvailableElements(db->GetAvailableElements());
	emit SignalSetPlotXAxisUnit(parameters_.temperature_range_unit);
	emit SignalSetPlotResultAxisUnit(parameters_);
}

void CoreApplication::SlotUpdateButtonHandler(
		const ParametersNS::Parameters parameters)
{
	LOG()
	SlotUpdate(parameters);

	// plots TF update
	for(auto&& [id, params] : graphs_tf_view) {
		SlotAddGraphPlotTF(id, params.name, params.color);
	}

	// plots Result updata
	for(auto&& [id, params] : graphs_result_view) {
		SlotAddGraphPlotResult(id, params.name, params.color);
	}
}

void CoreApplication::SlotSubstancesTableSelectionHandler(int id)
{
	selected_substance_id = id;
	is_selected = true;
	UpdateRangeTabulatedModels();
}

void CoreApplication::SlotAddGraphPlotTF(const GraphId id, const QString& name,
										 const QColor& color)
{
	graphs_tf_view[id].color = color;
	graphs_tf_view[id].name = name;
	QVector<double> x, y;
	auto db = Database(static_cast<ParametersNS::Database>(id.database));
	auto data_temp_range = db->GetSubstanceTempRangeData(id.substance_id);
	Thermodynamics::TabulateOneTF(parameters_.temperature_range,
								  parameters_.temperature_range_unit,
								  parameters_.extrapolation,
								  static_cast<ParametersNS::Database>(id.database),
								  static_cast<ParametersNS::ThermodynamicFunction>(id.option),
								  data_temp_range, x, y);
	emit SignalAddGraphPlotTF(id, name, color, x, y);
}

void CoreApplication::SlotRemoveGraphPlotTF(const GraphId id)
{
	graphs_tf_view.erase(id);
	emit SignalRemoveGraphPlotTF(id);
}

void CoreApplication::SlotChangeColorGraphPlotTF(const GraphId id,
												 const QColor& color)
{
	LOG(color)
	auto it = graphs_tf_view.find(id);
	if(it != graphs_tf_view.end()) {
		it->second.color = color;
		emit SignalChangeColorGraphPlotTF(id, color);
	}
}

void CoreApplication::SlotAllGraphsRemovedPlotTFVtM()
{
	graphs_tf_view.clear();
	model_plot_tf->SlotRemoveAllGraphs();
}

void CoreApplication::SlotGraphColorChangedPlotTFVtM(const GraphId id,
													 const QColor& color)
{
	auto it = graphs_tf_view.find(id);
	if(it != graphs_tf_view.end()) {
		it->second.color = color;
		model_plot_tf->SlotChangeColorGraph(id, color);
	}
}

void CoreApplication::SlotGraphRemovedPlotTFVtM(const GraphId id)
{
	graphs_tf_view.erase(id);
	model_plot_tf->SlotRemoveOneGraph(id);
}

void CoreApplication::SlotGraphsRemovedPlotTFVtM(const QVector<GraphId>& ids)
{
	for(auto&& id : ids) {
		graphs_tf_view.erase(id);
	}
	model_plot_tf->SlotRemoveGraphs(ids);
}

void CoreApplication::SlotAddGraphPlotResult(const GraphId id, const QString& name,
											 const QColor& color)
{
	LOG()
	QString new_name = name.arg(GetUnits(id));
	switch (parameters_.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		break;
	case ParametersNS::Workmode::TemperatureRange: {
		assert(x_size == result_data.size());
		graphs_result_view[id] = {color, name};
		QVector<double> x(x_size);
		std::transform(result_data.cbegin(), result_data.cend(), x.begin(),
					   [u = parameters_.temperature_result_unit](Optimization::OptimizationVector::const_reference i){
			return Thermodynamics::FromKelvin(i.temperature_K_initial, u);});
		auto y{MakeYVector(id)};
		emit SignalAddGraphPlotResult(id, new_name, color, x, y);
	}
		break;
	case ParametersNS::Workmode::CompositionRange: {
		assert(x_size == result_data.size());
		graphs_result_view[id] = {color, name};
		QVector<double> x(x_size);
		std::transform(result_data.cbegin(), result_data.cend(), x.begin(),
					   [](Optimization::OptimizationVector::const_reference i){
			return i.composition_variable;});
		auto y{MakeYVector(id)};
		emit SignalAddGraphPlotResult(id, new_name, color, x, y);
	}
		break;
	case ParametersNS::Workmode::TemperatureCompositionRange: {
		// 3d plot and heatmap
		assert(x_size * y_size == result_data.size());
		graphs_result_view[id] = {color, name};
		QVector<double> composition, temperature;
		QVector<QVector<double>> values;
		QSurfaceDataArray* data = new QSurfaceDataArray;
		MakeHeatmapAnd3DVectors(id, composition, temperature, values, data);
		emit SignalAddHeatmapPlotResult(id, new_name, color, composition, temperature, values);
		emit SignalAdd3DGraphPlotResult(id, new_name, color, data);
	}
		break;
	}
}

QString CoreApplication::GetUnits(const GraphId id) const
{
	if(id.substance_id > 0) {
		assert(id.option == -1);
		return parameters_.GetCompositionResultUnit();
	} else {
		assert(id.option >= 0);
		switch (static_cast<ResultFields::RowNames>(id.option)) {
		case ResultFields::RowNames::T_result:
		case ResultFields::RowNames::T_initial:
			return parameters_.GetTemperatureResultUnit();
		case ResultFields::RowNames::H_initial:
		case ResultFields::RowNames::H_equilibrium:
			return tr("kJ/mol");
		case ResultFields::RowNames::c_equilibrium:
			return tr("G/RT");
		case ResultFields::RowNames::Sum:
			return parameters_.GetCompositionResultUnit();
		}
	}
	return QString{};
}

QVector<double> CoreApplication::MakeYVector(const GraphId id) const
{
	QVector<double> y(result_data.size());
	for(int i = 0, max = result_data.size(); i != max; ++i) {
		y[i] = ChooseValueInResultData(id, i);
	}
	return y;
}

double CoreApplication::ChooseValueInResultData(const GraphId id, const int index) const
{
	if(id.substance_id > 0) {
		assert(id.option == -1);
		const auto& val = parameters_.show_initial_in_result
				? result_data.at(index).amounts.at(id.substance_id)
				: result_data.at(index).amounts_of_equilibrium.at(id.substance_id);
		switch (parameters_.composition_result_unit) {
		case ParametersNS::CompositionUnit::AtomicPercent:
			return val.sum_atpct;
		case ParametersNS::CompositionUnit::WeightPercent:
			return val.sum_wtpct;
		case ParametersNS::CompositionUnit::Mol:
			return val.sum_mol;
		case ParametersNS::CompositionUnit::Gram:
			return val.sum_gram;
		}
	} else {
		assert(id.option >= 0);
		switch (static_cast<ResultFields::RowNames>(id.option)) {
		case ResultFields::RowNames::T_result:
			return Thermodynamics::FromKelvin(result_data.at(index).temperature_K_current,
											  parameters_.temperature_result_unit);
			break;
		case ResultFields::RowNames::T_initial:
			return Thermodynamics::FromKelvin(result_data.at(index).temperature_K_initial,
											  parameters_.temperature_result_unit);
			break;
		case ResultFields::RowNames::H_initial:
			return result_data.at(index).H_initial;
			break;
		case ResultFields::RowNames::H_equilibrium:
			return result_data.at(index).H_current;
			break;
		case ResultFields::RowNames::c_equilibrium:
			return result_data.at(index).result_of_optimization;
			break;
		case ResultFields::RowNames::Sum:
			const auto& sum = parameters_.show_initial_in_result
					? result_data.at(index).sum_of_initial
					: result_data.at(index).sum_of_equilibrium;
			switch (parameters_.composition_result_unit) {
			case ParametersNS::CompositionUnit::AtomicPercent:
				return sum.sum_atpct;
			case ParametersNS::CompositionUnit::WeightPercent:
				return sum.sum_wtpct;
			case ParametersNS::CompositionUnit::Mol:
				return sum.sum_mol;
			case ParametersNS::CompositionUnit::Gram:
				return sum.sum_gram;
			}
			break;
		}
	}
#ifndef NDEBUG
	throw std::out_of_range("ChooseValueInResultData has wrong id");
#else
	return 0;
#endif
}

void CoreApplication::MakeHeatmapAnd3DVectors(const GraphId id,
	QVector<double>& composition, QVector<double>& temperatures,
	QVector<QVector<double> >& values, QSurfaceDataArray* data) const
{
	const int t_size = x_size;
	const int c_size = y_size;
	const int full_size = result_data.size();
	assert(full_size == t_size * c_size);
	composition.resize(c_size);
	temperatures.resize(t_size);
	data->reserve(t_size);
	values.resize(t_size);
	for(auto&& i : values) {
		i.resize(c_size);
	}
	for(int i = 0; i != c_size; ++i) {
		composition[i] = result_data.at(i).composition_variable;
	}
	int i = 0;
	for(int ti = 0; ti != t_size; ++ti) {
		temperatures[ti] = Thermodynamics::FromKelvin(result_data.at(i).temperature_K_initial,
													  parameters_.temperature_result_unit);
		auto row = new QSurfaceDataRow(c_size);
		for(int ci = 0; ci != c_size; ++ci, ++i) {
			auto v = ChooseValueInResultData(id, i);
			values[ti][ci] = v;
			(*row)[ci].setPosition(QVector3D{static_cast<float>(composition.at(ci)),
											 static_cast<float>(v),
											 static_cast<float>(temperatures.at(ti))});
		}
		data->append(row);
	}
}

void CoreApplication::SlotRemoveGraphPlotResult(const GraphId id)
{
	LOG()
	graphs_result_view.erase(id);
	emit SignalRemoveGraphPlotResult(id);
}

void CoreApplication::SlotChangeColorGraphPlotResult(const GraphId id, const QColor& color)
{
	LOG()
	auto it = graphs_result_view.find(id);
	if(it != graphs_result_view.end()) {
		it->second.color = color;
		emit SignalChangeColorGraphPlotResult(id, color);
	}
}

void CoreApplication::SlotAllGraphsRemovedPlotResultVtM()
{
	LOG()
	graphs_result_view.clear();
	model_result->SlotRemoveAllGraphs();
}

void CoreApplication::SlotGraphColorChangedPlotResultVtM(const GraphId id,
														 const QColor& color)
{
	LOG()
	auto it = graphs_result_view.find(id);
	if(it != graphs_result_view.end()) {
		it->second.color = color;
		model_result->SlotChangeColorGraph(id, color);
	}
}

void CoreApplication::SlotGraphRemovedPlotResultVtM(const GraphId id)
{
	LOG()
	graphs_result_view.erase(id);
	model_result->SlotRemoveOneGraph(id);
}

void CoreApplication::SlotGraphsRemovedPlotResultVtM(const QVector<GraphId>& ids)
{
	LOG()
	for(auto&& id : ids) {
		graphs_result_view.erase(id);
	}
	model_result->SlotRemoveGraphs(ids);
}

void CoreApplication::UpdateRangeTabulatedModels()
{
	if(is_selected) {
		auto db = CurrentDatabase();
		auto data_temp_range = db->GetSubstanceTempRangeData(
					selected_substance_id);
		auto tabdata = Thermodynamics::Tabulate(
					parameters_.temperature_range,
					parameters_.temperature_range_unit,
					parameters_.extrapolation,
					parameters_.database,
					data_temp_range);
		model_substances_tabulated_tf->SetNewData(std::move(tabdata));
		model_substances_temp_range->SetNewData(std::move(data_temp_range));
		auto name = db->GetSubstanceName(selected_substance_id);
		emit SignalSetSelectedSubstanceLabel(name);
	} else {
		model_substances_temp_range->Clear();
		model_substances_tabulated_tf->Clear();
		emit SignalSetSelectedSubstanceLabel(tr("none"));
	}
}

/****************************************************************************
 *							Calculations
 ****************************************************************************/

namespace {
template<typename ForwardIt, typename UnaryOperation, typename = std::void_t<
			 std::enable_if_t<std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<ForwardIt>::iterator_category>>,
			 std::enable_if_t<std::is_invocable_v<UnaryOperation, typename std::iterator_traits<ForwardIt>::value_type>>,
			 std::enable_if_t<std::is_arithmetic_v<std::remove_reference_t<std::invoke_result_t<UnaryOperation, typename std::iterator_traits<ForwardIt>::value_type>>>>
			 >>
QString MakeCommaSeparatedString(ForwardIt first, ForwardIt last,
								 UnaryOperation&& unary_op)
{
	using val = typename std::iterator_traits<ForwardIt>::value_type;
	static_assert(std::is_invocable_v<UnaryOperation, val>);
	static_assert(std::is_arithmetic_v<std::remove_reference_t<std::invoke_result_t<UnaryOperation, val>>>);
	QStringList strlist;
	std::transform(first, last, std::back_inserter(strlist),
				   [&unary_op](const auto& i){
		return QString::number(std::invoke(std::forward<UnaryOperation>(unary_op),
										   std::forward<decltype(i)>(i)));});
	auto str = QStringLiteral("'") + strlist.join("','") + QStringLiteral("'");
	return str;
}
} // namespace

void CoreApplication::SlotStartCalculations()
{
	LOG(">> START CALCULATION <<")

	auto composition_data = model_amounts->GetCompositionData();
	if(std::all_of(composition_data.amounts.cbegin(),
				   composition_data.amounts.cend(),
				   [](auto&& pair){return pair.second.isZero();}))
	{
		LOG(">> Amounts are zero <<") // https://godbolt.org/z/bj4YW3T3E
		return;
	}
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);

	auto db = CurrentDatabase();

	// 1. Make species list
	auto ids_str = MakeCommaSeparatedString(composition_data.weights.cbegin(),
											composition_data.weights.cend(),
											&SubstanceWeight::id);
	LOG(ids_str)

	// 2. Make elements list for the number of elements
	auto elements = db->GetAvailableElements(ids_str);

	// 3. Get species temp range data from current database
	auto temp_ranges = db->GetSubstancesTempRangeData(ids_str);

	// 4. Get elements composition for species
	auto subs_element_composition = db->GetSubstancesElementComposition(ids_str);

	// 5. Prepare vector of calculation instances
	Optimization::OptimizationItemsMaker oim(parameters_, elements, temp_ranges,
		subs_element_composition, composition_data.weights, composition_data.amounts);
	auto vec{std::move(oim.GetData())};
	x_size = oim.GetXSize();
	y_size = oim.GetYSize();

	// 6. emit vector
	emit SignalStartCalculations(vec, parameters_.threads);

	LOG(">> END CALCULATION <<")
}

void CoreApplication::SlotResieveResult(Optimization::OptimizationVector& vec)
{
	LOG("vec.size:", vec.size())
	if(vec.empty()) return;
	model_result->Clear();
	model_detail_result->Clear();

	result_data = std::move(vec);

	model_result->SetNewData(&(result_data.cbegin()->weights),
							 parameters_);
	model_detail_result->SetNewData(&result_data, parameters_, x_size, y_size);

}
