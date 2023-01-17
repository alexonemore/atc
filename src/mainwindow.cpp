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

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <vector>
#include <QThread>
#include <QMessageBox>
#include <QtConcurrent>
#include <cassert>
#include "utilities.h"
#include "amountsmodel.h"
#include "specialdelegates.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtDataVisualization;
#endif

namespace win {
const QString win_logo = QStringLiteral("images/logo_atc.svg");
const QString win_title =
#ifndef NDEBUG
			QStringLiteral("Adiabatic Temperature Calculator (DEBUG)");
#else
			QStringLiteral("Adiabatic Temperature Calculator");
#endif
const QString win_title_short = QStringLiteral("ATC");
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow{parent}
	, ui{new Ui::MainWindow}
{
	LOG()

	ui->setupUi(this);
	setWindowTitle(win::win_title);
	setWindowIcon(QIcon(win::win_logo));

	connect(ui->calculation_parameters, &CalculationParameters::UpdateParameters,
			this, &MainWindow::UpdateParametersHandler); // SignalUpdate
	connect(ui->calculation_parameters, &CalculationParameters::UpdateButtonClicked,
			this, &MainWindow::SignalUpdateButtonClicked);
	connect(ui->calculation_parameters, &CalculationParameters::StartCalculate,
			this, &MainWindow::SignalStartCalculate);

	//demo
	std::vector<QAbstractButton*> check_butons{
		ui->check_button_1,
		ui->check_button_2,
		ui->check_button_3
	};
	std::vector<QAbstractButton*> buttons{
		ui->button_1,
		ui->button_2
	};
	for(auto&& button : check_butons) {
		connect(button, &QRadioButton::clicked,
				this, &MainWindow::CheckButtonHandler);
	}
	for(auto&& button : buttons) {
		connect(button, &QPushButton::clicked,
				this, &MainWindow::PushButtonHandler);
	}
	connect(ui->button_heavy, &QPushButton::clicked,
			this, &MainWindow::PushButtonHeavyHandler);


	SetupMenu();

	// setup dialog
	dialog = new QProgressDialog(this);
	dialog->setWindowTitle(win::win_title_short);
	dialog->setModal(true);
	dialog->setMinimumDuration(100);
	dialog->setLabelText(tr("Calculating ..."));
	dialog->close();

	fw = new QFutureWatcher<void>(this);
	connect(fw, &QFutureWatcher<void>::started, dialog,
			static_cast<void(QProgressDialog::*)()>(&QProgressDialog::open));
	connect(fw,	&QFutureWatcher<void>::finished,
			dialog,	&QProgressDialog::reset);
	connect(dialog,	&QProgressDialog::canceled,
			fw,	&QFutureWatcher<void>::cancel);
	connect(fw,	&QFutureWatcher<void>::progressRangeChanged,
			dialog,	&QProgressDialog::setRange);
	connect(fw,	&QFutureWatcher<void>::progressValueChanged,
			dialog,	&QProgressDialog::setValue);

	// DEMO
	connect(ui->plot2d, &Plot2DGraph::SignalPointClick,
			this, &MainWindow::SlotShowStatusBarText);
	connect(ui->plotheatmap, &Plot2DHeatMap::SignalPointClick,
			this, &MainWindow::SlotShowStatusBarText);
	connect(ui->add_2d_graph, &QPushButton::clicked,
			this, &MainWindow::SignalNeed2DGraphData);
	connect(ui->add_heat_map, &QPushButton::clicked,
			this, &MainWindow::SignalNeedHeatMapData);
	connect(ui->add_3d_graph, &QPushButton::clicked,
			this, &MainWindow::SignalNeed3DGraphData);

	// internal selections
	connect(ui->view_substances, &SubstancesTableView::SelectSubstance,
			this, &MainWindow::SignalSubstancesTableSelection);

	// plot TF
	connect(ui->plot_tf_view, &PlotTFView::SignalAllGraphsRemoved,
			this, &MainWindow::SignalAllGraphsRemovedPlotTF);
	connect(ui->plot_tf_view, &PlotTFView::SignalGraphColorChanged,
			this, &MainWindow::SignalGraphColorChangedPlotTF);
	connect(ui->plot_tf_view, &PlotTFView::SignalGraphRemoved,
			this, &MainWindow::SignalGraphRemovedPlotTF);
	connect(ui->plot_tf_view, &PlotTFView::SignalGraphsRemoved,
			this, &MainWindow::SignalGraphsRemovedPlotTF);

	// plot Result
	connect(ui->result_view, &ResultView::SignalAllGraphsRemoved,
			this, &MainWindow::SignalAllGraphsRemovedPlotResult);
	connect(ui->result_view, &ResultView::SignalGraphColorChanged,
			this, &MainWindow::SignalGraphColorChangedPlotResult);
	connect(ui->result_view, &ResultView::SignalGraphRemoved,
			this, &MainWindow::SignalGraphRemovedPlotResult);
	connect(ui->result_view, &ResultView::SignalGraphsRemoved,
			this, &MainWindow::SignalGraphsRemovedPlotResult);

	// amounts
	auto double_delegate = new DoubleNumberDelegate(this);
	ui->amounts_view->setItemDelegateForColumn(static_cast<int>(AmountsModelFields::Names::Group_1_mol), double_delegate);
	ui->amounts_view->setItemDelegateForColumn(static_cast<int>(AmountsModelFields::Names::Group_1_gram), double_delegate);
	ui->amounts_view->setItemDelegateForColumn(static_cast<int>(AmountsModelFields::Names::Group_2_mol), double_delegate);
	ui->amounts_view->setItemDelegateForColumn(static_cast<int>(AmountsModelFields::Names::Group_2_gram), double_delegate);
	ui->amounts_view->setItemDelegateForColumn(static_cast<int>(AmountsModelFields::Names::Sum_mol), double_delegate);
	ui->amounts_view->setItemDelegateForColumn(static_cast<int>(AmountsModelFields::Names::Sum_gram), double_delegate);
	ui->amounts_view->verticalHeader()->setVisible(false);
	connect(ui->amounts_view, &Table::Delete,
			this, &MainWindow::SignalAmountsTableDelete);
}

MainWindow::~MainWindow()
{
	LOG()
	delete ui;
}

void MainWindow::Initialize()
{
	ui->view_substances->Initialize();
	ui->plot_tf_view->Initialize();
	ui->result_view->Initialize();
	ui->amounts_view->setColumnHidden(static_cast<int>(AmountsModelFields::Names::ID), true);
}

void MainWindow::SetSubstancesTableModel(QAbstractItemModel* model)
{
	LOG()
	ui->view_substances->SetSubstancesTableModel(model);
}

void MainWindow::SetSubstancesTempRangeModel(QAbstractItemModel* model)
{
	LOG()
	ui->view_substances->SetSubstancesTempRangeModel(model);
}

void MainWindow::SetSubstancesTabulatedModel(QAbstractItemModel* model)
{
	LOG()
	ui->view_substances->SetSubstancesTabulatedModel(model);
}

void MainWindow::SetTFPlotModel(QAbstractItemModel* model)
{
	LOG()
	ui->plot_tf_view->SetModel(model);
}

void MainWindow::SetAmountsModel(QAbstractItemModel* model)
{
	LOG()
	ui->amounts_view->setModel(model);
}

void MainWindow::SetResultModel(QAbstractItemModel* model)
{
	LOG()
	ui->result_view->SetModel(model);
}

void MainWindow::SetResultDetailModel(QAbstractItemModel* model)
{
	LOG()
	ui->result_table_view->setModel(model);
}

void MainWindow::SetModel_1(QAbstractItemModel* model)
{
	LOG()
	ui->table_view->setModel(model);
}

void MainWindow::SetModel_2(QAbstractItemModel* model)
{
	LOG()
	ui->list_view->setModel(model);
}

void MainWindow::SetSelectonModel(QItemSelectionModel* selection)
{
	LOG()
	ui->table_view->setSelectionModel(selection);
	ui->list_view->setSelectionModel(selection);
}

void MainWindow::SlotAddGraphPlotTF(const GraphId id, const QString& name,
			const QColor& color, QVector<double>& x, QVector<double>& y)
{
	ui->plot_tf_view->AddGraph(id, name, color, x, y);
}

void MainWindow::SlotRemoveGraphPlotTF(const GraphId id)
{
	ui->plot_tf_view->RemoveGraph(id);
}

void MainWindow::SlotChangeColorGraphPlotTF(const GraphId id, const QColor& color)
{
	LOG(color)
	ui->plot_tf_view->ChangeColorGraph(id, color);
}

void MainWindow::SlotAddGraphPlotResult(const GraphId id, const QString& name,
	const QColor& color, QVector<double>& x, QVector<double>& y)
{
	LOG(name)
	ui->result_view->AddGraph(id, name, color, x, y);
}

void MainWindow::SlotAddHeatmapPlotResult(const GraphId id, const QString& name,
	const QColor& color, QVector<double>& x, QVector<double>& y, QVector<QVector<double> >& z)
{
	LOG()
	ui->result_view->AddHeatMap(name, x, y, z);
}

void MainWindow::SlotAdd3DGraphPlotResult(const GraphId id, const QString& name,
										  const QColor& color, QSurfaceDataArray* data)
{
	LOG()
	ui->result_view->Add3DGraph(name, data);
}

void MainWindow::SlotRemoveGraphPlotResult(const GraphId id)
{
	ui->result_view->RemoveGraph(id);
}

void MainWindow::SlotChangeColorGraphPlotResult(const GraphId id, const QColor& color)
{
	ui->result_view->ChangeColorGraph(id, color);
}

void MainWindow::SlotSetPlotResultAxisUnit(const ParametersNS::Parameters params)
{
	ui->result_view->SetAxisUnits(params);
}

void MainWindow::SlotStartCalculations(Optimization::OptimizationVector& vec,
									   int threads)
{
	Timer t; t.start();
	if(vec.size() <= 1) {
		LOG(">> CALCULATION START FOR <= 1 ITEM <<")
		for(auto&& i : vec) i.Calculate();
	} else {
		LOG(">> CALCULATION START <<")
		QThreadPool::globalInstance()->setMaxThreadCount(threads);
		fw->setFuture(QtConcurrent::map(vec, &Optimization::OptimizationItem::Calculate));
		// fw->setFuture(QtConcurrent::map(vec.begin(), vec.end(), &Optimization::OptimizationItem::Calculate));
		dialog->exec();
		fw->waitForFinished();
		if(fw->future().isCanceled()) {
			vec.clear();
		}
#ifndef NDEBUG
		qDebug() << "isFinished " << fw->future().isFinished();
		qDebug() << "isCanceled " << fw->future().isCanceled();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		qDebug() << "isPaused   " << fw->future().isPaused();
#else
		qDebug() << "isSuspended" << fw->future().isSuspended();
#endif
		qDebug() << "isRunning  " << fw->future().isRunning();
		qDebug() << "isStarted  " << fw->future().isStarted();
#endif
		assert(fw->future().isFinished());
	}
	t.stop();
	QString time{tr("Time: %1 Threads: %2").arg(t.duration()).arg(threads)};
	LOG(time)
	SlotShowStatusBarText(time);
	QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
	emit SignalSendResult(vec);
	LOG(">> CALCULATION END <<")
}

void MainWindow::SlotHeavyComputations(QVector<HeavyContainer>& ho) // demo
{
	LOG(">> HEAVY START <<")
	Timer t; t.start();
	QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());
	fw->setFuture(QtConcurrent::map(ho, &HeavyContainer::HeavyCalculations));
	//fw->setFuture(QtConcurrent::map(ho.begin(), ho.end(), &HeavyContainer::HeavyCalculations));
	dialog->exec();
	fw->waitForFinished();
#ifndef NDEBUG
	qDebug() << "isFinished " << fw->future().isFinished();
	qDebug() << "isCanceled " << fw->future().isCanceled();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	qDebug() << "isPaused   " << fw->future().isPaused();
#else
	qDebug() << "isSuspended" << fw->future().isSuspended();
#endif
	qDebug() << "isRunning  " << fw->future().isRunning();
	qDebug() << "isStarted  " << fw->future().isStarted();
#endif
	assert(fw->future().isFinished());
	t.stop();
	QString text{tr("Time: %1 Threads: %2").arg(t.duration()).
				arg(QThreadPool::globalInstance()->maxThreadCount())};
	SlotShowStatusBarText(text);
	LOG(">> HEAVY END <<")
}

