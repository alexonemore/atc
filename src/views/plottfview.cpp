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

#include "plottfview.h"
#include <QSplitter>
#include <QHBoxLayout>
#include "parameters.h"
#include "utilities.h"

PlotTFView::PlotTFView(QWidget *parent)
	: QWidget(parent)
{
	table = new QTableView(this);
	plot = new Plot2DGraph(this);

	auto splitter = new QSplitter(this);
	splitter->addWidget(table);
	splitter->addWidget(plot);
	splitter->setChildrenCollapsible(false);
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 3);

	auto layout = new QHBoxLayout(this);
	setLayout(layout);
	layout->addWidget(splitter);

	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->setSelectionBehavior(QAbstractItemView::SelectItems);
	table->verticalHeader()->setVisible(false);

	plot->SetTitle(tr("Thermodynamic functions"));
	plot->SetAxisY1Name(QStringLiteral("G, H [KJ/mol]"));
	plot->SetAxisY2Name(QStringLiteral("F, S, Cp, c [J/molK]"));
	SetXAxisUnit(ParametersNS::TemperatureUnit::Kelvin);

}

PlotTFView::~PlotTFView()
{

}

void PlotTFView::SetModel(QAbstractItemModel* model)
{
	table->setModel(model);
}

void PlotTFView::SetXAxisUnit(const ParametersNS::TemperatureUnit unit)
{
	auto&& str = ParametersNS::temperature_units.at(static_cast<int>(unit));
	LOG(str)
	plot->SetAxisXName(tr("Temperature [%1]").arg(str));
}
