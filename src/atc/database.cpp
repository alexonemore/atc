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

#include "database.h"
#include "utilities.h"
#include <stdexcept>
#include <QtSql>

namespace SQL {
const QString available_elements = QStringLiteral(
"SELECT Elements.Symbol "
"FROM Elements "
"WHERE Elements.element_id IN ("
"SELECT DISTINCT CompositionsOfSpecies.element_id "
"FROM CompositionsOfSpecies);");

const QString available_elements_for_spesies = QStringLiteral(
"SELECT CompositionsOfSpecies.element_id "
"FROM CompositionsOfSpecies "
"WHERE CompositionsOfSpecies.species_id IN (%1) "
"GROUP BY CompositionsOfSpecies.element_id;");

const QString substances_element_composition = QStringLiteral(
"SELECT CompositionsOfSpecies.species_id, "
"CompositionsOfSpecies.element_id, "
"CompositionsOfSpecies.Amount "
"FROM CompositionsOfSpecies "
"WHERE CompositionsOfSpecies.species_id IN (%1);");

const QString hsc_substances_template = QStringLiteral(
"SELECT Species.species_id AS 'ID', Species.Formula AS 'Formula', "
"Species.Weight AS Weight, "
"IIF(length(Species.NameCh)>0, Species.NameCh, '') || "
"IIF(length(Species.NameCh)>0 AND length(Species.NameCo)>0, ' (', '') || "
"IIF(length(Species.NameCo)>0, Species.NameCo, '') || "
"IIF(length(Species.NameCh)>0 AND length(Species.NameCo)>0, ')', '') "
"AS 'Name', "
"Species.T_min AS 'T min', Species.T_max AS 'T max' "
"FROM ( "
"SELECT CompositionsOfSpecies.species_id FROM CompositionsOfSpecies "
"WHERE CompositionsOfSpecies.element_id IN ( "
"SELECT Elements.element_id FROM Elements WHERE Elements.Symbol IN (%1)) "
"EXCEPT "
"SELECT CompositionsOfSpecies.species_id FROM CompositionsOfSpecies "
"WHERE CompositionsOfSpecies.element_id NOT IN ( "
"SELECT Elements.element_id FROM Elements WHERE Elements.Symbol IN (%1)) "
") AS T "
"JOIN Species ON Species.species_id = T.species_id "
"WHERE Species.Suffix IN (%2);");

const QString thermo_substances_template = QStringLiteral(
"SELECT Species.species_id AS 'ID', "
"Species.Formula || '(' || State.Symbol || ')' AS 'Formula', "
"Species.Weight AS Weight, "
"Species.Name AS 'Name', "
"Species.T_min AS 'T min', "
"Species.T_max AS 'T max' "
"FROM ( "
"SELECT CompositionsOfSpecies.species_id FROM CompositionsOfSpecies "
"WHERE CompositionsOfSpecies.element_id IN ( "
"SELECT Elements.element_id FROM Elements WHERE Elements.Symbol IN (%1)) "
"EXCEPT "
"SELECT CompositionsOfSpecies.species_id FROM CompositionsOfSpecies "
"WHERE CompositionsOfSpecies.element_id NOT IN ( "
"SELECT Elements.element_id FROM Elements WHERE Elements.Symbol IN (%1)) "
") AS T "
"JOIN Species ON Species.species_id = T.species_id "
"JOIN State ON State.state_id = Species.state_id "
"WHERE State.Symbol IN (%2);");

const QString hsc_substance_temprange_template = QStringLiteral(
"SELECT TempRange.T1 AS 'T min', TempRange.T2 AS 'T max', "
"TempRange.H AS 'H', "
"TempRange.S AS 'S', "
"TempRange.A AS 'f1', "
"TempRange.B AS 'f2', "
"TempRange.C AS 'f3', "
"TempRange.D AS 'f4', "
"TempRange.E AS 'f5', "
"TempRange.F AS 'f6', "
"0 AS 'f7', "
"TempRange.Phase AS 'Phase' "
"FROM TempRange "
"WHERE TempRange.species_id = %1;");

const QString hsc_substances_temprange_template = QStringLiteral(
"SELECT TempRange.species_id AS 'ID', "
"TempRange.T1 AS 'T min', TempRange.T2 AS 'T max', "
"TempRange.H AS 'H', "
"TempRange.S AS 'S', "
"TempRange.A AS 'f1', "
"TempRange.B AS 'f2', "
"TempRange.C AS 'f3', "
"TempRange.D AS 'f4', "
"TempRange.E AS 'f5', "
"TempRange.F AS 'f6', "
"0 AS 'f7', "
"TempRange.Phase AS 'Phase' "
"FROM TempRange "
"WHERE TempRange.species_id IN (%1);");

const QString thermo_substance_temprange_template = QStringLiteral(
"SELECT TempRange.T_min AS 'T min', TempRange.T_max AS 'T max', "
"Species.H0 AS 'H', 0 AS 'S', "
"TempRange.f1 AS 'f1', "
"TempRange.f2 AS 'f2', "
"TempRange.f3 AS 'f3', "
"TempRange.f4 AS 'f4', "
"TempRange.f5 AS 'f5', "
"TempRange.f6 AS 'f6', "
"TempRange.f7 AS 'f7', "
"State.Symbol AS 'Phase' "
"FROM TempRange "
"JOIN Species ON Species.species_id = TempRange.species_id "
"JOIN State ON State.state_id = Species.state_id "
"WHERE TempRange.species_id = %1;");

const QString thermo_substances_temprange_template = QStringLiteral(
"SELECT TempRange.species_id AS 'ID', "
"TempRange.T_min AS 'T min', TempRange.T_max AS 'T max', "
"Species.H0 AS 'H', 0 AS 'S', "
"TempRange.f1 AS 'f1', "
"TempRange.f2 AS 'f2', "
"TempRange.f3 AS 'f3', "
"TempRange.f4 AS 'f4', "
"TempRange.f5 AS 'f5', "
"TempRange.f6 AS 'f6', "
"TempRange.f7 AS 'f7', "
"State.Symbol AS 'Phase' "
"FROM TempRange "
"JOIN Species ON Species.species_id = TempRange.species_id "
"JOIN State ON State.state_id = Species.state_id "
"WHERE TempRange.species_id IN (%1);");

const QString hsc_substance_name_template = QStringLiteral(
"SELECT "
"Species.Formula || ' ' || "
"IIF(length(Species.NameCh)>0, Species.NameCh, '') || "
"IIF(length(Species.NameCh)>0 AND length(Species.NameCo)>0, ' (', '') || "
"IIF(length(Species.NameCo)>0, Species.NameCo, '') || "
"IIF(length(Species.NameCh)>0 AND length(Species.NameCo)>0, ')', '') "
"AS 'Name' "
"FROM Species "
"WHERE Species.species_id = %1;");

const QString thermo_substance_name_template = QStringLiteral(
"SELECT "
"Species.Formula || '(' || State.Symbol || ') ' || Species.Name AS 'Name' "
"FROM Species "
"JOIN State ON State.state_id = Species.state_id "
"WHERE Species.species_id = %1;");

QSqlQuery Query(const QString& query, const QString& name)
{
	LOG()
	QSqlQuery q(query, QSqlDatabase::database(name));
	if(q.lastError().isValid()) {
		auto str = q.lastError().text();
		LOG(str)
		LOG(query)
		throw std::runtime_error(str.toStdString().c_str());
	}
	return q;
}

} // SQL

