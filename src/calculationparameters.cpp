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

template<typename T>
QStringList to_list() {
	auto me = QMetaEnum::fromType<T>();
	auto max = me.keyCount();
	QStringList list;
	for(int i = 0; i != max; ++i) {
		list.append(me.valueToKey(me.value(i)));
	}
	return list;
}

CalculationParameters::CalculationParameters(QWidget *parent) :
	QGroupBox(parent),
	ui(new Ui::CalculationParameters)
{
	ui->setupUi(this);
	setTitle(QStringLiteral("Calculation parameters"));

	auto workmodes = to_list<ParametersNS::Workmode>();
	auto targets = to_list<ParametersNS::Target>();
	auto liquid_solutions = to_list<ParametersNS::LiquidSolution>();
	auto composition_units = to_list<ParametersNS::CompositionUnit>();
	auto temperature_units = to_list<ParametersNS::TemperatureUnit>();
	auto pressure_units = to_list<ParametersNS::PressureUnit>();
	auto databases = to_list<ParametersNS::Databases>();
	auto choose_substances = to_list<ParametersNS::ChooseSubstances>();
	auto extrapolation = to_list<ParametersNS::Extrapolation>();

	ui->composition1_units->set



}

CalculationParameters::~CalculationParameters()
{
	delete ui;
}
