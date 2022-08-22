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

#include "dialogchangetext.h"
#include "ui_dialogchangetext.h"
#include <QFontDialog>
#include <QColorDialog>

DialogChangeText::DialogChangeText(const TextParameters previous,
								   const QString name,
								   QWidget *parent)
	: QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint |
			  Qt::WindowCloseButtonHint)
	, ui(new Ui::DialogChangeText)
{
	ui->setupUi(this);
	setWindowTitle(name);
	ui->text->setPlainText(previous.text);
	ui->text->setFont(previous.font);
	QPalette p = ui->text->palette();
	p.setColor(QPalette::Text, previous.color);
	ui->text->setPalette(p);

	connect(ui->button_font, &QPushButton::clicked,
			this, &DialogChangeText::ButtonFontClick);
	connect(ui->button_color, &QPushButton::clicked,
			this, &DialogChangeText::ButtonColorClick);
}

DialogChangeText::~DialogChangeText()
{
	delete ui;
}

TextParameters DialogChangeText::GetTextParameters() const
{
	return TextParameters{ui->text->toPlainText(),
						  ui->text->font(),
						  ui->text->palette().color(QPalette::Text)};
}

void DialogChangeText::ButtonFontClick()
{
	bool ok;
	const QFont font = QFontDialog::getFont(&ok, ui->text->font(), this,
											tr("Select Font"));
	if (ok) {
		ui->text->setFont(font);
	}
}

void DialogChangeText::ButtonColorClick()
{
	QPalette p = ui->text->palette();
	const QColor color = QColorDialog::getColor(p.color(QPalette::Text), this,
												tr("Select Color"));
	if (color.isValid()) {
		p.setColor(QPalette::Text, color);
		ui->text->setPalette(p);
	}
}
