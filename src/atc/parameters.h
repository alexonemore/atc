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
extern const QStringList workmode;

enum class Target {
	AdiabaticTemperature,
	Equilibrium
};
extern const QStringList target;

enum class LiquidSolution {
	No,
	One
};
extern const QStringList liquid_solution;

enum class CompositionUnit {
	AtomicPercent,
	WeightPercent,
	Mol,
	Gram
};
extern const QStringList composition_units;

enum class TemperatureUnit {
	Kelvin,
	Celsius,
	Fahrenheit
};
extern const QStringList temperature_units;

enum class PressureUnit {
	MPa,
	atm,
	bar
};
extern const QStringList pressure_units;

enum class Database {
	Thermo,
	HSC
};
extern const QStringList datebases;

enum class ChooseSubstances {
	AsChecked,
	ByMinimumGibbsEnergy
};
extern const QStringList choose_substances;

enum class Extrapolation {
	Enable,
	Disable
};
extern const QStringList extrapolation;

enum class MinimizationFunction {
	GibbsEnergy,
	Entropy
};
extern const QStringList minimization_function;

struct Range {
	double start, stop, step;
};

struct ShowPhases {
	bool gas{true};
	bool liquid{true};
	bool solid{true};
//	bool aqueous{false}; TODO
//	bool ions{false}; TODO
};

struct Parameters
{
	Workmode		workmode			{Workmode::SinglePoint};
	Target			target				{Target::AdiabaticTemperature};
	LiquidSolution	liquid_solution		{LiquidSolution::One};
	ChooseSubstances choose_substances	{ChooseSubstances::AsChecked};
	Database		database			{Database::Thermo};
	MinimizationFunction minimization_function {MinimizationFunction::GibbsEnergy};
	Extrapolation	extrapolation		{Extrapolation::Enable};
	CompositionUnit group1_unit			{CompositionUnit::AtomicPercent};
	CompositionUnit group2_unit			{CompositionUnit::AtomicPercent};
	CompositionUnit composition1_unit	{CompositionUnit::AtomicPercent};
	CompositionUnit composition2_unit	{CompositionUnit::AtomicPercent};
	TemperatureUnit	temperature_initial_unit {TemperatureUnit::Kelvin};
	TemperatureUnit temperature_range_unit {TemperatureUnit::Kelvin};
	PressureUnit	pressure_initial_unit {PressureUnit::MPa};
	PressureUnit	pressure_range_unit {PressureUnit::MPa};
	double			temperature_initial	{300.0};
	double			pressure_initial	{0.1};
	Range			composition1_range	{  0.0,  100.0,  1.0};
	Range			composition2_range	{  0.0,  100.0,  1.0};
	Range			temperature_range	{300.0, 1000.0, 10.0};
	Range			pressure_range		{  0.1,    1.0,  0.1};
	int				threads				{1};
	int				at_accuracy			{1}; // digits after the decimal point
	ShowPhases		show_phases			{};

	Parameters();

};
} // namespace ParametersNS
#endif // PARAMETERS_H
