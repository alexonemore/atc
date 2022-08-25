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

#ifndef PERIODICTABLE_H
#define PERIODICTABLE_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <unordered_map>

namespace Ui {
class PeriodicTable;
}

class PeriodicTable : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(PeriodicTable)

public:
	explicit PeriodicTable(QWidget *parent = nullptr);
	~PeriodicTable() override;

private:
	Ui::PeriodicTable *ui;
	QVector<QPushButton*> buttons;
	std::unordered_map<QString, QPushButton*> buttons_map;

public:
	QStringList GetCheckedElements() const;
	void SetEnabledElements(const QStringList& elements);

};

#endif // PERIODICTABLE_H
