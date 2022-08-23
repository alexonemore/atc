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

	auto to_list = [](QMetaEnum&& me) -> QStringList {
		auto max = me.keyCount();
		QStringList list;
		for(int i = 0; i != max; ++i) {
			list.append(me.valueToKey(me.value(i)));
		}
		return list;
	};

	auto workmodes = to_list(QMetaEnum::fromType<ParametersNS::Workmode>());
	auto targets = to_list(QMetaEnum::fromType<ParametersNS::Target>());
	auto liquid_solutions = to_list(QMetaEnum::fromType<ParametersNS::LiquidSolution>());
	auto composition_units = to_list(QMetaEnum::fromType<ParametersNS::CompositionUnit>());
	auto temperature_units = to_list(QMetaEnum::fromType<ParametersNS::TemperatureUnit>());
	auto pressure_units = to_list(QMetaEnum::fromType<ParametersNS::PressureUnit>());


}

CalculationParameters::~CalculationParameters()
{
	delete ui;
}
