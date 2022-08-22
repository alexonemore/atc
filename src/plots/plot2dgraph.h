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

#ifndef PLOT2DGRAPH_H
#define PLOT2DGRAPH_H

#include "plot2dbase.h"
#include "dialogchangegraphsettings.h"

class Plot2DGraph : public Plot2DBase
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(Plot2DGraph)

private:
	enum class YAxis {
		y1, y2
	};

private:
	using GraphPointer = QCPGraph*;
	GraphMap<GraphPointer> graph_map;

private:
	QAction* a_legend_show;
	QAction* a_legend_hide;
	QAction* a_remove_all_graphs;
	QAction* a_remove_selected_graphs;
	QAction* a_change_selected_graphs_settings;
	QList<QAction*> a_legend_positions;

public:
	explicit Plot2DGraph(QWidget *parent = nullptr);
	~Plot2DGraph() override;

public:
	void HideLegend();
	void ShowLegend();
	void SetAxisXName(const QString& name);
	void SetAxisY1Name(const QString& name);
	void SetAxisY2Name(const QString& name);
	void SetAxisXRange(double min, double max);
	void SetAxisY1Range(double min, double max);
	void SetAxisY2Range(double min, double max);
	void AddGraphY1(const GraphId id, QVector<double>&& x, QVector<double>&& y);
	void AddGraphY1(const GraphId id, const QString& name,
					QVector<double>&& x, QVector<double>&& y);
	void AddGraphY2(const GraphId id, QVector<double>&& x, QVector<double>&& y);
	void AddGraphY2(const GraphId id, const QString& name,
					QVector<double>&& x, QVector<double>&& y);
	void SetGraphName(const GraphId id, const QString& name);
	void SetGraphColor(const GraphId id, const QColor& color);
	void RemoveGraph(const GraphId id);
	void RemoveSelectedGraphs();
	void RemoveAllGraphs();	
	void SetLegendVisible(const bool is_visible);
	void SetLegendPosition(const Qt::Alignment alignment);	
	QString GetAxisXName() const;
	QString GetAxisY1Name() const;
	QString GetAxisY2Name() const;
	QString GetGraphName(const GraphId id) const;
	QColor GetGraphColor(const GraphId id) const;
	GraphSettings GetGraphSettings(const GraphId id) const;
	void SetGraphSettings(const GraphId id, const GraphSettings gs);

signals:
	void SignalAllGraphsRemoved();
	void SignalGraphRemoved(const GraphId);
	void SignalGraphsRemoved(const QVector<GraphId>&);
	void SignalGraphColorChanged(const QColor&);
	void SignalGraphNameChanged(const QString&);

private slots:	
	void PlotLegendDoubleClick(QCPLegend*, QCPAbstractLegendItem* item);	
	void PlotGraphClicked(QCPAbstractPlottable* plottable_, int data_index);
	void SetLegendPosition();
	void ChangeSelectedGraphsSettings();

private: // Plot2DBase interface
	void PrintGraphsToTextFile(QTextStream& stream, QString delimitier) const override;
	QString MakeTextForTracer(const QPoint& cursor_px) const override;
	void AddItemsToMenu(QMenu* menu) override;
	void PlotSelectionChanged() override;
	void PlotMousePress() override;
	void PlotMouseWheel() override;
	void Replot() override;

private:
	void SetupActions();
	void AddGraph(const GraphId id, QVector<double>&& x, QVector<double>&& y,
				  YAxis axis);
	void AddGraph(const GraphId id, const QString& name,
				  QVector<double>&& x, QVector<double>&& y, YAxis axis);
	const QIcon& GetLegendPositionIcon() const;
	GraphSettings GetGraphSettings(const GraphPointer pgraph) const;
	void SetGraphSettings(const GraphPointer pgraph, const GraphSettings gs);
	void ErrorInvalidGraphId(const GraphId id);

protected: // QWidget interface
	void keyPressEvent(QKeyEvent* event) override;

};

#endif // PLOT2DGRAPH_H
