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
	qRegisterMetaType<QVector<HeavyContainer>>("QVector<HeavyContainer>&");
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
	// demo
	table_1 = new QStringListModel(this);


	// set models
	gui->SetSubstancesTableModel(model_substances);
	gui->SetSubstancesTempRangeModel(model_substances_temp_range);
	gui->SetSubstancesTabulatedModel(model_substances_tabulated_tf);
	gui->SetTFPlotModel(model_plot_tf);
	gui->SetAmountsModel(model_amounts);
	gui->SetResultModel(model_result);
	gui->SetResultDetailModel(model_detail_result);
	gui->Initialize(); // must be called after set models

	// demo
	gui->SetModel_1(table_1);
	gui->SetModel_2(table_1);
	auto sel = new QItemSelectionModel(table_1);
	gui->SetSelectonModel(sel);

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

	//demo
	connect(gui, &MainWindow::SignalSendRequest,
			this, &CoreApplication::SlotRequestHandler);
	connect(gui, &MainWindow::SignalPushButtonClicked,
			this, &CoreApplication::SlotPushButtonHandler);
	connect(gui, &MainWindow::SignalHeavyCalculationsStart,
			this, &CoreApplication::SlotHeavyCalculations);
	connect(this, &CoreApplication::SignalShowError,
			gui, &MainWindow::SlotShowError);
	connect(this, &CoreApplication::SignalShowResponse,
			gui, &MainWindow::SlotShowResponse);
	connect(this, &CoreApplication::SignalShowTime,
			gui, &MainWindow::SlotShowStatusBarText);

	// plots
	connect(gui, &MainWindow::SignalNeed2DGraphData,
			this, &CoreApplication::SlotMake2DGraphData);
	connect(gui, &MainWindow::SignalNeedHeatMapData,
			this, &CoreApplication::SlotMakeHeatMapData);
	connect(gui, &MainWindow::SignalNeed3DGraphData,
			this, &CoreApplication::SlotMake3DGraphData);

	connect(this, &CoreApplication::SignalShow2DGraphData,
			gui, &MainWindow::SlotAdd2DGraph);
	connect(this, &CoreApplication::SignalShowHeatMapData,
			gui, &MainWindow::SlotAddHeatMap);
	connect(this, &CoreApplication::SignalShow3DGraphData,
			gui, &MainWindow::SlotAdd3DGraph);

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
	// demo
	connect(this, &CoreApplication::SignalStartHeavyComputations,
			gui, &MainWindow::SlotHeavyComputations);
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

void CoreApplication::SlotRequestHandler(int i)
{
	LOG()
	QThread::sleep(1);
	QString text{tr("Result: %1 Thread: %2").arg(i).
				arg(QThread::currentThread()->objectName())};
	emit SignalShowResponse(text);
}

void CoreApplication::SlotPushButtonHandler(const QString& text)
{
	LOG()
	QThread::sleep(1);
	auto&& list = static_cast<QStringListModel*>(table_1)->stringList();
	list.append(text);
	static_cast<QStringListModel*>(table_1)->setStringList(list);
}

void CoreApplication::SlotHeavyCalculations()
{
	// Demo
	LOG(">> CORE HEAVY START <<")
	auto vec = PrepareHeavyCalculations();
	emit SignalStartHeavyComputations(vec);
	LOG(">> CORE HEAVY END <<")
}

void CoreApplication::SlotMake2DGraphData()
{
	LOG()
	static GraphId id{};
	static RandomDouble rd{0.8, 1.2};
	const int size = 1000;
	QVector<double> x(size), y(size);
	std::iota(x.begin(), x.end(), 0);
	std::transform(x.cbegin(), x.cend(), y.begin(), [](double i){
		return (std::sin((i+rd())/50)+1)/rd();
	});
	id.substance_id++;
	emit SignalShow2DGraphData(id, x, y);
}

void CoreApplication::SlotMakeHeatMapData()
{
	LOG()
	static int id = 0;
	const int x_size = 301;
	const int y_size = 201;
	QVector<double> x(x_size), y(y_size);
	QVector<QVector<double>> z(y_size);
	//z.resize(y_size);
	for(auto&& i : z) {
		i.resize(x_size);
	}
	std::iota(x.begin(), x.end(), 0.0);
	std::iota(y.begin(), y.end(), 0.0);

	for(int yi = 0; yi != y_size; ++yi) {
		for(int xi = 0; xi != x_size; ++xi) {
			z[yi][xi] = std::sin(x.at(xi)*M_PI/x_size) +
					std::sin(y.at(yi)*M_PI/y_size);
		}
	}
	emit SignalShowHeatMapData(QString::number(id++), x, y, z);
}

