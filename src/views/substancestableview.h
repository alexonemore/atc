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

#ifndef SUBSTANCESTABLEVIEW_H
#define SUBSTANCESTABLEVIEW_H

#include <QWidget>
#include <QLabel>
#include "table.h"

class SubstancesTableView : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(SubstancesTableView)
private:
	Table* substances;
	Table* ranges;
	Table* tabulated_tf;
	QLabel* label;
public:
	SubstancesTableView(QWidget* parent = nullptr);
	~SubstancesTableView() override;
	void Initialize();
	void SetSubstancesTableModel(QAbstractItemModel* model);
	void SetSubstancesTempRangeModel(QAbstractItemModel* model);
	void SetSubstancesTabulatedModel(QAbstractItemModel* model);
	void SetSelectedSubstanceLabel(const QString& name);
signals:
	void SelectSubstance(int id);
	void ClearSelection();
private slots:
	void SelectionChanged(const QItemSelection &selected,
						  const QItemSelection &deselected);
};

#endif // SUBSTANCESTABLEVIEW_H
