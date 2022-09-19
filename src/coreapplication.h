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

#ifndef COREAPPLICATION_H
#define COREAPPLICATION_H

#include <QtConcurrent>
#include <QObject>
#include <QAbstractItemModel>
#include <QtDataVisualization>
#include "mainwindow.h"
#include "heavycontainer.h"
#include "database.h"
#include "substancestablemodel.h"
#include "substancestemprangemodel.h"
#include "substancestabulatedtfmodel.h"
#include "parameters.h"
#include "plottfmodel.h"

class CoreApplication : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(CoreApplication)

private:
	ParametersNS::Parameters parameters_{};
	QVector<Database*> databases;
	SubstancesTableModel* model_substances;
	SubstancesTempRangeModel* model_substances_temp_range;
	SubstancesTabulatedTFModel* model_substances_tabulated_tf;
	PlotTFModel* model_plot_tf;

	// selected substance in TempRange and TabulatedTF
	int selected_substance_id;
	bool is_selected{false};


	// demo
	QAbstractItemModel* table_1;

public:
	explicit CoreApplication(MainWindow *const mw, QObject *parent = nullptr);
	virtual ~CoreApplication() override;

signals:
	void SignalSetAvailableElements(const QStringList elements);
	void SignalSetSelectedSubstanceLabel(const QString& name);
	void SignalSetPlotXAxisUnit(const QString& unit);
	//demo
	void SignalShowResponse(const QString&);
	void SignalShowTime(const QString&);
	void SignalShowError(const QString&);
	void SignalShow2DGraphData(const GraphId& id, QVector<double>& x,
							   QVector<double>& y);
	void SignalShowHeatMapData(const QString& name, QVector<double>& x,
							   QVector<double>& y, QVector<QVector<double>>& z);
	void SignalShow3DGraphData(QSurfaceDataArray* data);
	void SignalStartHeavyComputations(QVector<HeavyContainer>& ho);

private slots:
	void SlotUpdate(const ParametersNS::Parameters parameters);
	void SlotSubstancesTableSelectionHandler(int id);

public slots:
	void Initialize();
	//demo
	void SlotRequestHandler(int i);
	void SlotPushButtonHandler(const QString& text);
	void SlotHeavyCalculations();
	void SlotMake2DGraphData();
	void SlotMakeHeatMapData();
	void SlotMake3DGraphData();

private:
	QVector<HeavyContainer> PrepareHeavyCalculations();
	Database* CurrentDatabase();
	void UpdateRangeTabulatedModels();
};

#endif // COREAPPLICATION_H
