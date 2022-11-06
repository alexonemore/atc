/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2022 Alexandr Shchukin
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

#ifndef PLOT2DHEATMAP_H
#define PLOT2DHEATMAP_H

#include "plot2dbase.h"

class Plot2DHeatMap : public Plot2DBase
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(Plot2DHeatMap)

public:
	QVector<double> x_;
	QVector<double> y_;
	QCPColorMap* heat_map;
	QCPColorScale* color_scale;

private:
	QAction* a_remove_graph;
	QAction* a_interpolate_enable;
	QAction* a_interpolate_disable;
	QList<QAction*> a_gradients;

public:
	explicit Plot2DHeatMap(QWidget *parent = nullptr);
	~Plot2DHeatMap() override;

public:
	void SetAxisXName(const QString& name);
	void SetAxisYName(const QString& name);
	void AddHeatMap(const QString& name, QVector<double>&& x,
					QVector<double>&& y, QVector<QVector<double>>&& z);
	void AddHeatMap(const QString& name, QVector<double>&& x,
					QVector<double>&& y, QVector<QVector<double>>&& z,
					Plot::Range range);
	void SetColorGradient(const QCPColorGradient& gradient);
	void SetInterpolation(const bool enabled);
	void InterpolationEnable();
	void InterpolationDisable();
	void RemoveGraph();
	QString GetAxisXName() const;
	QString GetAxisYName() const;

signals:
	void SignalGraphRemoved();

private slots:	
	void SetColorGradient();	

private: // Plot2DBase interface
	void PrintGraphsToTextFile(QTextStream& stream, QString delimiter) const override;
	QString MakeTextForTracer(const QPoint& cursor_px) const override;
	void AddItemsToMenu(QMenu* menu) override;
	void PlotSelectionChanged() override;
	void PlotMousePress() override;
	void PlotMouseWheel() override;
	void Replot() override;

private:
	void SetupActions();
	void MousePressHandler(QMouseEvent* event);

};



#endif // PLOT2DHEATMAP_H