void MainWindow::CheckButtonHandler()
{
	int cb{0};
	if(ui->check_button_1->isChecked()) {
		cb = 1;
	} else if (ui->check_button_2->isChecked()) {
		cb = 2;
	} else if (ui->check_button_3->isChecked()) {
		cb = 3;
	}
	emit SignalSendRequest(cb);
	LOG(cb)
	QString text{tr("Pressed: %1 Thread: %2").arg(cb).
				arg(QThread::currentThread()->objectName())};
	SlotShowRequest(text);
}

void MainWindow::PushButtonHandler()
{
	LOG()
	auto&& text = sender()->objectName();
	emit SignalPushButtonClicked(text);
}

void MainWindow::PushButtonHeavyHandler()
{
	LOG()
	emit SignalHeavyCalculationsStart();
}

void MainWindow::SlotShowResponse(const QString& text)
{
	LOG()
	ui->text_responce->appendPlainText(text);
}

void MainWindow::SlotShowRequest(const QString& text)
{
	LOG()
	ui->text_request->appendPlainText(text);
}

void MainWindow::SlotShowError(const QString& text)
{
	LOG()
	QMessageBox::critical(this, tr("Error"), text);
}

void MainWindow::SlotShowStatusBarText(const QString& text)
{
	LOG()
	statusBar()->showMessage(text);
}

