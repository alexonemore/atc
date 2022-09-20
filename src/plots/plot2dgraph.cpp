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

#include "plot2dgraph.h"
#include "utilities.h"
#include <QHBoxLayout>
#include <array>

namespace {
constexpr double y_default_min = 0.0;
constexpr double y_default_max = 500.0;
constexpr double x_default_min = 300.0;
constexpr double x_default_max = 1000.0;
}

namespace Plot {
struct PositionIcon {
	Qt::Alignment alignment;
	QString name;
	QString filename;
	QIcon icon;
};
static std::array position_icons{
	PositionIcon{(Qt::AlignTop | Qt::AlignLeft),		QStringLiteral("Top left"),			QStringLiteral("images/tl.svg"), QIcon{}},
	PositionIcon{(Qt::AlignTop | Qt::AlignHCenter),		QStringLiteral("Top center"),		QStringLiteral("images/tc.svg"), QIcon{}},
	PositionIcon{(Qt::AlignTop | Qt::AlignRight),		QStringLiteral("Top right"),		QStringLiteral("images/tr.svg"), QIcon{}},
	PositionIcon{(Qt::AlignVCenter | Qt::AlignLeft),	QStringLiteral("Middle left"),		QStringLiteral("images/ml.svg"), QIcon{}},
	PositionIcon{(Qt::AlignVCenter | Qt::AlignHCenter),	QStringLiteral("Middle center"),	QStringLiteral("images/mc.svg"), QIcon{}},
	PositionIcon{(Qt::AlignVCenter | Qt::AlignRight),	QStringLiteral("Middle right"),		QStringLiteral("images/mr.svg"), QIcon{}},
	PositionIcon{(Qt::AlignBottom | Qt::AlignLeft),		QStringLiteral("Bottom left"),		QStringLiteral("images/bl.svg"), QIcon{}},
	PositionIcon{(Qt::AlignBottom | Qt::AlignHCenter),	QStringLiteral("Bottom center"),	QStringLiteral("images/bc.svg"), QIcon{}},
	PositionIcon{(Qt::AlignBottom | Qt::AlignRight),	QStringLiteral("Bottom right"),		QStringLiteral("images/br.svg"), QIcon{}}
};
static void MakePositionIcons();
}

Plot2DGraph::Plot2DGraph(QWidget *parent) : Plot2DBase{parent}
{
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
						  QCP::iSelectAxes | QCP::iSelectLegend |
						  QCP::iSelectPlottables);
	plot->legend->setVisible(false);
	plot->xAxis2->setVisible(true);
	plot->xAxis2->setTickLabels(false);
	plot->yAxis2->setVisible(true);
	plot->xAxis->setRange(x_default_min, x_default_max);
	plot->xAxis2->setRange(x_default_min, x_default_max);
	plot->yAxis->setRange(y_default_min, y_default_max);
	plot->yAxis2->setRange(y_default_min, y_default_max);
	SetAxisXName("x");
	SetAxisY1Name("y1");
	SetAxisY2Name("y2");
	SetTitle(tr("Plot2DGraph"));

	QFont legendFont = font();
	legendFont.setPointSize(10);
	plot->legend->setFont(legendFont);
	plot->legend->setSelectedFont(legendFont);
	plot->legend->setSelectableParts(QCPLegend::spItems);

	connect(plot, &QCustomPlot::legendDoubleClick,
			this, &Plot2DGraph::PlotLegendDoubleClick);
	connect(plot->xAxis,
			static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),
			plot->xAxis2,
			static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::setRange));
	connect(plot, &QCustomPlot::plottableClick,
			this, &Plot2DGraph::PlotGraphClicked);

	SetupActions();
	plot->replot();
}

Plot2DGraph::~Plot2DGraph()
{
}

void Plot2DGraph::Replot()
{
	plot->rescaleAxes();
	plot->replot();
}

