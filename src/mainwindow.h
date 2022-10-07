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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractItemView>
#include <QProgressDialog>
#include <QFuture>
#include <QtDataVisualization/QSurfaceDataArray>
#include "plots.h"
#include "heavycontainer.h"
#include "parameters.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtDataVisualization;
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(MainWindow)

private:
	Ui::MainWindow *ui;
	QProgressDialog* dialog;
	QFutureWatcher<void>* fw;
	QString database_path;

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;
public:
	void Initialize();
	void SetSubstancesTableModel(QAbstractItemModel* model);
	void SetSubstancesTempRangeModel(QAbstractItemModel* model);
	void SetSubstancesTabulatedModel(QAbstractItemModel* model);
	void SetTFPlotModel(QAbstractItemModel* model);
	void SetAmountsModel(QAbstractItemModel* model);
	// demo
	void SetModel_1(QAbstractItemModel* model);
	void SetModel_2(QAbstractItemModel* model);
	void SetSelectonModel(QItemSelectionModel* selection);

	// plot TF
public slots:
	void SlotAddGraphPlotTF(const GraphId id, const QString& name, const QColor& color,
				  QVector<double>& x, QVector<double>& y);
	void SlotRemoveGraphPlotTF(const GraphId id);
	void SlotChangeColorGraphPlotTF(const GraphId id, const QColor& color);
signals:
	void SignalAllGraphsRemovedPlotTF();
	void SignalGraphColorChangedPlotTF(const GraphId, const QColor&);
	void SignalGraphRemovedPlotTF(const GraphId);
	void SignalGraphsRemovedPlotTF(const QVector<GraphId>&);


	// demo
public slots:
	void SlotHeavyComputations(QVector<HeavyContainer>& ho);

public slots:
	void SlotAdd2DGraph(const GraphId& id, QVector<double>& x,
						QVector<double>& y);
	void SlotAddHeatMap(const QString& name, QVector<double>& x,
						QVector<double>& y, QVector<QVector<double>>& z);
	void SlotAdd3DGraph(QSurfaceDataArray* data);

public slots:
	void SlotSetAvailableElements(const QStringList& elements);
	void SlotSetSelectedSubstanceLabel(const QString& name);
	void SlotSetPlotXAxisUnit(const ParametersNS::TemperatureUnit unit);
	// demo
	void SlotShowResponse(const QString& text);
	void SlotShowRequest(const QString& text);
	void SlotShowError(const QString& text);
	void SlotShowStatusBarText(const QString& text);
	void SlotLoadDatabase();

private slots:
	void MenuOpenDatabase();
	void MenuShowAbout();

signals:
	void SignalUpdate(const ParametersNS::Parameters parameters);
	void SignalUpdateButtonClicked(const ParametersNS::Parameters parameters);
	void SignalSubstancesTableSelection(int id);
	//demo
	void SignalSendRequest(int);
	void SignalPushButtonClicked(QString);
	void SignalHeavyCalculationsStart();
	void SignalNeed2DGraphData();
	void SignalNeedHeatMapData();
	void SignalNeed3DGraphData();

private:
	void SetupMenu();
	void UpdateButtonHandler(const ParametersNS::Parameters parameters);
	//demo
	void CheckButtonHandler();
	void PushButtonHandler();
	void PushButtonHeavyHandler();

};
#endif // MAINWINDOW_H
