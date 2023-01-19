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

#include "plot3dsurface.h"
#include "ui_plot3dsurface.h"
#include "utilities.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QMetaEnum>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtDataVisualization;
#endif

Plot3DSurface::Plot3DSurface(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Plot3DSurface)
{
	ui->setupUi(this);

	plot = new Q3DSurface;
	auto plot_container = QWidget::createWindowContainer(plot, this);
	if(!plot->hasContext()) {
		QMessageBox::warning(this, tr("Plot3D error"),
							 tr("Couldn't initialize the OpenGL context. "
								"The 3D plot may not work correctly."));
	}
	auto layout_container = new QVBoxLayout;
	layout_container->setContentsMargins(0, 0, 0, 0);
	layout_container->addWidget(plot_container);
	ui->plot_widget->setLayout(layout_container);

	ui->theme->addItems({"Qt", "PrimaryColors", "Digia", "StoneMoss",
						 "ArmyBlue", "Retro", "Ebony", "Isabelle"});
	connect(ui->theme, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
			this, &Plot3DSurface::SetTheme);

	QMetaEnum presets = QMetaEnum::fromType<QGradient::Preset>();
	auto max = presets.keyCount() - 1;
	for(int i = 0; i != max; ++i) {
		auto str = presets.valueToKey(presets.value(i));
		ui->gradient->addItem(str);
	}
	connect(ui->gradient, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
			this, &Plot3DSurface::SetGradient);

	connect(ui->wireframe, &QCheckBox::clicked, this, &Plot3DSurface::ShowWireframe);
	connect(ui->clear, &QPushButton::clicked, this, &Plot3DSurface::RemoveGraph);

	ui->shadow->addItems({"None", "Low", "Medium", "High", "SoftLow",
						  "SoftMedium", "SoftHigh"});
	ui->shadow->setCurrentIndex(static_cast<int>(plot->shadowQuality()));
	connect(ui->shadow, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
			this, &Plot3DSurface::SetShadowQuality);

	connect(ui->save_image, &QPushButton::clicked, this, &Plot3DSurface::SaveImage);
	connect(ui->save_text, &QPushButton::clicked, this, &Plot3DSurface::SaveText);

	// title
//	plot_title = new QCustom3DLabel("CustomTitle",
//		QFont("sans", 14, QFont::Bold), QVector3D{1,2,0.5}, QVector3D{1,1,1},
//		QQuaternion{0,0,0,0}, this);
//	plot_title->setBorderEnabled(false);
//	plot_title->setFacingCamera(true);
//	plot->addCustomItem(plot_title);

	// setup
	plot->axisX()->setTitle("X");
	plot->axisY()->setTitle("Y");
	plot->axisZ()->setTitle("Z");
	plot->axisX()->setTitleVisible(true);
	plot->axisY()->setTitleVisible(true);
	plot->axisZ()->setTitleVisible(true);
	plot->activeTheme()->setLabelBackgroundEnabled(false);
	plot->setHorizontalAspectRatio(1.0);
//	plot->setAspectRatio(1.0);

	QSurface3DSeries *series = new QSurface3DSeries;
	series->setDrawMode(QSurface3DSeries::DrawFlag::DrawSurface);
	series->setFlatShadingEnabled(true);
	series->dataProxy()->resetArray(nullptr);
	plot->addSeries(series);
}

Plot3DSurface::~Plot3DSurface()
{
	delete ui;
}

void Plot3DSurface::SetTitle(const QString& title)
{
	ui->title->setText(title);
	//plot_title->setText(title);
}

void Plot3DSurface::SetAxisNames(const QString& x_name, const QString& y_name,
								 const QString& z_name)
{
	plot->axisX()->setTitle(x_name);
	plot->axisY()->setTitle(y_name);
	plot->axisZ()->setTitle(z_name);
}

void Plot3DSurface::SetAxisXName(const QString& x_name)
{
	plot->axisX()->setTitle(x_name);
}

void Plot3DSurface::SetAxisYName(const QString& y_name)
{
	plot->axisY()->setTitle(y_name);
}

void Plot3DSurface::SetAxisZName(const QString& z_name)
{
	plot->axisZ()->setTitle(z_name);
}

