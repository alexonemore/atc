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

#ifndef PLOTTFVIEW_H
#define PLOTTFVIEW_H

#include <QWidget>
#include <QTableView>
#include "plot2dgraph.h"
#include "parameters.h"
#include "specialdelegates.h"

class PlotTFView : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(PlotTFView)
private:
	QTableView* table;
	Plot2DGraph* plot;
	ColorPickerDelegate* color_delegate;
public:
	explicit PlotTFView(QWidget *parent = nullptr);
	~PlotTFView() override;
	void SetModel(QAbstractItemModel* model);
	void Initialize();
public slots:
	void AddGraph(const GraphId id, const QString& name, const QColor& color,
				  QVector<double>& x, QVector<double>& y);
	void RemoveGraph(const GraphId id);
	void ChangeColorGraph(const GraphId id, const QColor& color);

	void SetXAxisUnit(const ParametersNS::TemperatureUnit unit);

signals:
	void SignalAllGraphsRemoved();
	void SignalGraphColorChanged(const GraphId, const QColor&);
	void SignalGraphRemoved(const GraphId);
	void SignalGraphsRemoved(const QVector<GraphId>&);

};

#endif // PLOTTFVIEW_H
