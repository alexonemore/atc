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
#include "datatypes.h"

namespace SQL {
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
	virtual SubstancesData GetData(const ParametersNS::Parameters& parameters) = 0;
	const QStringList& GetAvailableElements() const {
		return available_elements;
	}
protected:
	QSqlQuery Query(const QString& query);
	QString GetPhasesString(const ParametersNS::ShowPhases& phases);
};


class DatabaseThermo final : public Database
{
public:
	DatabaseThermo(const QString& filename);
	SubstancesData GetData(const ParametersNS::Parameters& parameters) override;

};


class DatabaseHSC final : public Database
{
public:
	DatabaseHSC(const QString& filename);
	SubstancesData GetData(const ParametersNS::Parameters& parameters) override;

};

#endif // DATABASE_H