void CoreApplication::SlotMake3DGraphData()
{
	LOG()
	int z_rows = 101;
	int x_cols = 201;
	auto array = new QSurfaceDataArray;
	array->reserve(z_rows);
	for(int zi = 0; zi != z_rows; ++zi) {
		auto row = new QSurfaceDataRow(x_cols);
		for(int xi = 0; xi != x_cols; ++xi) {
			auto value = std::sin(xi*M_PI/x_cols) + std::sin(zi*M_PI/z_rows);
			(*row)[xi].setPosition(QVector3D{static_cast<float>(xi),
											 static_cast<float>(value),
											 static_cast<float>(zi)});
		}
		array->append(row);
	}
	emit SignalShow3DGraphData(array);
}

QVector<HeavyContainer> CoreApplication::PrepareHeavyCalculations()
{
	LOG()
	QVector<HeavyContainer> vec(300);
	return vec;
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
	switch (parameters_.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		break;
	case ParametersNS::Workmode::TemperatureRange: {
		graphs_result_view[id].color = color;
		graphs_result_view[id].name = name;
		QVector<double> x(result_data.size());
		std::transform(result_data.cbegin(), result_data.cend(), x.begin(),
					   [](Optimization::OptimizationVector::const_reference i){
			return i.temperature_K_initial;});
		auto y{MakeYVector(id)};
		emit SignalAddGraphPlotResult(id, name, color, x, y);
	}
		break;
	case ParametersNS::Workmode::CompositionRange: {
		graphs_result_view[id].color = color;
		graphs_result_view[id].name = name;
		QVector<double> x(result_data.size());
		std::transform(result_data.cbegin(), result_data.cend(), x.begin(),
					   [](Optimization::OptimizationVector::const_reference i){
			return i.composition_variable;});
		auto y{MakeYVector(id)};
		emit SignalAddGraphPlotResult(id, name, color, x, y);
	}
		break;
	case ParametersNS::Workmode::TemperatureCompositionRange: {

		// 3d plot and heatmap
	}
		break;
	}
}

QVector<double> CoreApplication::MakeYVector(const GraphId id) const
{
	QVector<double> y(result_data.size());
	for(int i = 0, max = result_data.size(); i != max; ++i) {
		if(id.substance_id > 0) {
			assert(id.option == -1);
			const auto& val = parameters_.show_initial_in_result
					? result_data.at(i).amounts.at(id.substance_id)
					: result_data.at(i).amounts_of_equilibrium.at(id.substance_id);
			switch (parameters_.composition_result_unit) {
			case ParametersNS::CompositionUnit::AtomicPercent:
				y[i] = val.sum_atpct; break;
			case ParametersNS::CompositionUnit::WeightPercent:
				y[i] = val.sum_wtpct; break;
			case ParametersNS::CompositionUnit::Mol:
				y[i] = val.sum_mol; break;
			case ParametersNS::CompositionUnit::Gram:
				y[i] = val.sum_gram; break;
			}
		} else {
			assert(id.option >= 0);
			switch (static_cast<ResultFields::RowNames>(id.option)) {
			case ResultFields::RowNames::T_result:
				y[i] = Thermodynamics::FromKelvin(result_data.at(i).temperature_K_current,
												  parameters_.temperature_result_unit);
				break;
			case ResultFields::RowNames::T_initial:
				y[i] = Thermodynamics::FromKelvin(result_data.at(i).temperature_K_initial,
												  parameters_.temperature_result_unit);
				break;
			case ResultFields::RowNames::H_initial:
				y[i] = result_data.at(i).H_initial;
				break;
			case ResultFields::RowNames::H_equilibrium:
				y[i] = result_data.at(i).H_current;
				break;
			case ResultFields::RowNames::c_equilibrium:
				y[i] = result_data.at(i).result_of_optimization;
				break;
			case ResultFields::RowNames::Sum:
				const auto& sum = parameters_.show_initial_in_result
						? result_data.at(i).sum_of_initial
						: result_data.at(i).sum_of_equilibrium;
				switch (parameters_.composition_result_unit) {
				case ParametersNS::CompositionUnit::AtomicPercent:
					y[i] = sum.sum_atpct; break;
				case ParametersNS::CompositionUnit::WeightPercent:
					y[i] = sum.sum_wtpct; break;
				case ParametersNS::CompositionUnit::Mol:
					y[i] = sum.sum_mol; break;
				case ParametersNS::CompositionUnit::Gram:
					y[i] = sum.sum_gram; break;
				}
				break;
			}
		}
	}
	return y;
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