Database::Database(const QString& filename)
{
	database_name = filename;
	auto sql = QSqlDatabase::addDatabase("QSQLITE", database_name);
	sql.setDatabaseName(database_name);
	if(!sql.open()) {
		QString str("Cannot open Database file: ");
		str += database_name + "\n" + sql.lastError().text();
		LOG(str)
		throw std::runtime_error(str.toStdString().c_str());
	} else {
		LOG(database_name)
	}
	auto q = SQL::Query(SQL::available_elements, database_name);
	while(q.next()) {
		available_elements.push_back(q.value(0).toString());
	}
	LOG(available_elements)
}

Database::~Database()
{
	LOG()
}

SubstancesData Database::GetSubstancesData(
		const ParametersNS::Parameters& parameters)
{
	auto elements_str = QStringLiteral("'") +
			parameters.checked_elements.join("','") + QStringLiteral("'");
	auto phases = GetPhasesString(parameters.show_phases);
	LOG(phases)
	auto q = SQL::Query(GetSubstancesDataString().arg(elements_str, phases),
						database_name);
	SubstancesData data;
	while(q.next()) {
		data.push_back(SubstanceData{{{q.value(0).toInt(),		// ID
									 q.value(1).toString()},	// Formula
									 q.value(2).toDouble()},	// Weight
									 q.value(3).toString(),		// Name
									 q.value(4).toDouble(),		// T_min
									 q.value(5).toDouble()});	// T_max
	}
	return data;
}