void Plot2DGraph::SetupActions()
{
	a_legend_show = new QAction(tr("Show legend"), this);
	connect(a_legend_show, &QAction::triggered, this, &Plot2DGraph::ShowLegend);

	a_legend_hide = new QAction(QIcon("images/clear.svg"), tr("Hide legend"), this);
	connect(a_legend_hide, &QAction::triggered, this, &Plot2DGraph::HideLegend);

	a_remove_all_graphs = new QAction(tr("Remove all graphs"), this);
	connect(a_remove_all_graphs, &QAction::triggered,
			this, &Plot2DGraph::RemoveAllGraphs);

	a_remove_selected_graphs = new QAction(tr("Remove selected graph"), this);
	connect(a_remove_selected_graphs, &QAction::triggered,
			this, &Plot2DGraph::RemoveSelectedGraphs);

	a_change_selected_graphs_settings = new QAction(
				tr("Setup selected graph..."), this);
	connect(a_change_selected_graphs_settings, &QAction::triggered,
			this, &Plot2DGraph::ChangeSelectedGraphsSettings);

	Plot::MakePositionIcons();
	for(const auto& position : Plot::position_icons) {
		auto action = new QAction(position.icon, position.name, this);
		action->setData(static_cast<int>(position.alignment));
		connect(action, &QAction::triggered, this,
				static_cast<void(Plot2DGraph::*)()>(&Plot2DGraph::SetLegendPosition));
		a_legend_positions.push_back(action);
	}
}

void Plot2DGraph::AddItemsToMenu(QMenu* menu)
{
	if(plot->legend->visible()) {
		menu->addAction(a_legend_hide);
		auto menu_legend_position = new QMenu(tr("Legend position"), menu);
		menu_legend_position->addActions(a_legend_positions);
		menu->addMenu(menu_legend_position);
	} else {
		a_legend_show->setIcon(GetLegendPositionIcon());
		menu->addAction(a_legend_show);
	}
	menu->addSeparator();

	if(plot->graphCount() > 0) {
		if(plot->selectedGraphs().size() > 0) {
			menu->addAction(a_change_selected_graphs_settings);
			menu->addAction(a_remove_selected_graphs);
		}
		menu->addAction(a_remove_all_graphs);
		menu->addSeparator();
	}
}

void Plot2DGraph::HideLegend()
{
	SetLegendVisible(false);
}

void Plot2DGraph::ShowLegend()
{
	SetLegendVisible(true);
}

void Plot2DGraph::SetAxisXName(const QString& name)
{
	plot->xAxis->setLabel(name);
	plot->replot();
}

void Plot2DGraph::SetAxisY1Name(const QString& name)
{
	plot->yAxis->setLabel(name);
	plot->replot();
}

void Plot2DGraph::SetAxisY2Name(const QString& name)
{
	plot->yAxis2->setLabel(name);
	plot->replot();
}

void Plot2DGraph::SetAxisXRange(double min, double max)
{
	plot->xAxis->setRange(min, max);
	Replot();
}

void Plot2DGraph::SetAxisY1Range(double min, double max)
{
	plot->yAxis->setRange(min, max);
	Replot();
}

void Plot2DGraph::SetAxisY2Range(double min, double max)
{
	plot->yAxis2->setRange(min, max);
	Replot();
}

void Plot2DGraph::AddGraphY1(const GraphId id, QVector<double>&& x,
						QVector<double>&& y)
{
	AddGraph(id, std::move(x), std::move(y), YAxis::y1);
}

void Plot2DGraph::AddGraphY1(const GraphId id, const QString& name,
						QVector<double>&& x, QVector<double>&& y)
{
	AddGraph(id, name, std::move(x), std::move(y), YAxis::y1);
}

void Plot2DGraph::AddGraphY2(const GraphId id, QVector<double>&& x,
						QVector<double>&& y)
{
	AddGraph(id, std::move(x), std::move(y), YAxis::y2);
}

void Plot2DGraph::AddGraphY2(const GraphId id, const QString& name,
						QVector<double>&& x, QVector<double>&& y)
{
	AddGraph(id, name, std::move(x), std::move(y), YAxis::y2);
}

void Plot2DGraph::AddGraph(const GraphId id, QVector<double>&& x,
					  QVector<double>&& y, YAxis axis)
{
	AddGraph(id, QString{">> %1"}.arg(id), std::move(x), std::move(y), axis);
}

