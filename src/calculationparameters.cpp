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

#include "calculationparameters.h"
#include "ui_calculationparameters.h"
#include "parameters.h"

CalculationParameters::CalculationParameters(QWidget *parent) :
	QGroupBox(parent),
	ui(new Ui::CalculationParameters)
{
	ui->setupUi(this);
	setTitle(QStringLiteral("Calculation parameters"));

	ui->workmode->addItems(ParametersNS::workmode);
	ui->target->addItems(ParametersNS::target);
	ui->liquid_solution->addItems(ParametersNS::liquid_solution);
	ui->database->addItems(ParametersNS::datebases);
	ui->choose_substances->addItems(ParametersNS::choose_substances);
	ui->extrapolation->addItems(ParametersNS::extrapolation);
	ui->minimization_function->addItems(ParametersNS::minimization_function);
	ui->composition1_units->addItems(ParametersNS::composition_units);
	ui->composition2_units->addItems(ParametersNS::composition_units);
	ui->group1_units->addItems(ParametersNS::composition_units);
	ui->group2_units->addItems(ParametersNS::composition_units);
	ui->temperature_initial_units->addItems(ParametersNS::temperature_units);
	ui->temperature_units->addItems(ParametersNS::temperature_units);
	ui->pressure_initial_units->addItems(ParametersNS::pressure_units);
	ui->pressure_units->addItems(ParametersNS::pressure_units);

	SetupInitialParameters();

}

CalculationParameters::~CalculationParameters()
{
	delete ui;
}

void CalculationParameters::SetupInitialParameters()
{
	ParametersNS::Parameters p;
	ui->workmode->setCurrentIndex(static_cast<int>(p.workmode));
	ui->target->setCurrentIndex(static_cast<int>(p.target));
	ui->liquid_solution->setCurrentIndex(static_cast<int>(p.liquid_solution));
	ui->choose_substances->setCurrentIndex(static_cast<int>(p.choose_substances));
	ui->database->setCurrentIndex(static_cast<int>(p.database));
	ui->minimization_function->setCurrentIndex(static_cast<int>(p.minimization_function));
	ui->extrapolation->setCurrentIndex(static_cast<int>(p.extrapolation));
	ui->group1_units->setCurrentIndex(static_cast<int>(p.group1_unit));
	ui->group2_units->setCurrentIndex(static_cast<int>(p.group2_unit));
	ui->temperature_initial_units->setCurrentIndex(static_cast<int>(p.temperature_initial_unit));
	ui->pressure_initial_units->setCurrentIndex(static_cast<int>(p.pressure_initial_unit));
	ui->composition1_units->setCurrentIndex(static_cast<int>(p.composition1_unit));
	ui->composition2_units->setCurrentIndex(static_cast<int>(p.composition2_unit));
	ui->temperature_units->setCurrentIndex(static_cast<int>(p.temperature_range_unit));
	ui->pressure_units->setCurrentIndex(static_cast<int>(p.pressure_range_unit));




}
