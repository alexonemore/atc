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

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <vector>
#include <QThread>
#include <QMessageBox>
#include <QtConcurrent>
#include <cassert>
#include "utilities.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtDataVisualization;
#endif

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow{parent}
	, ui{new Ui::MainWindow}
{
	LOG()

	ui->setupUi(this);
	setWindowTitle(QStringLiteral("Adiabatic Temperature Calculator"));

	connect(ui->calculation_parameters, &CalculationParameters::UpdateParameters,
			this, &MainWindow::UpdateButtonHandler);
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

	//
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

}

MainWindow::~MainWindow()
{
	LOG()
	delete ui;
}

void MainWindow::Initialize()
{
	ui->view_substances->Initialize();
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

void MainWindow::SlotHeavyComputations(QVector<HeavyContainer>& ho)
{
	LOG(">> HEAVY START <<")
	Timer t; t.start();
	QThreadPool::globalInstance()->setMaxThreadCount(4);
	fw->setFuture(QtConcurrent::map(ho, &HeavyContainer::HeavyCalculations));
	//fw->setFuture(QtConcurrent::map(ho.begin(), ho.end(), &HeavyContainer::HeavyCalculations));
	dialog->exec();
	fw->waitForFinished();
#ifndef NDEBUG
	qDebug() << "isFinished" << fw->future().isFinished();
	qDebug() << "isCanceled" << fw->future().isCanceled();
	qDebug() << "isPaused  " << fw->future().isPaused();
	qDebug() << "isRunning " << fw->future().isRunning();
	qDebug() << "isStarted " << fw->future().isStarted();
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
	ui->plot2d->AddGraphY1(id, std::move(x), std::move(y));
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
	LOG()
	QMessageBox::about(this, tr("About ATC"),
					   tr("Text about <b>ATC</b> program"));
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

void MainWindow::UpdateButtonHandler(const ParametersNS::Parameters parameters)
{
	// TODO update mainwindow for new parameters
	// I don't know what to update in the mainwindow yet.

	// get selected substance



	emit SignalUpdate(parameters);
}