void Plot2DGraph::AddGraph(const GraphId id, const QString& name,
					  QVector<double>&& x, QVector<double>&& y, YAxis axis)
{
	QCPGraph* pgraph;
	switch(axis) {
	case YAxis::y1:
		pgraph = plot->addGraph(plot->xAxis, plot->yAxis);
		break;
	case YAxis::y2:
		pgraph = plot->addGraph(plot->xAxis, plot->yAxis2);
		break;
	}
	if(pgraph == nullptr) return;
	pgraph->setData(x, y, true);
	pgraph->setName(name);
	pgraph->setLineStyle(QCPGraph::LineStyle::lsLine);
	pgraph->setScatterStyle(QCPScatterStyle::ScatterShape::ssNone);
	pgraph->setPen(QPen{GetRandomColor()});
	auto it = graph_map.find(id);
	if(it != graph_map.end()) {
		plot->removeGraph(it->second);
		graph_map.erase(it);
	}
	graph_map.emplace(id, pgraph);
	plot->rescaleAxes();
	plot->legend->setVisible(true);
	LOG()
	plot->replot(/*QCustomPlot::RefreshPriority::rpQueuedReplot*/);
}

void Plot2DGraph::SetGraphName(const GraphId id, const QString& name)
{
	auto it = graph_map.find(id);
	if(it != graph_map.end()) {
		auto pgraph = it->second;
		pgraph->setName(name);
		plot->replot();
	} else {
		ErrorInvalidGraphId(id);
	}
}

void Plot2DGraph::SetGraphColor(const GraphId id, const QColor& color)
{
	auto it = graph_map.find(id);
	if(it != graph_map.end()) {
		auto pgraph = it->second;
		auto brush = pgraph->brush();
		brush.setColor(color);
		pgraph->setBrush(brush);
		plot->replot();
	} else {
		ErrorInvalidGraphId(id);
	}
}

void Plot2DGraph::RemoveGraph(const GraphId id)
{
	auto it = graph_map.find(id);
	if(it != graph_map.end()) {
		auto pgraph = it->second;
		plot->removeGraph(pgraph);
		graph_map.erase(it);
		if(plot->graphCount() > 0) plot->legend->setVisible(true);
		else plot->legend->setVisible(false);
		plot->replot();
		emit SignalGraphRemoved(id);
	} else {
		ErrorInvalidGraphId(id);
	}
}

void Plot2DGraph::RemoveSelectedGraphs()
{
	auto selected_graphs = plot->selectedGraphs();
	QVector<GraphId> ids_to_remove;
	ids_to_remove.reserve(selected_graphs.size());
	for(const auto& [id, pgraph] : graph_map) {
		if(selected_graphs.contains(pgraph)) {
			ids_to_remove.push_back(id);
		}
	}
	for(const auto& pgraph : selected_graphs) {
		plot->removeGraph(pgraph);
	}
	for(const auto& id : ids_to_remove) {
		graph_map.erase(id);
	}
	if(plot->graphCount() > 0) plot->legend->setVisible(true);
	else plot->legend->setVisible(false);
	plot->replot();
	emit SignalGraphsRemoved(ids_to_remove);
}

void Plot2DGraph::RemoveAllGraphs()
{
	graph_map.clear();
	plot->clearGraphs();
	if(plot->graphCount() > 0) plot->legend->setVisible(true);
	else plot->legend->setVisible(false);
	plot->replot();
	emit SignalAllGraphsRemoved();
}

void Plot2DGraph::SetLegendVisible(const bool is_visible)
{
	plot->legend->setVisible(is_visible);
	plot->replot();
}

void Plot2DGraph::SetLegendPosition(const Qt::Alignment alignment)
{
	plot->axisRect()->insetLayout()->setInsetAlignment(0, alignment);
	plot->replot();
}

void Plot2DGraph::SetLegendPosition()
{
	// make sure this slot is really called by a context menu
	// action, so it carries the data we need
	if(QAction* context_action = qobject_cast<QAction*>(sender())) {
		bool ok;
		int alignment = context_action->data().toInt(&ok);
		if(ok) {
			SetLegendPosition(static_cast<Qt::Alignment>(alignment));
		}
	}
}

QString Plot2DGraph::GetAxisXName() const
{
	return plot->xAxis->label();
}

QString Plot2DGraph::GetAxisY1Name() const
{
	return plot->yAxis->label();
}

QString Plot2DGraph::GetAxisY2Name() const
{
	return plot->yAxis2->label();
}

