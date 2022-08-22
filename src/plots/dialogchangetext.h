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

#ifndef DIALOGCHANGETEXT_H
#define DIALOGCHANGETEXT_H

#include <QDialog>
#include <QString>
#include <QFont>
#include <QColor>

struct TextParameters {
	QString	text;
	QFont	font;
	QColor	color;
};

namespace Ui {
class DialogChangeText;
}

class DialogChangeText : public QDialog
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(DialogChangeText)

public:
	explicit DialogChangeText(const TextParameters previous, const QString name,
							  QWidget *parent = nullptr);
	~DialogChangeText() override;
	TextParameters GetTextParameters() const;

private:
	Ui::DialogChangeText *ui;

private slots:
	void ButtonFontClick();
	void ButtonColorClick();
};

#endif // DIALOGCHANGETEXT_H
