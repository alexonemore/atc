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

#include "plot2dbase.h"
#include "dialogchangetext.h"
#include <QHBoxLayout>
#include "utilities.h"

Plot2DBase::Plot2DBase(QWidget *parent)
	: QWidget{parent}
{
	auto layout = new QHBoxLayout(this);
	plot = new QCustomPlot(this);
	layout->addWidget(plot);
	layout->setContentsMargins(0, 0, 0, 0);
	title = new QCPTextElement(plot, "", QFont("sans", 14, QFont::Bold));
	plot->plotLayout()->insertRow(0);
	plot->plotLayout()->addElement(0, 0, title);
	plot->setContextMenuPolicy(Qt::CustomContextMenu);

	tracer_cross = new QCPItemTracer(plot);
	tracer_cross->setVisible(false);
	tracer_cross->position->setType(QCPItemPosition::ptAbsolute);
	tracer_cross->setStyle(QCPItemTracer::TracerStyle::tsCrosshair);

	tracer_text = new QCPItemText(plot);
	tracer_text->setVisible(false);
	tracer_text->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
	tracer_text->setTextAlignment(Qt::AlignLeft);
	tracer_text->position->setType(QCPItemPosition::ptAbsolute);
	constexpr int txtpad = 1;
	tracer_text->setPadding(QMargins{txtpad, txtpad, txtpad, txtpad});
	tracer_text->setFont(QFont("sans", 8));
	tracer_text->setBrush(QColor{Qt::white});

	plot->addLayer(QStringLiteral("tracer"));
	tracer_cross->setLayer(QStringLiteral("tracer"));
	tracer_text->setLayer(QStringLiteral("tracer"));

	connect(plot, &QCustomPlot::mouseMove,
			this, &Plot2DBase::UpdateTracerPosition);
	connect(title, &QCPTextElement::doubleClicked,
			this, &Plot2DBase::PlotTitleDoubleClick);
	connect(plot, &QCustomPlot::axisDoubleClick,
			this, &Plot2DBase::PlotAxisLabelDoubleClick);
	connect(plot, &QCustomPlot::selectionChangedByUser,
			this, &Plot2DBase::PlotSelectionChanged);
	connect(plot, &QCustomPlot::mousePress, this, &Plot2DBase::PlotMousePress);
	connect(plot, &QCustomPlot::mouseWheel, this, &Plot2DBase::PlotMouseWheel);
	connect(plot, &QCustomPlot::customContextMenuRequested,
			this, &Plot2DBase::PlotContextMenuRequest);

	menu = new QMenu(this);

	SetupActionsBase();
}

Plot2DBase::~Plot2DBase()
{

}

void Plot2DBase::HideTracer()
{
	SetTracerVisible(false);
}

void Plot2DBase::ShowTracer()
{
	SetTracerVisible(true);
}

void Plot2DBase::SetTracerVisible(const bool is_visible)
{
	show_tracer = is_visible;
	tracer_cross->setVisible(is_visible);
	tracer_text->setVisible(is_visible);
	plot->replot();
}

void Plot2DBase::SetTitle(const QString& name)
{
	title->setText(name);
}

QString Plot2DBase::GetTitle() const
{
	return title->text();
}

void Plot2DBase::SaveImage()
{
	// TODO window with more options (scale)
	QString filename = QFileDialog::getSaveFileName(
				this, tr("Choose a filename to save image"), "",
				QStringLiteral("PNG (*.png);;JPG (*.jpg);;PDF (*.pdf);;BMP (*.bmp);;All Files (*)"));
	if(!filename.isEmpty()) {
		if(filename.endsWith(QStringLiteral(".png"), Qt::CaseInsensitive)) {
			plot->savePng(filename);
		} else if(filename.endsWith(QStringLiteral(".jpg"), Qt::CaseInsensitive)) {
			plot->saveJpg(filename);
		} else if(filename.endsWith(QStringLiteral(".pdf"), Qt::CaseInsensitive)) {
			plot->savePdf(filename, 0, 0, QCP::epNoCosmetic);
		} else if(filename.endsWith(QStringLiteral(".bmp"), Qt::CaseInsensitive)) {
			plot->saveBmp(filename);
		} else {
			filename += QStringLiteral(".png");
			plot->savePng(filename);
		}
	}
}