void MainWindow::SlotAdd2DGraph(const GraphId& id, QVector<double>& x,
								QVector<double>& y)
{
	LOG()
	ui->plot2d->AddGraphY1(id, QString::number(id.substance_id), std::move(x),
						   std::move(y), GetRandomColor());
}

void MainWindow::SlotAddHeatMap(const QString& name, QVector<double>& x,
								QVector<double>& y, QVector<QVector<double>>& z)
{
	LOG()
	ui->plotheatmap->AddHeatMap(name, std::move(x), std::move(y), std::move(z));
}

void MainWindow::SlotAdd3DGraph(QSurfaceDataArray* data)
{
	LOG()
	ui->plot3d->AddGraph(data);
}

void MainWindow::SlotSetAvailableElements(const QStringList& elements)
{
	ui->calculation_parameters->SetEnabledElements(elements);
}

void MainWindow::SlotSetSelectedSubstanceLabel(const QString& name)
{
	ui->view_substances->SetSelectedSubstanceLabel(name);
}

void MainWindow::SlotSetPlotXAxisUnit(const ParametersNS::TemperatureUnit unit)
{
	ui->plot_tf_view->SetXAxisUnit(unit);
}

void MainWindow::SlotLoadDatabase()
{
	LOG()
	// save last open path

	QString new_path = QFileDialog::getOpenFileName(this, tr("Open Database"),
			database_path, QStringLiteral("SQLite3 (*.db);;All Files (*)"));

	if (new_path.isEmpty())
		return;

	// Database db(new_path);
	// if(!db.isNull()) {
	//     loadData(db);
	//     database_path = new_path;
	// }

}

