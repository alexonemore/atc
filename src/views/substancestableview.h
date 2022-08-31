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

#ifndef SUBSTANCESTABLEVIEW_H
#define SUBSTANCESTABLEVIEW_H

#include <QWidget>
#include <QTableView>

class SubstancesTableView : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(SubstancesTableView)
private:
	QTableView* substances;
	QTableView* ranges;
	QTableView* tabulated_tf;
public:
	SubstancesTableView(QWidget* parent = nullptr);
	void SetSubstancesTableModel(QAbstractItemModel* model) {
		substances->setModel(model);
	}
	void SetSubstancesRangeModel(QAbstractItemModel* model) {
		ranges->setModel(model);
	}
	void SetSubstancesTabulatedModel(QAbstractItemModel* model) {
		tabulated_tf->setModel(model);
	}
};

#endif // SUBSTANCESTABLEVIEW_H
