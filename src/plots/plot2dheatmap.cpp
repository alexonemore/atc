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

#include "plot2dheatmap.h"
#include "utilities.h"
#include <QHBoxLayout>
#include <array>

namespace Plot {
struct Gradient {
	QString name;
	QCPColorGradient::GradientPreset preset;
	QIcon icon;
};
auto& gradients()
{
	static std::array gradients{
		Gradient{QStringLiteral("Grayscale"),	QCPColorGradient::GradientPreset::gpGrayscale,	QIcon{}},
		Gradient{QStringLiteral("Hot"),			QCPColorGradient::GradientPreset::gpHot,		QIcon{}},
		Gradient{QStringLiteral("Cold"),		QCPColorGradient::GradientPreset::gpCold,		QIcon{}},
		Gradient{QStringLiteral("Night"),		QCPColorGradient::GradientPreset::gpNight,		QIcon{}},
		Gradient{QStringLiteral("Candy"),		QCPColorGradient::GradientPreset::gpCandy,		QIcon{}},
		Gradient{QStringLiteral("Geography"),	QCPColorGradient::GradientPreset::gpGeography,	QIcon{}},
		Gradient{QStringLiteral("Ion"),			QCPColorGradient::GradientPreset::gpIon,		QIcon{}},
		Gradient{QStringLiteral("Thermal"),		QCPColorGradient::GradientPreset::gpThermal,	QIcon{}},
		Gradient{QStringLiteral("Polar"),		QCPColorGradient::GradientPreset::gpPolar,		QIcon{}},
		Gradient{QStringLiteral("Spectrum"),	QCPColorGradient::GradientPreset::gpSpectrum,	QIcon{}},
		Gradient{QStringLiteral("Jet"),			QCPColorGradient::GradientPreset::gpJet,		QIcon{}},
		Gradient{QStringLiteral("Hues"),		QCPColorGradient::GradientPreset::gpHues,		QIcon{}}
	};
	return gradients;
}
static void MakeGradientIcons();

}

Plot2DHeatMap::Plot2DHeatMap(QWidget *parent)
	: Plot2DBase{parent}
{
	plot->axisRect()->setupFullAxesBox(true);
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
	plot->legend->setVisible(false);
	plot->xAxis2->setVisible(true);
	plot->xAxis2->setTickLabels(false);
	plot->yAxis2->setVisible(true);
	plot->yAxis2->setTickLabels(false);
	SetAxisXName("x");
	SetAxisYName("y");
	SetTitle(tr("Plot2DHeatMap"));

	// heat map
	heat_map = new QCPColorMap(plot->xAxis, plot->yAxis);
	color_scale = new QCPColorScale(plot);
	color_scale->setType(QCPAxis::atRight);
	color_scale->setVisible(false);
	plot->plotLayout()->addElement(1, 1, color_scale);

	heat_map->setTightBoundary(true);
	heat_map->setColorScale(color_scale);
	heat_map->setGradient(QCPColorGradient::gpHot);
	heat_map->setInterpolate(false);
	heat_map->setVisible(true);
	heat_map->data()->clear();
	heat_map->removeFromLegend();

	// synchronize the axis rect and color scale bottom and top margins:
	QCPMarginGroup *marginGroup = new QCPMarginGroup(plot);
	plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
	color_scale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

	connect(plot, &QCustomPlot::mousePress,
			this, &Plot2DHeatMap::MousePressHandler);

	SetupActions();
	plot->replot();
}

Plot2DHeatMap::~Plot2DHeatMap()
{
}

void Plot2DHeatMap::SetAxisXName(const QString& name)
{
	plot->xAxis->setLabel(name);
}

void Plot2DHeatMap::SetAxisYName(const QString& name)
{
	plot->yAxis->setLabel(name);
}

void Plot2DHeatMap::AddHeatMap(const QString& name, QVector<double>&& x,
							   QVector<double>&& y, QVector<QVector<double>>&& z)
{
	auto [xmin, xmax] = std::minmax_element(x.cbegin(), x.cend());
	auto [ymin, ymax] = std::minmax_element(y.cbegin(), y.cend());
	Plot::Range range{*xmin, *xmax, *ymin, *ymax};
	AddHeatMap(name, std::move(x), std::move(y), std::move(z), range);
}

