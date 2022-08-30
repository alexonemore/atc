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
	int Rows() const {
		return data.size();
	}
	int Cols() const {
		return names.size();
	}
	const QVariant& AtIndex(const QModelIndex& index) const {
		return data.at(index.row()).at(index.column());
	}
	QVector<QVariant>& Last() {
		return data.last();
	}
	void PushBackRow() {
		data.push_back(QVector<QVariant>{Cols()});
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
	void PushBackName(const QString& name) {
		names.push_back(name);
	}
};

#endif // DATATYPES_H
