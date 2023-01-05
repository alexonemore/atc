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

#ifndef RESULTMODEL_H
#define RESULTMODEL_H

#include <QAbstractTableModel>
#include "optimization.h"

enum class Variable
{
	NoVariable,
	TemperatureInitial,
	TemperatureCalculation,
	Composition
};

enum class ColNames
{
	ID,
	Formula,
	Mol,
	Gram,
	AtPct,
	WtPct
};

class ResultModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(ResultModel)
private:
	Optimization::OptimizationVector items;
	int row_count{1};
	int col_count{1};
public:
	explicit ResultModel(QObject *parent = nullptr);
	~ResultModel() override;
	void SetNewData(Optimization::OptimizationVector& vec);
	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
};

#endif // RESULTMODEL_H
