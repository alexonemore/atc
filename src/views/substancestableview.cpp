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

#include "substancestableview.h"
#include <QSplitter>

SubstancesTableView::SubstancesTableView(QWidget* parent)
	: QWidget(parent)
{
	substances = new QTableView(this);
	ranges = new QTableView(this);
	tabulated_tf = new QTableView(this);
	auto splitter = new QSplitter(this);
	auto right_splitter = new QSplitter(this);
	splitter->addWidget(substances);
	splitter->addWidget(right_splitter);
	right_splitter->addWidget(ranges);
	right_splitter->addWidget(tabulated_tf);
	splitter->setChildrenCollapsible(false);
	right_splitter->setChildrenCollapsible(false);
	right_splitter->setOrientation(Qt::Vertical);
}
