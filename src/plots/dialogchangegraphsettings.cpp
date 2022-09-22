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

#include "dialogchangegraphsettings.h"
#include "ui_dialogchangegraphsettings.h"
#include "utilities.h"

DialogChangeGraphSettings::DialogChangeGraphSettings(
		const GraphSettings previous, const QString name, QWidget *parent)
	: QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint |
			  Qt::WindowCloseButtonHint)
	, ui(new Ui::DialogChangeGraphSettings)
{
	ui->setupUi(this);
	setWindowTitle(name);
	ui->plot->SetTitle(tr("Example"));
	ui->plot->SetAxisXName("");
	ui->plot->SetAxisY1Name("");
	ui->plot->SetAxisY2Name("");
	ui->plot->SetLegendVisible(true);

	const int size = 101;
	QVector<double> x(size);
	QVector<double> y(size);
	std::iota(x.begin(), x.end(), 0.0);
	std::transform(x.constBegin(), x.constEnd(), y.begin(), [](auto i){
		return std::sin(i/10)/i;
	});
	ui->plot->AddGraphY1(GraphId{}, previous.name, std::move(x), std::move(y));
	ui->plot->SetGraphSettings(GraphId{}, previous);

	ui->name->setText(previous.name);

	ui->line_graph_style->addItems({"None", "Line", "StepLeft", "StepRight",
									"StepCenter", "Impulse"});
	ui->line_graph_style->setCurrentIndex(static_cast<int>(previous.line_style));

	ui->line_style->addItems({"NoPen", "SolidLine", "DashLine", "DotLine",
							  "DashDotLine", "DashDotDotLine"});
	ui->line_style->setCurrentIndex(static_cast<int>(previous.line_pen.style()));

	ui->line_width->setValue(previous.line_pen.widthF());

	ui->scatter_shape->addItems({"None", "Dot", "Cross", "Plus", "Circle", "Disc",
								 "Square", "Diamond", "Star", "Triangle",
								 "TriangleInverted", "CrossSquare", "PlusSquare",
								 "CrossCircle", "PlusCircle", "Peace"});
	ui->scatter_shape->setCurrentIndex(static_cast<int>(previous.scatter_style.shape()));

	ui->scatter_size->setValue(previous.scatter_style.pen().widthF());

	connect(ui->name, &QLineEdit::textChanged,
			this, &DialogChangeGraphSettings::ChangeName);
	connect(ui->line_graph_style, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
			this, &DialogChangeGraphSettings::ChangeLineGraphStyle);
	connect(ui->line_style, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
			this, &DialogChangeGraphSettings::ChangeLineStyle);
	connect(ui->line_width, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
			this, &DialogChangeGraphSettings::ChangeLineWidth);
	connect(ui->line_color, &QPushButton::clicked,
			this, &DialogChangeGraphSettings::ChangeLineColor);
	connect(ui->scatter_shape, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
			this, &DialogChangeGraphSettings::ChangeScatterShape);
	connect(ui->scatter_size, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
			this, &DialogChangeGraphSettings::ChangeScatterSize);
	connect(ui->scatter_line_color, &QPushButton::clicked,
			this, &DialogChangeGraphSettings::ChangeScatterLineColor);
	connect(ui->scatter_fill_color, &QPushButton::clicked,
			this, &DialogChangeGraphSettings::ChangeScatterFillColor);
	connect(ui->scatter_line_width, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
			this, &DialogChangeGraphSettings::ChangeScatterLineWidth);
}

DialogChangeGraphSettings::~DialogChangeGraphSettings()
{
	LOG()
	delete ui;
}

GraphSettings DialogChangeGraphSettings::GetGraphSettings() const
{
	LOG()
	return ui->plot->GetGraphSettings(GraphId{});
}

void DialogChangeGraphSettings::ChangeName(const QString& name)
{
	LOG()
	ui->plot->SetGraphName(GraphId{}, name);
}

void DialogChangeGraphSettings::ChangeLineGraphStyle(int index)
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	settings.line_style = static_cast<QCPGraph::LineStyle>(index);
	ui->plot->SetGraphSettings(GraphId{}, settings);
}

void DialogChangeGraphSettings::ChangeLineStyle(int index)
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	settings.line_pen.setStyle(static_cast<Qt::PenStyle>(index));
	ui->plot->SetGraphSettings(GraphId{}, settings);
}

void DialogChangeGraphSettings::ChangeLineWidth(double width)
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	settings.line_pen.setWidthF(width);
	ui->plot->SetGraphSettings(GraphId{}, settings);
}

void DialogChangeGraphSettings::ChangeLineColor()
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	const QColor new_color = QColorDialog::getColor(settings.line_pen.color(),
									this, tr("Select Line Color"));
	if (new_color.isValid()) {
		settings.line_pen.setColor(new_color);
		ui->plot->SetGraphSettings(GraphId{}, settings);
	}
}

void DialogChangeGraphSettings::ChangeScatterShape(int index)
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	settings.scatter_style.setShape(static_cast<QCPScatterStyle::ScatterShape>(index));
	ui->plot->SetGraphSettings(GraphId{}, settings);
}

void DialogChangeGraphSettings::ChangeScatterSize(double width)
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	settings.scatter_style.setSize(width);
	ui->plot->SetGraphSettings(GraphId{}, settings);
}

void DialogChangeGraphSettings::ChangeScatterLineWidth(double width)
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	auto color = settings.scatter_style.pen().color();
	QPen pen{color};
	pen.setWidthF(width);
	settings.scatter_style.setPen(pen);
	ui->plot->SetGraphSettings(GraphId{}, settings);
}

void DialogChangeGraphSettings::ChangeScatterLineColor()
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	auto prev_color = settings.scatter_style.pen().color();
	const QColor new_color = QColorDialog::getColor(prev_color, this,
												tr("Select Scatter Color"));
	if (new_color.isValid()) {
		auto width = settings.scatter_style.pen().widthF();
		QPen pen{new_color};
		pen.setWidthF(width);
		settings.scatter_style.setPen(pen);
		ui->plot->SetGraphSettings(GraphId{}, settings);
	}
}

void DialogChangeGraphSettings::ChangeScatterFillColor()
{
	LOG()
	auto settings = ui->plot->GetGraphSettings(GraphId{});
	auto prev_color = settings.scatter_style.brush().color();
	const QColor new_color = QColorDialog::getColor(prev_color, this,
												tr("Select Scatter Color"));
	if (new_color.isValid()) {
		settings.scatter_style.setBrush(QBrush{new_color});
		ui->plot->SetGraphSettings(GraphId{}, settings);
	}
}