void Plot2DBase::SaveText()
{
	// TODO dialog for delimiter
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

void Plot2DBase::PlotTitleDoubleClick(QMouseEvent*)
{
	if(QCPTextElement* title = qobject_cast<QCPTextElement*>(sender())) {
		TextParameters tp{title->text(), title->font(), title->textColor()};
		DialogChangeText dialog(tp, tr("Change title"), this);
		if (dialog.exec() == QDialog::Accepted) {
			tp = dialog.GetTextParameters();
			title->setText(tp.text);
			title->setFont(tp.font);
			title->setTextColor(tp.color);
			plot->replot();
		}
	}
}

void Plot2DBase::PlotAxisLabelDoubleClick(QCPAxis* axis,
										  QCPAxis::SelectablePart part)
{
	if(part == QCPAxis::spAxisLabel && axis) {
		TextParameters tp{axis->label(), axis->labelFont(), axis->labelColor()};
		DialogChangeText dialog(tp, tr("Change axis label"), this);
		if (dialog.exec() == QDialog::Accepted) {
			tp = dialog.GetTextParameters();
			axis->setLabel(tp.text);
			axis->setLabelFont(tp.font);
			axis->setLabelColor(tp.color);
			plot->replot();
		}
	}
}

void Plot2DBase::UpdateTracerPosition(QMouseEvent* event)
{
	if(show_tracer) {
		constexpr int pad = 2;
		int xpad = pad;
		int ypad = -pad;
		auto cursor_px = event->pos();
		auto min_px = plot->axisRect()->topLeft();
		auto max_px = plot->axisRect()->bottomRight();
		cursor_px.rx() = std::clamp(cursor_px.x(), min_px.x(), max_px.x());
		cursor_px.ry() = std::clamp(cursor_px.y(), min_px.y(), max_px.y());
		// Text is making by virtual function of the derived class
		tracer_text->setText(MakeTextForTracer(cursor_px));
		tracer_cross->position->setCoords(cursor_px);
		Qt::Alignment al{Qt::AlignLeft | Qt::AlignBottom};
		tracer_text->setPositionAlignment(al);
		tracer_text->position->setCoords(cursor_px.x()+xpad, cursor_px.y()+ypad);
		auto tr_text_px = tracer_text->topRight->pixelPosition();
		if(tr_text_px.x() > max_px.x()) {
			al ^= Qt::AlignLeft;
			al |= Qt::AlignRight;
			xpad = -pad;
		}
		if(tr_text_px.y() < min_px.y()) {
			al ^= Qt::AlignBottom;
			al |= Qt::AlignTop;
			ypad = pad;
		}
		tracer_text->setPositionAlignment(al);
		tracer_text->position->setCoords(cursor_px.x()+xpad, cursor_px.y()+ypad);
		plot->replot();
	}
}

void Plot2DBase::SetupActionsBase()
{
	a_replot = new QAction(QIcon("images/update.svg"), tr("Replot"), this);
	connect(a_replot, &QAction::triggered, this, &Plot2DBase::Replot);

	a_tracer_show = new QAction(QIcon("images/crosshair.svg"),
								tr("Show crosshair"), this);
	connect(a_tracer_show, &QAction::triggered, this, &Plot2DBase::ShowTracer);

	a_tracer_hide = new QAction(QIcon("images/crosshair.svg"),
								tr("Hide crosshair"), this);
	connect(a_tracer_hide, &QAction::triggered, this, &Plot2DBase::HideTracer);

	a_save_text = new QAction(tr("Save text..."), this);
	connect(a_save_text, &QAction::triggered, this, &Plot2DBase::SaveText);

	a_save_image = new QAction(tr("Save image..."), this);
	connect(a_save_image, &QAction::triggered, this, &Plot2DBase::SaveImage);
}

void Plot2DBase::PlotContextMenuRequest(QPoint pos)
{
	menu->clear();

	menu->addAction(a_replot);
	if(show_tracer) {
		menu->addAction(a_tracer_hide);
	} else {
		menu->addAction(a_tracer_show);
	}
	menu->addSeparator();

	AddItemsToMenu(menu);

	menu->addAction(a_save_text);
	menu->addAction(a_save_image);
	menu->popup(plot->mapToGlobal(pos));
}

