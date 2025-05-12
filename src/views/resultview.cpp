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

#include "resultview.h"
#include <QSplitter>
#include <QTabWidget>
#include "parameters.h"
#include "resultmodel.h"
#include "utilities.h"

namespace ResultViewGraph {
const auto axis_temperature = QStringLiteral("Temperature, %1");
const auto axis_composition = QStringLiteral("Composition, %1");
const auto y1_axis_name = QStringLiteral("T [%1], H [kJ/mol], c");
const auto y2_axis_name = QStringLiteral("Composition, %1");
}

ResultView::ResultView(QWidget *parent)
    : QWidget{parent}
{
    table_check = new QTableView(this);
    plot2d_graph = new Plot2DGraph(this);
    plot2d_heatmap = new Plot2DHeatMap(this);
    plot3d = new Plot3DSurface(this);

    auto splitter = new QSplitter(Qt::Orientation::Horizontal, this);
    auto tabs = new QTabWidget(this);

    splitter->addWidget(table_check);
    splitter->addWidget(tabs);
    tabs->addTab(plot2d_graph, tr("Graph"));
    tabs->addTab(plot2d_heatmap, tr("HeatMap"));
    tabs->addTab(plot3d, tr("3DView"));
    splitter->setChildrenCollapsible(false);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

    auto layout = new QHBoxLayout(this);
    setLayout(layout);
    layout->addWidget(splitter);
    layout->setContentsMargins(0, 0, 0, 0);

    table_check->setSelectionMode(QAbstractItemView::SingleSelection);
    table_check->setSelectionBehavior(QAbstractItemView::SelectItems);
    table_check->verticalHeader()->setVisible(false);

    plot2d_graph->SetTitle(tr(""));
    plot2d_graph->SetAxisXName(ResultViewGraph::axis_temperature.arg("K"));
    plot2d_graph->SetAxisY1Name(ResultViewGraph::y1_axis_name.arg(tr("K")));
    plot2d_graph->SetAxisY2Name(ResultViewGraph::y2_axis_name.arg(tr("mol")));

    plot2d_heatmap->SetTitle(tr("Heatmap"));
    plot2d_heatmap->SetAxisXName(ResultViewGraph::axis_composition.arg("mol"));
    plot2d_heatmap->SetAxisYName(ResultViewGraph::axis_temperature.arg("K"));

    plot3d->SetTitle(tr(""));
    plot3d->SetAxisNames(ResultViewGraph::axis_composition.arg("mol"),
                         ResultViewGraph::axis_temperature.arg("K"),
                         ResultViewGraph::axis_temperature.arg("K"));

    color_delegate = new ColorPickerDelegate(this);
    table_check->setItemDelegateForColumn(
                static_cast<int>(ResultFields::ColNames::Show),
                color_delegate);

    connect(plot2d_graph, &Plot2DGraph::SignalAllGraphsRemoved,
            this, &ResultView::SignalAllGraphsRemoved);
    connect(plot2d_graph, &Plot2DGraph::SignalGraphColorChanged,
            this, &ResultView::SignalGraphColorChanged);
    connect(plot2d_graph, &Plot2DGraph::SignalGraphRemoved,
            this, &ResultView::SignalGraphRemoved);
    connect(plot2d_graph, &Plot2DGraph::SignalGraphsRemoved,
            this, &ResultView::SignalGraphsRemoved);

    connect(plot2d_heatmap, &Plot2DHeatMap::SignalGraphRemoved,
            this, &ResultView::SignalAllGraphsRemoved);
    connect(plot3d, &Plot3DSurface::SignalGraphRemoved,
            this, &ResultView::SignalAllGraphsRemoved);

}

ResultView::~ResultView()
{

}

void ResultView::SetModel(QAbstractItemModel* model)
{
    table_check->setModel(model);
}

void ResultView::Initialize()
{
    table_check->setColumnHidden(static_cast<int>(ResultFields::ColNames::ID), true);
    table_check->setColumnWidth(static_cast<int>(ResultFields::ColNames::Show),
                                ParametersNS::GUISize::table_column_width);
}

void ResultView::AddGraph(const GraphId id, const QString& name,
                          const QColor& color, QVector<double>& x,
                          QVector<double>& y)
{
    if(id.substance_id > 0) {
        assert(id.option == -1);
        plot2d_graph->AddGraphY2(id, name, std::move(x), std::move(y), color);
    } else {
        assert(id.option >= 0);
        if(id.option == static_cast<int>(ResultFields::RowNames::Sum)) {
            plot2d_graph->AddGraphY2(id, name, std::move(x), std::move(y), color);
        } else {
            plot2d_graph->AddGraphY1(id, name, std::move(x), std::move(y), color);
        }
    }
}

void ResultView::AddHeatMap(const QString& name, QVector<double>& x, QVector<double>& y,
                            QVector<QVector<double> >& z)
{
    plot2d_heatmap->AddHeatMap(name, std::move(x), std::move(y), std::move(z));
}

void ResultView::Add3DGraph(const QString& name, QSurfaceDataArray* data)
{
    plot3d->AddGraph(data);
    plot3d->SetAxisYName(name);
    plot3d->SetTitle(name);
}

void ResultView::RemoveGraph(const GraphId id)
{
    plot2d_graph->RemoveGraph(id);
}

void ResultView::ChangeColorGraph(const GraphId id, const QColor& color)
{
    plot2d_graph->SetGraphColor(id, color);
}

void ResultView::SetAxisUnits(const ParametersNS::Parameters params)
{
    switch (params.workmode) {
    case ParametersNS::Workmode::SinglePoint:
        break;
    case ParametersNS::Workmode::TemperatureRange:
        plot2d_graph->SetAxisXName(ResultViewGraph::axis_temperature.
            arg(params.GetTemperatureResultUnit()));
        break;
    case ParametersNS::Workmode::CompositionRange:
        plot2d_graph->SetAxisXName(ResultViewGraph::axis_composition.
            arg(params.GetCompositionRangeUnit()));
        break;
    case ParametersNS::Workmode::TemperatureCompositionRange:
        plot2d_heatmap->SetAxisXName(ResultViewGraph::axis_composition.
            arg(params.GetCompositionRangeUnit()));
        plot2d_heatmap->SetAxisYName(ResultViewGraph::axis_temperature.
            arg(params.GetTemperatureResultUnit()));
        plot3d->SetAxisXName(ResultViewGraph::axis_composition.
            arg(params.GetCompositionRangeUnit()));
        plot3d->SetAxisZName(ResultViewGraph::axis_temperature.
            arg(params.GetTemperatureResultUnit()));
        break;
    }
    plot2d_graph->SetAxisY1Name(ResultViewGraph::y1_axis_name.
        arg(params.GetTemperatureResultUnit()));
    plot2d_graph->SetAxisY2Name(ResultViewGraph::y2_axis_name.
        arg(params.GetCompositionResultUnit()));
}