void Plot3DSurface::AddGraph(QSurfaceDataArray* data)
{
	plot->seriesList().at(0)->dataProxy()->resetArray(data);
}

void Plot3DSurface::SetTheme(int theme)
{
	LOG(theme)
	plot->activeTheme()->setType(static_cast<Q3DTheme::Theme>(theme));
	plot->activeTheme()->setLabelBackgroundEnabled(false);
}

void Plot3DSurface::SetGradient(int index)
{
	QMetaEnum presets = QMetaEnum::fromType<QGradient::Preset>();
	auto value = presets.value(index);
	LOG(index)
	LOG(value)
	LOG(presets.valueToKey(value))
	assert(value < QGradient::Preset::NumPresets);

	QGradient gradient{static_cast<QGradient::Preset>(value)};
	QLinearGradient linear_gradient;
	linear_gradient.setStops(gradient.stops());

	auto series = plot->seriesList().at(0);
	series->setBaseGradient(linear_gradient);
	series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void Plot3DSurface::SetShadowQuality(int index)
{
	plot->setShadowQuality(static_cast<QAbstract3DGraph::ShadowQuality>(index));
}

void Plot3DSurface::RemoveGraph()
{
	plot->seriesList().at(0)->dataProxy()->resetArray(nullptr);
	emit SignalGraphRemoved();
}

QString Plot3DSurface::GetTitle() const
{
	return ui->title->text();
}

Plot3DSurface::AxisNames Plot3DSurface::GetAxisNames() const
{
	return std::make_tuple(plot->axisX()->title(), plot->axisY()->title(),
					plot->axisZ()->title());
}

void Plot3DSurface::ShowWireframe(bool visible)
{
	if(visible) {
		plot->seriesList().at(0)->setDrawMode(
			QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe);
	} else {
		plot->seriesList().at(0)->setDrawMode(
			QSurface3DSeries::DrawFlag::DrawSurface);
	}
}

bool Plot3DSurface::IsEmpty() const
{
	return plot->seriesList().at(0)->dataProxy()->array()->isEmpty();
}

void Plot3DSurface::SaveImage()
{
	// TODO custom menu
	QString filename = QFileDialog::getSaveFileName(this,
		tr("Choose a filename to save image"), "",
		QStringLiteral("PNG (*.png);;JPG (*.jpg);;JPEG (*.jpeg);;BMP (*.bmp)"));
	if(!filename.isEmpty()) {
		LOG(filename)
		if(!(filename.endsWith(".png") || filename.endsWith(".PNG") ||
		   filename.endsWith(".jpg") || filename.endsWith(".JPG") ||
		   filename.endsWith(".jpeg") || filename.endsWith(".JPEG") ||
		   filename.endsWith(".bmp") || filename.endsWith(".BMP"))) {
			filename.append(".png");
		}
		LOG(filename)
		auto image = plot->renderToImage(16, {3840, 2160});
		auto ok = image.save(filename);
		if(!ok) {
			QMessageBox::critical(this, tr("Error"), tr("Image not saved:\n") +
								  filename);
		}
	}
}

void Plot3DSurface::SaveText()
{
	// TODO dialog for delimiter
	if(IsEmpty()) return;
	QString fileName = QFileDialog::getSaveFileName(
				this, tr("Choose a filename to save data"), "",
				QStringLiteral("TXT (*.txt);;CSV (*.csv);;All Files (*)"));
	if(!fileName.isEmpty()) {
		QFile file(fileName);
		if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			QTextStream stream(&file);
			QString delimiter = ";";
			PrintGraphsToTextFile(stream, delimiter);
		}
		file.close();
	}
}

void Plot3DSurface::PrintGraphsToTextFile(QTextStream& stream,
										  QString delimiter) const
{
	auto arr = plot->seriesList().at(0)->dataProxy()->array();
	auto [x, y, z] = GetAxisNames();
	stream << GetTitle() << "\n";
	stream << tr("Values: ") << y << "\n";
	stream << z << "\\" << x << delimiter;
	for(auto&& i : *arr->first()) {
		stream << i.x() << delimiter;
	}
	stream << "\n";
	for(auto&& row : *arr) {
		stream << row->first().z() << delimiter;
		for(auto&& item : *row) {
			stream << item.y() << delimiter;
		}
		stream << "\n";
	}
}
