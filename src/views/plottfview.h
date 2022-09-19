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

#ifndef PLOTTFVIEW_H
#define PLOTTFVIEW_H

#include <QWidget>
#include <QTableView>
#include "plot2dgraph.h"

class PlotTFView : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(PlotTFView)
private:
	QTableView* table;
	Plot2DGraph* plot;
public:
	explicit PlotTFView(QWidget *parent = nullptr);
	~PlotTFView() override;
public slots:
	void SlotSetXAxisUnit(const QString& temperature_unit);

signals:

};

#endif // PLOTTFVIEW_H
