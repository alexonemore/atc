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

#include "database.h"
#include "utilities.h"
#include <stdexcept>
#include <QtSql>

namespace SQLQueries {
const QString hsc_available_elements = QStringLiteral(
"SELECT Elements.Symbol "
"FROM Elements "
"WHERE Elements.element_id IN ("
"SELECT CompositionsOfSpecies.element_id "
"FROM CompositionsOfSpecies "
"GROUP BY CompositionsOfSpecies.element_id);");

const QString thermo_available_elements = QStringLiteral(
"SELECT elements.symbol "
"FROM elements "
"WHERE elements.element_id IN ("
"SELECT composition.element_id "
"FROM composition "
"GROUP BY composition.element_id);");


}

Database::Database(const QString& filename)
{
	sql = QSqlDatabase::addDatabase("QSQLITE", filename);
	if(!sql.open()) {
		QString str("Cannot open Database file: ");
		str += filename + "\n" + sql.lastError().text();
		LOG(str)
		throw std::runtime_error(str.toStdString().c_str());
	} else {
		LOG(filename)
	}
}

Database::~Database()
{
	sql.close();
}


/* **********************************************************************
 * *************************** Thermo ***********************************
 * ********************************************************************** */

DatabaseThermo::DatabaseThermo(const QString& filename)
	: Database(filename)
{
	sql.exec(SQLQueries::thermo_available_elements);

}

SubstancesData DatabaseThermo::GetData(const QStringList& elements)
{

	return SubstancesData{};
}


/* **********************************************************************
 * *************************** HSC **************************************
 * ********************************************************************** */

DatabaseHSC::DatabaseHSC(const QString& filename)
	: Database(filename)
{
	sql.exec(SQLQueries::hsc_available_elements);

}

SubstancesData DatabaseHSC::GetData(const QStringList& elements)
{

	return SubstancesData{};
}


