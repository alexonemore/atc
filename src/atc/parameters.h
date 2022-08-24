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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QStringList>

namespace ParametersNS {
enum class Workmode {
	SinglePoint,
	TemperatureRange,
//	PressureRange, TODO
	CompositionRange,
	DoubleCompositionRange,
	TemperatureCompositionRange
};
const QStringList workmode{
	QT_TR_NOOP("Single point"),
	QT_TR_NOOP("Temperature range"),
//	QT_TR_NOOP("PressureRange"), TODO
	QT_TR_NOOP("Composition range"),
	QT_TR_NOOP("Double composition range"),
	QT_TR_NOOP("Temperature-composition range")
};

enum class Target {
	AdiabaticTemperature,
	Equilibrium
};
const QStringList target{
	QT_TR_NOOP("Adibatic temperature"),
	QT_TR_NOOP("Equilibrium")
};

enum class LiquidSolution {
	No,
	One
};
const QStringList liquid_solution{
	QT_TR_NOOP("No"),
	QT_TR_NOOP("One")
};

enum class CompositionUnit {
	AtomicPercent,
	WeightPercent,
	Mol,
	Gram
};
const QStringList composition_units{
	QT_TR_NOOP("at. %"),
	QT_TR_NOOP("wt. %"),
	QT_TR_NOOP("mol"),
	QT_TR_NOOP("gram")
};

enum class TemperatureUnit {
	Kelvin,
	Celsius,
	Fahrenheit
};
const QStringList temperature_units{
	QT_TR_NOOP("K"),
	QT_TR_NOOP("C"),
	QT_TR_NOOP("F")
};

enum class PressureUnit {
	MPa,
	atm,
	bar
};
const QStringList pressure_units{
	QT_TR_NOOP("MPa"),
	QT_TR_NOOP("atm"),
	QT_TR_NOOP("bar")
};

enum class Databases {
	Thermo,
	HSC
};
const QStringList datebases{
	QT_TR_NOOP("Thermo"),
	QT_TR_NOOP("HSC")
};

enum class ChooseSubstances {
	AsChecked,
	ByMinimumGibbsEnergy
};
const QStringList choose_substances{
	QT_TR_NOOP("As checked"),
	QT_TR_NOOP("By minimum Gibbs")
};

enum class Extrapolation {
	Enable,
	Disable
};
const QStringList extrapolation{
	QT_TR_NOOP("Enable"),
	QT_TR_NOOP("Disable")
};

}

struct Range {
	double start, stop, step;
};

class Parameters
{

public:
	Parameters();

};

#endif // PARAMETERS_H