SubstanceTempRangeData Database::GetSubstanceTempRangeData(const int id)
{
	LOG(id)
	auto q = SQL::Query(GetSubstanceTempRangeDataString().arg(id),
						database_name);
	SubstanceTempRangeData data;
	while(q.next()) {
		data.push_back(TempRangeData{q.value(0).toDouble(), // T_min
									 q.value(1).toDouble(), // T_max
									 q.value(2).toDouble(), // H
									 q.value(3).toDouble(), // S
									 q.value(4).toDouble(), // f1
									 q.value(5).toDouble(), // f2
									 q.value(6).toDouble(), // f3
									 q.value(7).toDouble(), // f4
									 q.value(8).toDouble(), // f5
									 q.value(9).toDouble(), // f6
									 q.value(10).toDouble(), // f7
								 q.value(11).toString().toUpper()}); // phase
	}
	return data;
}

SubstancesTempRangeData Database::GetSubstancesTempRangeData(const QString& ids)
{
	auto q = SQL::Query(GetSubstancesTempRangeDataString().arg(ids),
						database_name);
	SubstancesTempRangeData tr;
	while(q.next()) {
		int id = q.value(0).toInt();
		tr[id].push_back(TempRangeData{q.value(1).toDouble(), // T_min
									   q.value(2).toDouble(), // T_max
									   q.value(3).toDouble(), // H
									   q.value(4).toDouble(), // S
									   q.value(5).toDouble(), // f1
									   q.value(6).toDouble(), // f2
									   q.value(7).toDouble(), // f3
									   q.value(8).toDouble(), // f4
									   q.value(9).toDouble(), // f5
									   q.value(10).toDouble(), // f6
									   q.value(11).toDouble(), // f7
								   q.value(12).toString().toUpper()}); // phase
	}
	return tr;
}

std::vector<int> Database::GetAvailableElements(const QString& ids)
{
	std::vector<int> elements;
	auto q = SQL::Query(SQL::available_elements_for_spesies.arg(ids),
						database_name);
	while(q.next()) {
		elements.push_back(q.value(0).toInt());
	}
	return elements;
}

SubstancesElementComposition Database::GetSubstancesElementComposition(
		const QString& ids)
{
	SubstancesElementComposition cmp;
	auto q = SQL::Query(SQL::substances_element_composition.arg(ids),
						database_name);
	while(q.next()) {
		auto sub_id = q.value(0).toInt();
		auto el_id = q.value(1).toInt();
		cmp[sub_id][el_id] = q.value(2).toDouble();
	}
	return cmp;
}

QString Database::GetSubstanceName(const int id)
{
	LOG(id)
	auto q = SQL::Query(GetSubstanceNameString().arg(id), database_name);
	q.first();
	return q.value(0).toString();
}

QString Database::GetPhasesString(const ParametersNS::ShowPhases& phases)
{
	QStringList list;
	if(phases.gas) {
		list.append(QStringLiteral("g"));
	}
	if(phases.liquid) {
		list.append(QStringLiteral("l"));
	}
	if(phases.solid) {
		list.append(QStringLiteral("s"));
		list.append(QLatin1String(""));
	}
	if(list.isEmpty()) {
		return QString{};
	} else {
		return QStringLiteral("'") + list.join("','") + QStringLiteral("'");
	}
}

