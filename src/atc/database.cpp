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

namespace Models {
const QStringList substances_field_names = {
	QStringLiteral("ID"),
	QStringLiteral("Formula"),
	QStringLiteral("Name"),
	QStringLiteral("T min"),
	QStringLiteral("T max")
};
const QStringList substances_temprange_field_names = {
	QStringLiteral("T min"),
	QStringLiteral("T max"),
	QStringLiteral("H"),
	QStringLiteral("S"),
	QStringLiteral("f1"),
	QStringLiteral("f2"),
	QStringLiteral("f3"),
	QStringLiteral("f4"),
	QStringLiteral("f5"),
	QStringLiteral("f6"),
	QStringLiteral("f7"),
	QStringLiteral("Phase")
};
const QStringList substance_tabulated_tf_field_names = {
	QStringLiteral("T [%1]"),
	QStringLiteral("G [kJ/mol]"),
	QStringLiteral("H [kJ/mol]"),
	QStringLiteral("F [J/molK]"),
	QStringLiteral("S [J/molK]"),
	QStringLiteral("Cp [J/molK]"),
	QStringLiteral("c [G/RT]")
};
extern const QStringList phases_names = {
	QStringLiteral("G"),
	QStringLiteral("L"),
	QStringLiteral("S"),
	QStringLiteral("Error")
};
} // Models

namespace SQL {
const QString available_elements = QStringLiteral(
"SELECT Elements.Symbol "
"FROM Elements "
"WHERE Elements.element_id IN ("
"SELECT DISTINCT CompositionsOfSpecies.element_id "
"FROM CompositionsOfSpecies);");

const QString hsc_substances_template = QStringLiteral(
"SELECT Species.species_id AS 'ID', Species.Formula AS 'Formula', "
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

const QString hsc_substances_temprange_template = QStringLiteral(
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

const QString thermo_substances_temprange_template = QStringLiteral(
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
	name = filename;
	auto sql = QSqlDatabase::addDatabase("QSQLITE", name);
	sql.setDatabaseName(name);
	if(!sql.open()) {
		QString str("Cannot open Database file: ");
		str += name + "\n" + sql.lastError().text();
		LOG(str)
		throw std::runtime_error(str.toStdString().c_str());
	} else {
		LOG(name)
	}
	auto q = SQL::Query(SQL::available_elements, name);
	while(q.next()) {
		available_elements.push_back(q.value(0).toString());
	}
	LOG(available_elements)
}

Database::~Database()
{
}

SubstancesData Database::GetSubstancesData(
		const ParametersNS::Parameters& parameters)
{
	auto elements_str = QStringLiteral("'") +
			parameters.checked_elements.join("','") + QStringLiteral("'");
	auto phases = GetPhasesString(parameters.show_phases);
	LOG(phases)
	auto q = SQL::Query(GetSubstancesDataString().arg(elements_str, phases),
						name);
	SubstancesData data;
	while(q.next()) {
		data.push_back(SubstanceData{q.value(0).toInt(),		// ID
									 q.value(1).toString(),		// Formula
									 q.value(2).toString(),		// Name
									 q.value(3).toDouble(),		// T_min
									 q.value(4).toDouble()});	// T_max
	}
	return data;
}

SubstanceTempRangeData Database::GetSubstancesTempRangeData(const int id)
{
	LOG(id)
	auto q = SQL::Query(GetSubstancesTempRangeDataString().arg(id), name);
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
								  ToPhase(q.value(11).toString())}); // phase
	}
	return data;
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

Models::Phase Database::ToPhase(const QString& phase)
{
	if(phase == QStringLiteral("s") || phase == QStringLiteral("S")) {
		return Models::Phase::S;
	} else if (phase == QStringLiteral("l") || phase == QStringLiteral("L")) {
		return Models::Phase::L;
	} else if (phase == QStringLiteral("g") || phase == QStringLiteral("G")) {
		return Models::Phase::G;
	} else {
		return Models::Phase::Error;
	}
}

