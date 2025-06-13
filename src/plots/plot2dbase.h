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

#ifndef PLOT2DBASE_H
#define PLOT2DBASE_H

#include <QWidget>
#include "qcustomplot.h"
#include "plots.h"

class Plot2DBase : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(Plot2DBase)

protected:
	QCustomPlot* plot;

private:
	QCPTextElement* title;
	QCPItemTracer* tracer_cross;
	QCPItemText* tracer_text;

private:
	QMenu* menu;
	QAction* a_replot;
	QAction* a_tracer_show;
	QAction* a_tracer_hide;
	QAction* a_save_text;
	QAction* a_save_image;

private:
	bool show_tracer{false};

public:
	explicit Plot2DBase(QWidget *parent = nullptr);
	~Plot2DBase() override;

public:
	void HideTracer();
	void ShowTracer();
	void SetTracerVisible(const bool is_visible);
	void SetTitle(const QString& name);
	QString GetTitle() const;
	void SaveImage();
	void SaveText();

signals:
	void SignalPointClick(const QString&);

protected slots:
	void PlotTitleDoubleClick(QMouseEvent*);
	void PlotAxisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
	void PlotContextMenuRequest(QPoint pos);
	void UpdateTracerPosition(QMouseEvent* event);

protected:
	virtual void PrintGraphsToTextFile(QTextStream& stream, QString delimiter) const = 0;
	virtual QString MakeTextForTracer(const QPoint& cursor_px) const = 0;
	virtual void AddItemsToMenu(QMenu* menu) = 0;
	virtual void PlotSelectionChanged() = 0;
	virtual void PlotMousePress() = 0;
	virtual void PlotMouseWheel() = 0;
	virtual void Replot() = 0;

private:
	void SetupActionsBase();

};

#endif // PLOT2DBASE_H