void Plot2DHeatMap::AddHeatMap(const QString& name, QVector<double>&& x,
							   QVector<double>&& y, QVector<QVector<double>>&& z,
							   Plot::Range range)
{
	const auto x_size = x.size();
	const auto y_size = y.size();
	LOG("sizes x:", x_size, "y:", y_size, "z:", z.size() * z.at(0).size())
	assert(x.size() > 0 && "x.size() <= 0");
	assert(y.size() > 0 && "y.size() <= 0");
	assert(z.size() == y.size() && "z.size() != y.size()");
	assert(z.at(0).size() == x.size() && "z.at(0).size() != x.size()");
	assert(z.at(0).size() * z.size() == x_size * y_size &&
		   "z.at(0).size() * z.size() != x_size * y_size");

	heat_map->data()->clear();
	heat_map->data()->setSize(x_size, y_size);
	heat_map->data()->setRange(QCPRange(range.x_min, range.x_max),
							   QCPRange(range.y_min, range.y_max));
	for(int yi = 0; yi < y_size; ++yi) {
		for(int xi = 0; xi < x_size; ++xi) {
			heat_map->data()->setData(x[xi], y[yi], z[yi][xi]);
		}
	}

	x_ = std::move(x);
	y_ = std::move(y);

	SetTitle(name);
	heat_map->rescaleDataRange(true);
	color_scale->setVisible(true);
	plot->rescaleAxes();
	plot->replot();
}

void Plot2DHeatMap::SetColorGradient(const QCPColorGradient& gradient)
{
	heat_map->setGradient(gradient);
	plot->replot();
}

void Plot2DHeatMap::SetInterpolation(const bool enabled)
{
	heat_map->setInterpolate(enabled);
	plot->replot();
}

void Plot2DHeatMap::InterpolationEnable()
{
	SetInterpolation(true);
}

void Plot2DHeatMap::InterpolationDisable()
{
	SetInterpolation(false);
}

void Plot2DHeatMap::RemoveGraph()
{
	heat_map->data()->clear();
	plot->clearGraphs();
	color_scale->setVisible(false);
	x_.clear();
	y_.clear();
	plot->replot();
	emit SignalGraphRemoved();
}

QString Plot2DHeatMap::GetAxisXName() const
{
	return plot->xAxis->label();
}

QString Plot2DHeatMap::GetAxisYName() const
{
	return plot->yAxis->label();
}

void Plot2DHeatMap::PrintGraphsToTextFile(QTextStream& stream, QString delimiter) const
{
	if(!heat_map->data()->isEmpty()) {
		stream << GetTitle() << "\n";
		stream << GetAxisYName() << "\\" << GetAxisXName() << delimiter;
		for(int xi = 0, x_max = x_.size(); xi != x_max; ++xi) {
			stream << x_.at(xi) << delimiter;
		}
		stream << "\n";
		const auto& data = heat_map->data();
		for(int yi = 0, y_max = y_.size(); yi != y_max; ++yi) {
			stream << y_.at(yi) << delimiter;
			for(int xi = 0, x_max = x_.size(); xi != x_max; ++xi) {
				stream << data->cell(xi, yi) << delimiter;
			}
			stream << "\n";
		}
	}
}

void Plot2DHeatMap::PlotSelectionChanged()
{
	if(plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
	   plot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
	   plot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
	   plot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		plot->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		plot->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}

	if(plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
	   plot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
	   plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
	   plot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		plot->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		plot->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}
}

void Plot2DHeatMap::PlotMousePress()
{
	if(plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeDragAxes(plot->xAxis,plot->yAxis);
		plot->axisRect()->setRangeDrag(plot->xAxis->orientation());
	}
	else if(plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeDragAxes(plot->xAxis,plot->yAxis);
		plot->axisRect()->setRangeDrag(plot->yAxis->orientation());
	}
	else {
		plot->axisRect()->setRangeDragAxes({plot->xAxis, plot->yAxis});
		plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	}
}

void Plot2DHeatMap::PlotMouseWheel()
{
	if(plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeZoomAxes(plot->xAxis, plot->yAxis);
		plot->axisRect()->setRangeZoom(plot->xAxis->orientation());
	}
	else if(plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		plot->axisRect()->setRangeZoomAxes(plot->xAxis, plot->yAxis);
		plot->axisRect()->setRangeZoom(plot->yAxis->orientation());
	}
	else {
		plot->axisRect()->setRangeZoomAxes({plot->xAxis, plot->yAxis});
		plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
	}
}

void Plot2DHeatMap::Replot()
{
	heat_map->rescaleDataRange(true);
	plot->rescaleAxes();
	plot->replot();
}

