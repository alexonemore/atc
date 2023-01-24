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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractItemView>
#include <QProgressDialog>
#include <QFuture>
#include <QtDataVisualization/QSurfaceDataArray>
#include "plots.h"
#include "parameters.h"
#include "optimization.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtDataVisualization;
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace win {
extern const QString atc_version;
extern const QString win_title;
extern const QString win_title_short;
extern const QString win_logo;
}

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
	void SetResultModel(QAbstractItemModel* model);
	void SetResultDetailModel(QAbstractItemModel* model);

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

	// plot Result
public slots:
	void SlotAddGraphPlotResult(const GraphId id, const QString& name, const QColor& color,
								QVector<double>& x, QVector<double>& y);
	void SlotAddHeatmapPlotResult(const GraphId id, const QString& name, const QColor& color,
								  QVector<double>& x, QVector<double>& y,
								  QVector<QVector<double>>& z);
	void SlotAdd3DGraphPlotResult(const GraphId id, const QString& name, const QColor& color,
								  QSurfaceDataArray* data);
	void SlotRemoveGraphPlotResult(const GraphId id);
	void SlotChangeColorGraphPlotResult(const GraphId id, const QColor& color);
	void SlotSetPlotResultAxisUnit(const ParametersNS::Parameters params);
signals:
	void SignalAllGraphsRemovedPlotResult();
	void SignalGraphColorChangedPlotResult(const GraphId, const QColor&);
	void SignalGraphRemovedPlotResult(const GraphId);
	void SignalGraphsRemovedPlotResult(const QVector<GraphId>&);

public slots:
	void SlotStartCalculations(Optimization::OptimizationVector& vec, int threads);

public slots:
	void SlotSetAvailableElements(const QStringList& elements);
	void SlotSetSelectedSubstanceLabel(const QString& name);
	void SlotSetPlotXAxisUnit(const ParametersNS::TemperatureUnit unit);
	void SlotShowStatusBarText(const QString& text);
	void SlotShowError(const QString& text);

private slots:
	void MenuShowAbout();

signals:
	void SignalSendResult(Optimization::OptimizationVector& vec);
	void SignalUpdate(const ParametersNS::Parameters parameters);
	void SignalUpdateButtonClicked(const ParametersNS::Parameters parameters);
	void SignalStartCalculate();
	void SignalSubstancesTableSelection(int id);
	void SignalAmountsTableDelete(const QModelIndexList& selected);
	//demo
	void SignalSendRequest(int);
	void SignalPushButtonClicked(QString);
	void SignalHeavyCalculationsStart();

private:
	void SetupMenu();
	void UpdateParametersHandler(const ParametersNS::Parameters parameters);

};
#endif // MAINWINDOW_H