QString Plot2DGraph::GetGraphName(const GraphId id) const
{
	auto it = graph_map.find(id);
	if(it != graph_map.end()) {
		auto pgraph = it->second;
		return pgraph->name();
	} else {
		return QStringLiteral("Invalid GraphId");
	}
}

QColor Plot2DGraph::GetGraphColor(const GraphId id) const
{
	auto it = graph_map.find(id);
	if(it != graph_map.end()) {
		auto pgraph = it->second;
		return pgraph->brush().color();
	} else {
		return QColor{Qt::GlobalColor::black};
	}
}

void Plot2DGraph::PrintGraphsToTextFile(QTextStream& stream, QString delimitier) const
{
	for(int i = 0, max = plot->graphCount(); i != max; ++i) {
		auto pgraph = plot->graph(i);
		auto data = pgraph->data();
		stream << GetAxisXName() << delimitier;
		for(const auto& d : *data) {
			stream << d.mainKey() << delimitier;
		}
		stream << "\n";
		stream << pgraph->name() << delimitier;
		for(const auto& d : *data) {
			stream << d.mainValue() << delimitier;
		}
		stream << "\n";
	}
}

void Plot2DGraph::PlotSelectionChanged()
{
	if(plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
	   plot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
	   plot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
	   plot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		plot->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		plot->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}

	if(plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
	   plot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		plot->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}

	if(plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
	   plot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		plot->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}

	// synchronize selection of graphs with selection of corresponding legend
	// items (loop useful when multiple selection enabled):
	for(int i = 0, max = plot->graphCount(); i < max; ++i)
	{
		QCPGraph* pgraph = plot->graph(i);
		QCPPlottableLegendItem* item = plot->legend->itemWithPlottable(pgraph);
		if(item->selected() || pgraph->selected()) {
			item->setSelected(true);
			pgraph->setSelection(QCPDataSelection(pgraph->data()->dataRange()));
		}
	}
}

void Plot2DGraph::PlotMousePress()
{
	if(plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeDragAxes(plot->xAxis,plot->yAxis);
		plot->axisRect()->setRangeDrag(plot->xAxis->orientation());
	}
	else if(plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeDragAxes(plot->xAxis,plot->yAxis);
		plot->axisRect()->setRangeDrag(plot->yAxis->orientation());
	}
	else if(plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeDragAxes(plot->xAxis2,plot->yAxis2);
		plot->axisRect()->setRangeDrag(plot->yAxis2->orientation());
	}
	else {
		plot->axisRect()->setRangeDragAxes({plot->xAxis, /*m_plot->xAxis2,*/
											plot->yAxis, plot->yAxis2});
		plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	}
}

void Plot2DGraph::PlotMouseWheel()
{
	if(plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeZoomAxes(plot->xAxis, plot->yAxis);
		plot->axisRect()->setRangeZoom(plot->xAxis->orientation());
	}
	else if(plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeZoomAxes(plot->xAxis, plot->yAxis);
		plot->axisRect()->setRangeZoom(plot->yAxis->orientation());
	}
	else if(plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeZoomAxes(plot->xAxis2, plot->yAxis2);
		plot->axisRect()->setRangeZoom(plot->yAxis2->orientation());
	}
	else {
		plot->axisRect()->setRangeZoomAxes({plot->xAxis, /*m_plot->xAxis2,*/
											plot->yAxis, plot->yAxis2});
		plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
	}
}

void Plot2DGraph::PlotGraphClicked(QCPAbstractPlottable* plottable_, int data_index)
{
	// TODO not call while tracer is shown, possible due to different layer
	LOG("data index:", data_index)
	// since we know we only have QCPGraphs in the plot, we can immediately
	// access interface1D() usually it's better to first check whether
	// interface1D() returns non-zero, and only then use it.
	auto interface_ = plottable_->interface1D();
	if(interface_) {
		double x = interface_->dataMainKey(data_index);
		double y = interface_->dataMainValue(data_index);
		auto text = QString {"%1 %2: %3 %4: %5"}.arg(
					plottable_->name(), GetAxisXName(), QString::number(x),
					GetAxisY1Name(), QString::number(y));
		// TODO get axis name y2 for y2 graphs
		emit SignalPointClick(text);
	}
}

