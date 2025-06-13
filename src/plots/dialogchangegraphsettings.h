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

#ifndef DIALOGCHANGEGRAPHSETTINGS_H
#define DIALOGCHANGEGRAPHSETTINGS_H

#include <QDialog>
#include <QFont>
#include <QColor>
#include "qcustomplot.h"

struct GraphSettings {
	QString name;
	QCPGraph::LineStyle line_style;
	QPen line_pen; // include style, width, brush, capStyle and joinStyle
	QCPScatterStyle scatter_style; // include size, shape, pen, brush
};

namespace Ui {
class DialogChangeGraphSettings;
}

class DialogChangeGraphSettings : public QDialog
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(DialogChangeGraphSettings)

public:
	explicit DialogChangeGraphSettings(const GraphSettings previous,
									   const QString name,
									   QWidget *parent = nullptr);
	~DialogChangeGraphSettings() override;
	GraphSettings GetGraphSettings() const;

private:
	Ui::DialogChangeGraphSettings *ui;

private slots:
	void ChangeName(const QString& name);
	void ChangeLineGraphStyle(int index);
	void ChangeLineStyle(int index);
	void ChangeLineWidth(double width);
	void ChangeLineColor();
	void ChangeScatterShape(int index);
	void ChangeScatterSize(double width);
	void ChangeScatterLineWidth(double width);
	void ChangeScatterLineColor();
	void ChangeScatterFillColor();

};

#endif // DIALOGCHANGEGRAPHSETTINGS_H
