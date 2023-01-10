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
#include <QColor>
#include "database.h"
#include "parameters.h"
#include "optimization.h"

namespace ResultFields {
enum class ColNames {
	ID,
	Name,
	Show
};
extern const QStringList col_names;
extern const int col_names_size;

enum class RowNames {
	T_result,
	T_initial,
	H_initial,
	H_equilibrium,
	c_equilibrium,
	Sum_mol,
	Sum_gram,
	Sum_atpct,
	Sum_wtpct
};
extern const QStringList row_names;
extern const int row_names_size;

enum class DetailRowNamesSingle {
	T_units,
	T_result,
	T_initial,
	H_initial,
	H_equilibrium,
	c_equilibrium,
	Sum_units,
	Sum_value
};
extern const QStringList detail_row_names_single;
extern const int detail_row_names_single_size;

enum class DetailRowNames1D {
	X_Axis_values,
	T_result,
	T_initial,
	H_initial,
	H_equilibrium,
	c_equilibrium,
	Sum_value
};
extern const QStringList detail_row_names_1d;
extern const int detail_row_names_1d_size;

enum class DetailRowNames2D {
	X_Axis_values_T_initial, // temperature initial
	Y_Axis_values_Composition, // composition
	T_result,
	H_initial,
	H_equilibrium,
	c_equilibrium,
	Sum_value
};
extern const QStringList detail_row_names_2d;
extern const int detail_row_names_2d_size;

} // namespace ResultFields

class ResultModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(ResultModel)
private:
	struct Cell {
		QColor color{Qt::white};
		Qt::CheckState checked{Qt::Unchecked};
	};
private:
	const SubstanceWeights* items{nullptr};
	ParametersNS::Target target{ParametersNS::Target::AdiabaticTemperature};
	std::unordered_map<int, Cell> checked; // int = just row
	int row_count{0};
public:
	explicit ResultModel(QObject *parent = nullptr);
	~ResultModel() override;
	void SetNewData(const SubstanceWeights* vec, ParametersNS::Target tar);
	void Clear();

signals:
	void AddGraph();
	void AddGraphSubstance(const int id, const QColor& color);
	void AddGraphResult(const ResultFields::RowNames row_name,
						const QColor& color);

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
private:
	bool CheckIndexValidParent(const QModelIndex& index) const;

};

class ResultDetailModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(ResultDetailModel)
private:
	const Optimization::OptimizationVector* items{nullptr};
	ParametersNS::Parameters parameters{};
	int row_count{0};
	int col_count{0};
public:
	explicit ResultDetailModel(QObject *parent = nullptr);
	~ResultDetailModel() override;
	void SetNewData(const Optimization::OptimizationVector* vec,
					const ParametersNS::Parameters params,
					const int x_size, const int y_size);
	void UpdateParameters(const ParametersNS::Parameters& params);
	void Clear();

private:
	bool CheckIndexValidParent(const QModelIndex& index) const;
	QVariant DataSingle(const QModelIndex& index, int role) const;
	QVariant Data1D(const QModelIndex& index, int role) const;
	QVariant Data2D(const QModelIndex& index, int role) const;

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

};

#endif // RESULTMODEL_H
