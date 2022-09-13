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

#include "table.h"
#include <QMenu>
#include "utilities.h"

Table::Table(QWidget* parent)
	: QTableView(parent)
{
	context_menu = new QMenu(this);
	auto copy_action = new QAction(QIcon("images/copy.svg"),
								   tr("Copy"), context_menu);
	auto copy_with_headers_action = new QAction(QIcon("images/copy.svg"),
									tr("Copy with Headers"), context_menu);
	auto copy_as_SQL_action = new QAction(QIcon("images/copy.svg"),
									tr("Copy as SQL"), context_menu);
	auto print_action = new QAction(QIcon("images/ok.svg"),
									tr("Print..."), context_menu);
	context_menu->addAction(copy_action);
	context_menu->addAction(copy_with_headers_action);
	context_menu->addAction(copy_as_SQL_action);
	context_menu->addAction(print_action);
	setContextMenuPolicy(Qt::CustomContextMenu);

	// This is only for displaying the shortcut in the context menu.
	// An entry in keyPressEvent is still needed.
	copy_action->setShortcut(QKeySequence::Copy);
	copy_with_headers_action->setShortcut(QKeySequence(tr("Ctrl+Shift+C")));
	copy_as_SQL_action->setShortcut(QKeySequence(tr("Ctrl+Alt+C")));
	print_action->setShortcut(QKeySequence::Print);

	// Set up context menu actions
	connect(this, &QTableView::customContextMenuRequested, this,
			[this](const QPoint& pos) {
		context_menu->popup(viewport()->mapToGlobal(pos));
	});


}