void Plot2DHeatMap::SetupActions()
{
	a_remove_graph = new QAction(tr("Remove graph"), this);
	connect(a_remove_graph, &QAction::triggered,
			this, &Plot2DHeatMap::RemoveGraph);

	a_interpolate_enable = new QAction(tr("Enable interpolation"), this);
	connect(a_interpolate_enable, &QAction::triggered,
			this, &Plot2DHeatMap::InterpolationEnable);

	a_interpolate_disable = new QAction(tr("Disable interpolation"), this);
	connect(a_interpolate_disable, &QAction::triggered,
			this, &Plot2DHeatMap::InterpolationDisable);

	// gradients
	Plot::MakeGradientIcons();
	for(const auto& gradient : Plot::gradients()) {
		auto action = new QAction(gradient.icon, gradient.name, this);
		action->setData(static_cast<int>(gradient.preset));
		a_gradients.push_back(action);
		connect(action, &QAction::triggered,
				this, static_cast<void(Plot2DHeatMap::*)()>(
					&Plot2DHeatMap::SetColorGradient));
	}
}

void Plot2DHeatMap::AddItemsToMenu(QMenu* menu)
{
	if(!heat_map->data()->isEmpty()) {
		auto menu_gradients = new QMenu(tr("Gradients"), menu);
		menu_gradients->addActions(a_gradients);
		menu->addMenu(menu_gradients);

		if(heat_map->interpolate()) {
			menu->addAction(a_interpolate_disable);
		} else {
			menu->addAction(a_interpolate_enable);
		}
		menu->addSeparator();

		menu->addAction(a_remove_graph);
		menu->addSeparator();
	}
}

void Plot2DHeatMap::SetColorGradient()
{
	// make sure this slot is really called by a context menu
	// action, so it carries the data we need
	if(QAction* context_action = qobject_cast<QAction*>(sender())) {
		bool ok;
		int gradient = context_action->data().toInt(&ok);
		if(ok) {
			SetColorGradient(static_cast<QCPColorGradient::GradientPreset>(
								 gradient));
		}
	}
}

QString Plot2DHeatMap::MakeTextForTracer(const QPoint& cursor_px) const
{
	double x, y;
	heat_map->pixelsToCoords(cursor_px, x, y);
	double z = heat_map->data()->data(x, y);
	return QString{"%1: %2\n%3: %4\n%5: %6"}.arg(
		GetAxisXName(), QString::number(x), GetAxisYName(),
		QString::number(y), GetTitle(), QString::number(z));
}

void Plot2DHeatMap::MousePressHandler(QMouseEvent* event)
{
	LOG(event)
	const auto& pos = event->pos();
	if(plot->plottableAt(pos) == heat_map) {
		double x, y;
		heat_map->pixelsToCoords(pos, x, y);
		double z = heat_map->data()->data(x, y);
		LOG("x:", x, "y:", y, "z:", z)
		auto text = QString{"%1 %2: %3 %4: %5 z: %6"}.
				arg(GetTitle(), GetAxisXName(), QString::number(x),
					GetAxisYName(), QString::number(y), QString::number(z));
		emit SignalPointClick(text);
	}
}

void Plot::MakeGradientIcons()
{
	static std::atomic_bool ready{false};
	if(ready) return;
	constexpr int size = 16;
	constexpr static std::array<std::array<double, size>, size> arr {{
		{{16,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}},
		{{15,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31}},
		{{14,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30}},
		{{13,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29}},
		{{12,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28}},
		{{11,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27}},
		{{10,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26}},
		{{ 9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25}},
		{{ 8,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24}},
		{{ 7, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23}},
		{{ 6, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22}},
		{{ 5, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21}},
		{{ 4, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20}},
		{{ 3, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19}},
		{{ 2, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18}},
		{{ 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16}},
	}};
	const static QCPRange range{1, 32};
	QImage img(size, size, QImage::Format::Format_ARGB32_Premultiplied);
	QCPColorGradient gradient_tmp;
	for(auto&& gradient : Plot::gradients()) {
		gradient_tmp.loadPreset(gradient.preset);
		for(int y = 0; y != size; ++y) {
			QRgb* temp_line = reinterpret_cast<QRgb*>(img.scanLine(y));
			gradient_tmp.colorize(arr[y].data(), range, temp_line, size);
		}
		gradient.icon = QIcon{QPixmap::fromImage(img)};
	}
	ready = true;
}
