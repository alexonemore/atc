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

#include <QVector>
#include <QVariant>
#include <QModelIndex>

class SubstancesData : public QVector<QVector<QVariant>>
{
public:
	int Rows() const {
		return size();
	}
	int Cols() const {
		return size() < 1 ? 0 : at(0).size();
	}
	QVariant AtIndex(const QModelIndex& index) const {
		return at(index.row()).at(index.column());
	}
	bool CheckIndex(const QModelIndex& index) const {
		if(index.row() >= Rows() || index.column() >= Cols()) {
			return false;
		} else {
			return true;
		}
	}
};

#endif // DATATYPES_H
