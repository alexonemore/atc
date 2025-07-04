/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2025 Alexandr Shchukin
 * Corresponding email: shchukin.aleksandr.sergeevich@gmail.com
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

#include <QObject>
#include <QAbstractItemModel>
#include <QtDataVisualization>
#include <unordered_map>
#include <QColor>
#include "mainwindow.h"
#include "database.h"
#include "substancestablemodel.h"
#include "substancestemprangemodel.h"
#include "substancestabulatedtfmodel.h"
#include "parameters.h"
#include "plottfmodel.h"
#include "amountsmodel.h"
#include "resultmodel.h"
#include "optimization.h"

class CoreApplication : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(CoreApplication)

private:
	ParametersNS::Parameters parameters_{};
	QVector<std::shared_ptr<Database>> databases;
	SubstancesTableModel* model_substances;
	SubstancesTempRangeModel* model_substances_temp_range;
	SubstancesTabulatedTFModel* model_substances_tabulated_tf;
	PlotTFModel* model_plot_tf;
	AmountsModel* model_amounts;
	ResultModel* model_result;
	ResultDetailModel* model_detail_result;

	Optimization::OptimizationVector result_data;
	int y_size{0};
	int x_size{0};

	// selected substance in TempRange and TabulatedTF
	int selected_substance_id;
	bool is_selected{false};

	struct GraphParams {
		QColor color;
		QString name;
	};
	std::unordered_map<GraphId, GraphParams> graphs_tf_view;
	std::unordered_map<GraphId, GraphParams> graphs_result_view;

public:
	explicit CoreApplication(MainWindow *const mw, QObject *parent = nullptr);
	virtual ~CoreApplication() override;

signals:
	void SignalSetAvailableElements(const QStringList elements);
	void SignalSetSelectedSubstanceLabel(const QString& name);
	void SignalSetPlotXAxisUnit(const ParametersNS::TemperatureUnit unit);

	void SignalStartCalculations(Optimization::OptimizationVector& vec,
								 int threads);

	void SignalError(const QString& text);

private slots:
	void SlotUpdate(const ParametersNS::Parameters parameters);
	void SlotUpdateButtonHandler(const ParametersNS::Parameters parameters);
	void SlotSubstancesTableSelectionHandler(int id);
	// calculate
	void SlotStartCalculations();
	void SlotResieveResult(Optimization::OptimizationVector& vec);

	// tf plots: model to view
private slots:
	void SlotAddGraphPlotTF(const GraphId id, const QString& name, const QColor& color);
	void SlotRemoveGraphPlotTF(const GraphId id);
	void SlotChangeColorGraphPlotTF(const GraphId id, const QColor& color);
signals:
	void SignalAddGraphPlotTF(const GraphId id, const QString& name, const QColor& color,
				  QVector<double>& x, QVector<double>& y);
	void SignalRemoveGraphPlotTF(const GraphId id);
	void SignalChangeColorGraphPlotTF(const GraphId id, const QColor& color);
	// tf plots: view to model
private slots:
	void SlotAllGraphsRemovedPlotTFVtM();
	void SlotGraphColorChangedPlotTFVtM(const GraphId id, const QColor& color);
	void SlotGraphRemovedPlotTFVtM(const GraphId id);
	void SlotGraphsRemovedPlotTFVtM(const QVector<GraphId>& ids);

	// result plot
private slots:
	void SlotAddGraphPlotResult(const GraphId id, const QString& name, const QColor& color);
	void SlotRemoveGraphPlotResult(const GraphId id);
	void SlotChangeColorGraphPlotResult(const GraphId id, const QColor& color);
signals:
	void SignalAddGraphPlotResult(const GraphId id, const QString& name, const QColor& color,
				  QVector<double>& x, QVector<double>& y);
	void SignalAddHeatmapPlotResult(const GraphId id, const QString& name, const QColor& color,
									QVector<double>& x, QVector<double>& y,
									QVector<QVector<double>>& z);
	void SignalAdd3DGraphPlotResult(const GraphId id, const QString& name, const QColor& color,
									QSurfaceDataArray* data);
	void SignalRemoveGraphPlotResult(const GraphId id);
	void SignalChangeColorGraphPlotResult(const GraphId id, const QColor& color);
	void SignalSetPlotResultAxisUnit(const ParametersNS::Parameters params);
private slots: // result plots: view to model
	void SlotAllGraphsRemovedPlotResultVtM();
	void SlotGraphColorChangedPlotResultVtM(const GraphId id, const QColor& color);
	void SlotGraphRemovedPlotResultVtM(const GraphId id);
	void SlotGraphsRemovedPlotResultVtM(const QVector<GraphId>& ids);

public slots:
	void Initialize();

private:
	auto CurrentDatabase();
	auto Database(ParametersNS::Database database);
	void UpdateRangeTabulatedModels();
	QVector<double> MakeYVector(const GraphId id) const;
	double ChooseValueInResultData(const GraphId id, const int index) const;
	void MakeHeatmapAnd3DVectors(const GraphId id,
								 QVector<double>& composition,
								 QVector<double>& temperatures,
								 QVector<QVector<double>>& values,
								 QSurfaceDataArray* data) const;
	QString GetUnits(const GraphId id) const;

};

#endif // COREAPPLICATION_H
