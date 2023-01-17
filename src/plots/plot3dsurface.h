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

#ifndef PLOT3DSURFACE_H
#define PLOT3DSURFACE_H

#include <QWidget>
#include <QtDataVisualization/Q3DSurface>
//#include <QtDataVisualization/QCustom3DLabel>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtDataVisualization;
#endif

namespace Ui {
class Plot3DSurface;
}

class Plot3DSurface : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(Plot3DSurface)

public:
	explicit Plot3DSurface(QWidget *parent = nullptr);
	~Plot3DSurface() override;

	using AxisNames = std::tuple<QString, QString, QString>;

private:
	Ui::Plot3DSurface *ui;

private:
	Q3DSurface* plot;
	//QCustom3DLabel* plot_title;

private:
	QAction* a_remove_graph;
	QAction* a_save_text;
	QAction* a_save_image;

public:
	void SetTitle(const QString& title);
	void SetAxisNames(const QString& x_name, const QString& y_name,
					  const QString& z_name);
	void AddGraph(QSurfaceDataArray* data);
	void RemoveGraph();
	QString GetTitle() const;
	AxisNames GetAxisNames() const;
	void SaveImage();
	void SaveText();
	void ShowWireframe(bool visible);
	bool IsEmpty() const;

private slots:
	void SetTheme(int theme);
	void SetGradient(int index);
	void SetShadowQuality(int index);

private:
	void PrintGraphsToTextFile(QTextStream& stream, QString delimiter) const;

signals:
	void SignalGraphRemoved();
};

#endif // PLOT3DSURFACE_H
