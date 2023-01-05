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

ResultView::ResultView(QWidget *parent)
	: QWidget{parent}
{
	table = new Table(this);
	table_data = new Table(this);
	plot2d_graph = new Plot2DGraph(this);
	plot2d_heatmap = new Plot2DHeatMap(this);
	plot3d = new Plot3DSurface(this);

	auto splitter = new QSplitter(Qt::Orientation::Horizontal, this);
	auto tabs = new QTabWidget(this);

	splitter->setChildrenCollapsible(false);
	splitter->addWidget(table);
	splitter->addWidget(tabs);
	tabs->addTab(table_data, tr("Table"));
	tabs->addTab(plot2d_graph, tr("Graph"));
	tabs->addTab(plot2d_heatmap, tr("HeatMap"));
	tabs->addTab(plot3d, tr("3DView"));

	auto layout = new QHBoxLayout(this);
	setLayout(layout);
	layout->addWidget(splitter);
	layout->setContentsMargins(0, 0, 0, 0);

	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->setSelectionBehavior(QAbstractItemView::SelectItems);
	table->verticalHeader()->setVisible(false);

}

ResultView::~ResultView()
{

}

void ResultView::SetModel(QAbstractItemModel* model)
{
	table->setModel(model);
}

void ResultView::Initialize()
{
	table->setColumnHidden(0, true);
}