void MainWindow::MenuShowAbout()
{
#ifndef NDEBUG
	int major, minor, bugfix;
	nlopt::version(major, minor, bugfix);
	const QString nlopt_version = tr("%1.%2.%3").arg(major).arg(minor).arg(bugfix);
#endif
	const QString text_about = tr(
				"<h3>ATC Adiabatic Temperature Calculator</h3>"
				"<p>ATC is a high quality, visual, open source tool "
				"for thermodynamic calculations. "
				"See <a href=\"https://%4/\">%4</a> for more information.</p>"
				"<p>ATC is licensed under the GNU General Public License Version 3.</p>"
				"<p>Copyright (c) %1 %2</p>"
				"<p>Corresponding email: <a href=\"mailto:%3\">%3</a></p>"
#ifndef NDEBUG
				"<p>This program uses:</p>"
				"<ul><li>Qt version %5</li>"
				"<li>NLopt version %6</li>"
				"<li>QCustomPlot version %7</li></ul>"
#endif
				).arg(QStringLiteral("2023")
					  , QStringLiteral("Alexandr Shchukin")
					  , QStringLiteral("alexonemoreemail@gmail.com")
					  , QStringLiteral("github.com/alexonemore/atc")
#ifndef NDEBUG
					  , QLatin1String(QT_VERSION_STR)
					  , nlopt_version
					  , QLatin1String(QCUSTOMPLOT_VERSION_STR)
#endif
					  );
	QMessageBox::about(this, tr("About ATC"), text_about);
}

void MainWindow::MenuOpenDatabase()
{
	LOG()
	// TODO
}

void MainWindow::SetupMenu()
{
	LOG()
	auto a_opendb = new QAction(tr("&Open database..."), this);
	a_opendb->setShortcuts(QKeySequence::Open);
	a_opendb->setStatusTip(tr("Open an existing file"));
	connect(a_opendb, &QAction::triggered, this, &MainWindow::MenuOpenDatabase);

	auto a_exit = new QAction(tr("E&xit"), this);
	a_exit->setShortcuts(QKeySequence::Quit);
	a_exit->setStatusTip(tr("Exit the application"));
	connect(a_exit, &QAction::triggered, qApp, &QApplication::quit);

	auto a_about = new QAction(tr("&About"), this);
	a_about->setStatusTip(tr("Show the application's About box"));
	connect(a_about, &QAction::triggered, this, &MainWindow::MenuShowAbout);

	auto file_menu = menuBar()->addMenu(tr("&File"));
	auto edit_menu = menuBar()->addMenu(tr("&Edit"));
	auto view_menu = menuBar()->addMenu(tr("&View"));
	auto help_menu = menuBar()->addMenu(tr("&Help"));

	file_menu->addAction(a_opendb);
	file_menu->addSeparator();
	file_menu->addAction(a_exit);

	edit_menu->addAction(new QAction("template", this));

	view_menu->addAction(new QAction("template", this));

	help_menu->addAction(a_about);

}

void MainWindow::UpdateParametersHandler(const ParametersNS::Parameters parameters)
{
	// TODO update mainwindow for new parameters
	// I don't know what to update in the mainwindow yet.
	emit SignalUpdate(parameters);
}



