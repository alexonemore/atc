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

#include <QObject>
#include <QMetaEnum>

namespace ParametersNS {
	Q_NAMESPACE
enum class Workmode {
	SinglePoint,
	TemperatureRange,
//	PressureRange, TODO
	CompositionRange,
	DoubleCompositionRange,
	TemperatureCompositionRange
};
Q_ENUM_NS(Workmode)
enum class Target {
	AdiabaticTemperature,
	Equilibrium
};
Q_ENUM_NS(Target)
enum class LiquidSolution {
	NoLiquidSolution,
	OneLiquidSolution
};
Q_ENUM_NS(LiquidSolution)
enum class CompositionUnit {
	AtomicPercent,
	WeightPercent,
	Mol,
	Gram
};
Q_ENUM_NS(CompositionUnit)
enum class TemperatureUnit {
	Kelvin,
	Celsius,
	Fahrenheit
};
Q_ENUM_NS(TemperatureUnit)
enum class PressureUnit {
	MPa,
	atm,
	bar
};
Q_ENUM_NS(PressureUnit)
enum class Databases {
	Thermo,
	HSC
};
Q_ENUM_NS(Databases)
enum class ChooseSubstances {
	AsChecked,
	ByMinimumGibbsEnergy
};
Q_ENUM_NS(ChooseSubstances)
enum class Extrapolation {
	Enable,
	Disable
};
Q_ENUM_NS(Extrapolation)
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
