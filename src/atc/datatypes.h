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

#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QVector>
#include <QVariant>
#include <QModelIndex>

class SubstancesData
{
	QVector<QVector<QVariant>> data;
	QVector<QString> names;
public:
	SubstancesData(const int rows = 0, const int cols = 0)
	{
		names.resize(cols);
		data.resize(rows);
		for(auto&& i : data) {
			i.resize(cols);
		}
	}
	SubstancesData(const SubstancesData& rhs) = default;
	SubstancesData(SubstancesData&& rhs) = default;
	SubstancesData& operator=(const SubstancesData& rhs) = default;
	SubstancesData& operator=(SubstancesData&& rhs) = default;
	~SubstancesData() = default;

	int Rows() const {
		return data.size();
	}
	int Cols() const {
		return names.size();
	}
	const QVariant& AtIndex(const QModelIndex& index) const {
		return data.at(index.row()).at(index.column());
	}
	QVariant& At(int row, int col) {
		return data[row][col];
	}
	bool CheckIndex(const QModelIndex& index) const {
		if(index.row() >= Rows() || index.column() >= Cols()) {
			return false;
		} else {
			return true;
		}
	}
	const QString& NameAt(int i) const {
		return names.at(i);
	}
	QString& NameAt(int i) {
		return names[i];
	}
};

#endif // DATATYPES_H
