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

ResultView::ResultView(QWidget *parent)
	: QWidget{parent}
{
	table = new Table(this);
	plot2d_graph = new Plot2DGraph(this);
	plot2d_heatmap = new Plot2DHeatMap(this);
	plot3d = new Plot3DSurface(this);

	auto splitter_v1 = new QSplitter(Qt::Orientation::Horizontal, this);
	auto splitter_v2 = new QSplitter(Qt::Orientation::Horizontal, this);
	auto splitter_h1 = new QSplitter(Qt::Orientation::Vertical, this);

	splitter_v1->addWidget(table);
	splitter_v1->addWidget(splitter_h1);
	splitter_h1->addWidget(splitter_v2);
	splitter_h1->addWidget(plot3d);
	splitter_v2->addWidget(plot2d_graph);
	splitter_v2->addWidget(plot2d_heatmap);

	auto layout = new QHBoxLayout(this);
	setLayout(layout);
	layout->addWidget(splitter_v1);
	layout->setContentsMargins(0, 0, 0, 0);

}

ResultView::~ResultView()
{

}

void ResultView::SetModel(QAbstractItemModel* model)
{
	table->setModel(model);
}
