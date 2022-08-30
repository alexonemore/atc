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

#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include "parameters.h"

namespace SQL {
extern const QString available_elements;
extern const QString hsc_substances_template;
extern const QString thermo_substances_template;
extern const QStringList substances_field_names;
}

struct SubstanceData
{
	int id;
	QString formula, name;
	double T_min, T_max;
};
using SubstancesData = QVector<SubstanceData>;

class Database
{
protected:
	QString filename_;
	QStringList available_elements;
public:
	Database(const QString& filename);
	virtual ~Database();
	virtual SubstancesData GetSubstancesData(
			const ParametersNS::Parameters& parameters);
	const QStringList& GetAvailableElements() const {
		return available_elements;
	}
protected:
	virtual const QString& GetSubstancesDataString() const = 0;
	QSqlQuery Query(const QString& query);
	QString GetPhasesString(const ParametersNS::ShowPhases& phases);
};


class DatabaseThermo final : public Database
{
public:
	DatabaseThermo(const QString& filename)
		: Database(filename)
	{}
protected:
	const QString& GetSubstancesDataString() const override {
		return SQL::thermo_substances_template;
	}
};


class DatabaseHSC final : public Database
{
public:
	DatabaseHSC(const QString& filename)
		: Database(filename)
	{}
protected:
	const QString& GetSubstancesDataString() const override {
		return SQL::hsc_substances_template;
	}
};

#endif // DATABASE_H
