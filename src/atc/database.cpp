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
"SELECT DISTINCT CompositionsOfSpecies.element_id "
"FROM CompositionsOfSpecies);");

const QString thermo_available_elements = QStringLiteral(
"SELECT elements.symbol "
"FROM elements "
"WHERE elements.element_id IN ("
"SELECT DISTINCT composition.element_id "
"FROM composition);");

const QString hsc_substances = QStringLiteral(
"SELECT Species.Formula, "
"IIF(length(Species.NameCh)>0, Species.NameCh, '') ||"
"IIF(length(Species.NameCh)>0 AND length(Species.NameCo)>0, ' (', '') ||"
"IIF(length(Species.NameCo)>0, Species.NameCo, '') ||"
"IIF(length(Species.NameCh)>0 AND length(Species.NameCo)>0, ')', '')"
"AS 'Name',"
"Species.T_min AS 'T min', Species.T_max AS 'T max'"
"FROM ("
"	SELECT CompositionsOfSpecies.species_id FROM CompositionsOfSpecies"
"	WHERE CompositionsOfSpecies.element_id IN ("
"		SELECT Elements.element_id FROM Elements WHERE Elements.Symbol IN (%1))"
"	EXCEPT"
"	SELECT CompositionsOfSpecies.species_id FROM CompositionsOfSpecies"
"	WHERE CompositionsOfSpecies.element_id NOT IN ("
"		SELECT Elements.element_id FROM Elements WHERE Elements.Symbol IN (%1))"
") AS T"
"JOIN Species ON Species.species_id = T.species_id"
"WHERE Species.Suffix IN (%2)"
"");

const QString thermo_substances = QStringLiteral(""
"SELECT "
"substances.sub_id AS id, "
"substances.name || ' [' || state.symbol || '] ' || substances.alt_name "
"AS 'Name', "
"state.name AS State, "
"substances.T_min, "
"substances.T_max, "
"substances.weight AS Weight "
"FROM substances "
"JOIN state ON state.state_id = substances.state_id "
"WHERE substances.sub_id IN "
"(SELECT composition.sub_id FROM composition WHERE "
"composition.element_id IN (%1) "
"EXCEPT "
"SELECT composition.sub_id FROM composition WHERE "
"composition.element_id NOT IN (%1)) "
"ORDER BY substances.sub_id;");

}

Database::Database(const QString& filename)
{
	filename_ = filename;
	auto sql = QSqlDatabase::addDatabase("QSQLITE", filename_);
	sql.setDatabaseName(filename_);
	if(!sql.open()) {
		QString str("Cannot open Database file: ");
		str += filename_ + "\n" + sql.lastError().text();
		LOG(str)
		throw std::runtime_error(str.toStdString().c_str());
	} else {
		LOG(filename_)
	}
}

Database::~Database()
{
}

QSqlQuery Database::Query(const QString& query)
{
	LOG()
	return QSqlQuery(query, QSqlDatabase::database(filename_));
}


/* **********************************************************************
 * *************************** Thermo ***********************************
 * ********************************************************************** */

DatabaseThermo::DatabaseThermo(const QString& filename)
	: Database(filename)
{
	auto q = Query(SQLQueries::thermo_available_elements);
	while(q.next()) {
		available_elements.push_back(q.value(0).toString());
	}
	LOG(available_elements)
}

SubstancesData DatabaseThermo::GetData(const ParametersNS::Parameters parameters)
{
	auto elements_str = QStringLiteral("'") +
			parameters.checked_elements.join("','") + QStringLiteral("'");
	auto q = Query(SQLQueries::thermo_substances.arg(elements_str));

	while(q.next()) {



	}

	return SubstancesData{};
}


/* **********************************************************************
 * *************************** HSC **************************************
 * ********************************************************************** */

DatabaseHSC::DatabaseHSC(const QString& filename)
	: Database(filename)
{
	auto q = Query(SQLQueries::hsc_available_elements);
	while(q.next()) {
		available_elements.push_back(q.value(0).toString());
	}
	LOG(available_elements)
}

SubstancesData DatabaseHSC::GetData(const ParametersNS::Parameters parameters)
{
	auto elements_str = QStringLiteral("'") +
			parameters.checked_elements.join("','") + QStringLiteral("'");
	auto phases = GetPhasesString(parameters);
	LOG(phases)
	auto q = Query(SQLQueries::hsc_substances.arg(elements_str, phases));

	while(q.next()) {



	}

	return SubstancesData{};
}

QString DatabaseHSC::GetPhasesString(const ParametersNS::Parameters parameters)
{
	QStringList list;
	if(parameters.show_phases.gas) {
		list.append(QStringLiteral("g"));
	}
	if(parameters.show_phases.liquid) {
		list.append(QStringLiteral("l"));
	}
	if(parameters.show_phases.solid) {
		list.append(QStringLiteral("s"));
		list.append(QStringList(""));
	}
	if(list.isEmpty()) {
		return QString{};
	} else {
		return QStringLiteral("'") + list.join("','") + QStringLiteral("'");
	}
}

