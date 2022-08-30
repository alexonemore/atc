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

namespace SQL {
const QString available_elements = QStringLiteral(
"SELECT Elements.Symbol "
"FROM Elements "
"WHERE Elements.element_id IN ("
"SELECT DISTINCT CompositionsOfSpecies.element_id "
"FROM CompositionsOfSpecies);");

const QStringList substances_field_names = {
	QStringLiteral("ID"),
	QStringLiteral("Formula"),
	QStringLiteral("Name"),
	QStringLiteral("T min"),
	QStringLiteral("T max")
};

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
	auto q = Query(SQL::available_elements);
	while(q.next()) {
		available_elements.push_back(q.value(0).toString());
	}
	LOG(available_elements)
}

Database::~Database()
{
}

QSqlQuery Database::Query(const QString& query)
{
	LOG()
	QSqlQuery q(query, QSqlDatabase::database(filename_));
	if(q.lastError().isValid()) {
		auto str = q.lastError().text();
		LOG(str)
		LOG(query)
		throw std::runtime_error(str.toStdString().c_str());
	}
	return q;
}


/* **********************************************************************
 * *************************** Thermo ***********************************
 * ********************************************************************** */

DatabaseThermo::DatabaseThermo(const QString& filename)
	: Database(filename)
{

}

SubstancesData DatabaseThermo::GetSubstancesData(const ParametersNS::Parameters& parameters)
{
	auto elements_str = QStringLiteral("'") +
			parameters.checked_elements.join("','") + QStringLiteral("'");
	auto phases = GetPhasesString(parameters.show_phases);
	LOG(phases)
	auto q = Query(SQL::thermo_substances_template.arg(elements_str, phases));
	SubstancesData data;
	while(q.next()) {
		data.push_back(SubstanceData{q.value(0).toInt(),
									 q.value(1).toString(),
									 q.value(2).toString(),
									 q.value(3).toDouble(),
									 q.value(4).toDouble()});
	}
	return data;
}


/* **********************************************************************
 * *************************** HSC **************************************
 * ********************************************************************** */

DatabaseHSC::DatabaseHSC(const QString& filename)
	: Database(filename)
{

}

SubstancesData DatabaseHSC::GetSubstancesData(const ParametersNS::Parameters& parameters)
{
	auto elements_str = QStringLiteral("'") +
			parameters.checked_elements.join("','") + QStringLiteral("'");
	auto phases = GetPhasesString(parameters.show_phases);
	LOG(phases)
	auto q = Query(SQL::hsc_substances_template.arg(elements_str, phases));
	SubstancesData data;
	while(q.next()) {
		data.push_back(SubstanceData{q.value(0).toInt(),
									 q.value(1).toString(),
									 q.value(2).toString(),
									 q.value(3).toDouble(),
									 q.value(4).toDouble()});
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

