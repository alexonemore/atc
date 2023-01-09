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

#ifndef RESULTVIEW_H
#define RESULTVIEW_H

#include <QWidget>
#include "plot2dgraph.h"
#include "plot2dheatmap.h"
#include "plot3dsurface.h"

class ResultView : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(ResultView)
private:
	QTableView* table_check;
	Plot2DGraph* plot2d_graph;
	Plot2DHeatMap* plot2d_heatmap;
	Plot3DSurface* plot3d;
public:
	explicit ResultView(QWidget *parent = nullptr);
	~ResultView() override;
	void SetModel(QAbstractItemModel* model);
	void Initialize();
signals:

};

#endif // RESULTVIEW_H
