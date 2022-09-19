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
	qRegisterMetaType<GraphId>("GraphId");
	qRegisterMetaType<QVector<double>>("QVector<double>&");
	qRegisterMetaType<QVector<QVector<double>>>("QVector<QVector<double>>&");
	qRegisterMetaType<QVector<HeavyContainer>>("QVector<HeavyContainer>&");

	// GUI methods should be called only in this constructor,
	// but not in any other CoreApplication methods,
	// because after construct this object will be moved to another thread.
	// All communication with the GUI must be done through signals.

	// create models
	model_substances = new SubstancesTableModel(this);
	model_substances_temp_range = new SubstancesTempRangeModel(this);
	model_substances_tabulated_tf = new SubstancesTabulatedTFModel(this);
	model_plot_tf = new PlotTFModel(this);
	// demo
	table_1 = new QStringListModel(this);


	// set models
	gui->SetSubstancesTableModel(model_substances);
	gui->SetSubstancesTempRangeModel(model_substances_temp_range);
	gui->SetSubstancesTabulatedModel(model_substances_tabulated_tf);
	gui->SetTFPlotModel(model_plot_tf);
	gui->Initialize(); // must be called after set models

	// demo
	gui->SetModel_1(table_1);
	gui->SetModel_2(table_1);
	auto sel = new QItemSelectionModel(table_1);
	gui->SetSelectonModel(sel);

	// connects
	connect(gui, &MainWindow::SignalUpdate,
			this, &CoreApplication::SlotUpdate);
	connect(this, &CoreApplication::SignalSetAvailableElements,
			gui, &MainWindow::SlotSetAvailableElements);
	connect(gui, &MainWindow::SignalSubstancesTableSelection,
			this, &CoreApplication::SlotSubstancesTableSelectionHandler);
	connect(this, &CoreApplication::SignalSetSelectedSubstanceLabel,
			gui, &MainWindow::SlotSetSelectedSubstanceLabel);

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
	databases.push_back(new DatabaseThermo(ParametersNS::database_filenames.at(0)));
	databases.push_back(new DatabaseHSC(ParametersNS::database_filenames.at(1)));
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
	LOG(">> CORE HEAVY START <<")
	auto vec = PrepareHeavyCalculations();
	emit SignalStartHeavyComputations(vec);
	LOG(">> CORE HEAVY END <<")
}

void CoreApplication::SlotMake2DGraphData()
{
	LOG()
	static int id = 0;
	static RandomDouble rd{0.8, 1.2};
	const int size = 1000;
	QVector<double> x(size), y(size);
	std::iota(x.begin(), x.end(), 0);
	std::transform(x.cbegin(), x.cend(), y.begin(), [](double i){
		return (std::sin((i+rd())/50)*rd()+rd())/rd();
	});
	emit SignalShow2DGraphData(id++, x, y);
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
	QVector<HeavyContainer> vec(10);
	return vec;
}

Database* CoreApplication::CurrentDatabase()
{
	return databases.at(static_cast<int>(parameters_.database));
}

void CoreApplication::SlotUpdate(const ParametersNS::Parameters parameters)
{
	if(parameters.database != parameters_.database) {
		is_selected = false;
	}
	parameters_ = std::move(parameters);
	auto db = CurrentDatabase();
	auto data = db->GetSubstancesData(parameters_);

	SubstanceNames names(data.size());
#if 1 // structure slicing
	std::copy(data.cbegin(), data.cend(), names.begin());
#else // no slicing
	std::transform(data.cbegin(), data.cend(), names.begin(),
				   [](const SubstanceData& i){
		return SubstanceFormula{i.id, i.formula};
	});
#endif
	model_substances->SetNewData(std::move(data));
	model_plot_tf->SetNewData(std::move(names));
	UpdateRangeTabulatedModels();
	emit SignalSetAvailableElements(db->GetAvailableElements());
}

void CoreApplication::SlotSubstancesTableSelectionHandler(int id)
{
	selected_substance_id = id;
	is_selected = true;
	UpdateRangeTabulatedModels();
}

void CoreApplication::UpdateRangeTabulatedModels()
{
	if(is_selected) {
		auto db = CurrentDatabase();
		auto data_temp_range = db->GetSubstancesTempRangeData(
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

