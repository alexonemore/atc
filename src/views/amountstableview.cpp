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

#include "amountstableview.h"
#include <QKeyEvent>

AmountsTableView::AmountsTableView(QWidget* parent)
{

}

AmountsTableView::~AmountsTableView()
{

}

void AmountsTableView::keyPressEvent(QKeyEvent* event)
{
	switch(event->key()) {
	case Qt::Key_Delete:
	case Qt::Key_Backspace:
		if(hasFocus()) {
			auto selected = selectionModel()->selectedIndexes();
			emit Delete(selected);
		}
		break;
	default:
		break;
	}
	QTableView::keyPressEvent(event);
}