void Plot2DGraph::keyPressEvent(QKeyEvent* event)
{
	LOG(event)
	auto key = static_cast<QKeyEvent*>(event)->key();
	if(key == Qt::Key_Delete || key == Qt::Key_Backspace) {
		RemoveSelectedGraphs();
	} else {
		QWidget::keyPressEvent(event);
	}
}

QString Plot2DGraph::MakeTextForTracer(const QPoint& cursor_px) const
{
	double x  = plot->xAxis ->pixelToCoord(cursor_px.x());
	double y1 = plot->yAxis ->pixelToCoord(cursor_px.y());
	double y2 = plot->yAxis2->pixelToCoord(cursor_px.y());
	return QString{"%1: %2\n%3: %4\n%5: %6"}.arg(
		GetAxisXName(), QString::number(x),
		GetAxisY1Name(), QString::number(y1),
		GetAxisY2Name(), QString::number(y2));
}

void Plot::MakePositionIcons()
{
	for(auto&& position : Plot::position_icons) {
		position.icon = QIcon{position.filename};
	}
}

const QIcon& Plot2DGraph::GetLegendPositionIcon() const
{
	auto alignment = plot->axisRect()->insetLayout()->insetAlignment(0);
	auto it = std::find_if(Plot::position_icons.cbegin(),
						   Plot::position_icons.cend(), [alignment](auto i){
		return alignment == i.alignment; });
	if(it != Plot::position_icons.end()) {
		return it->icon;
	} else {
		return Plot::position_icons.cbegin()->icon;
	}
}

GraphSettings Plot2DGraph::GetGraphSettings(const GraphId id) const
{
	auto it = graph_map.find(id);
	if(it != graph_map.end()) {
		auto pgraph = it->second;
		return GetGraphSettings(pgraph);
	} else {
		return GraphSettings{};
	}
}

GraphSettings Plot2DGraph::GetGraphSettings(const GraphPointer pgraph) const
{
	return GraphSettings { pgraph->name(),
				pgraph->lineStyle(),
				pgraph->pen(),
				pgraph->scatterStyle() };
}

void Plot2DGraph::SetGraphSettings(const GraphId id, const GraphSettings gs)
{
	auto it = graph_map.find(id);
	if(it != graph_map.end()) {
		auto pgraph = it->second;
		SetGraphSettings(pgraph, gs);
	} else {
		ErrorInvalidGraphId(id);
	}
}

void Plot2DGraph::SetGraphSettings(const GraphPointer pgraph,
								   const GraphSettings gs)
{
	pgraph->setName(gs.name);
	pgraph->setLineStyle(gs.line_style);
	pgraph->setPen(gs.line_pen);
	pgraph->setScatterStyle(gs.scatter_style);
	plot->replot();
}

void Plot2DGraph::PlotLegendDoubleClick(QCPLegend*, QCPAbstractLegendItem* item)
{
	// only react if item was clicked (user could have clicked on
	// border padding of legend where there is no item, then item is 0)
	if(item) {
		auto pl_item = qobject_cast<QCPPlottableLegendItem*>(item);
		auto pgraph = qobject_cast<QCPGraph*>(pl_item->plottable());
		if(pgraph) {
			DialogChangeGraphSettings dialog(GetGraphSettings(pgraph),
										tr("Graph Settings"), this);
			if(dialog.exec() == QDialog::Accepted) {
				SetGraphSettings(pgraph, dialog.GetGraphSettings());
			}
		}
	}
}

void Plot2DGraph::ChangeSelectedGraphsSettings()
{
	auto selected_graphs = plot->selectedGraphs();
	GraphSettings settings;
	if(!selected_graphs.isEmpty()) {
		settings = GetGraphSettings(selected_graphs.first());
	}
	DialogChangeGraphSettings dialog(settings, tr("Graph Settings"), this);
	if(dialog.exec() == QDialog::Accepted) {
		settings = dialog.GetGraphSettings();
	}
	for(auto&& pgraph : selected_graphs) {
		SetGraphSettings(pgraph, settings);
	}
}

void Plot2DGraph::ErrorInvalidGraphId(const GraphId id)
{
	QMessageBox::critical(this, tr("Error"),
						  QString{tr("Invalid GraphId: ") + QString::number(id)},
						  QMessageBox::Ok, QMessageBox::Ok);
}


