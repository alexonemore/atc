/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2025 Alexandr Shchukin
 * Corresponding email: shchukin.aleksandr.sergeevich@gmail.com
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

#include "parameters.h"
#include <QThread>

namespace ParametersNS {
const QStringList thermodynamic_function{
	QStringLiteral("G"),
	QStringLiteral("H"),
	QStringLiteral("F"),
	QStringLiteral("S"),
	QStringLiteral("Cp"),
	QStringLiteral("c")
};
const QStringList thermodynamic_function_long{
	QStringLiteral("G_kJ"),
	QStringLiteral("H_kJ"),
	QStringLiteral("F_J"),
	QStringLiteral("S_J"),
	QStringLiteral("Cp_J"),
	QStringLiteral("c")
};
const QStringList thermodynamic_function_full{
	QStringLiteral("G [kJ/mol]"),
	QStringLiteral("H [kJ/mol]"),
	QStringLiteral("F [J/molK]"),
	QStringLiteral("S [J/molK]"),
	QStringLiteral("Cp [J/molK]"),
	QStringLiteral("c [G/RT]")
};
const QStringList workmode{
	QT_TR_NOOP("Single point"),
	QT_TR_NOOP("Temperature range"),
//	QT_TR_NOOP("PressureRange"), TODO
	QT_TR_NOOP("Composition range"),
	QT_TR_NOOP("Temperature-composition range")
};
const QStringList target{
	QT_TR_NOOP("Adibatic temperature"),
	QT_TR_NOOP("Equilibrium")
};
const QStringList liquid_solution{
	QT_TR_NOOP("No"),
	QT_TR_NOOP("One")
};
const QStringList composition_units{
	QT_TR_NOOP("at. %"),
	QT_TR_NOOP("wt. %"),
	QT_TR_NOOP("mol"),
	QT_TR_NOOP("gram")
};
const QStringList temperature_units{
	QStringLiteral("K"),
	QStringLiteral("C"),
	QStringLiteral("F")
};
const QStringList pressure_units{
	QT_TR_NOOP("MPa"),
	QT_TR_NOOP("atm"),
	QT_TR_NOOP("bar")
};
const QStringList databases{
	QStringLiteral("Thermo"),
	QStringLiteral("HSC")
};
const QStringList database_filenames{
	QStringLiteral("databases/database_thermo.db"),
	QStringLiteral("databases/database_hsc.db")
};
const QStringList choose_substances{
	QT_TR_NOOP("As checked"),
	QT_TR_NOOP("By minimum Gibbs energy")
};
const QStringList extrapolation{
	QT_TR_NOOP("Enable"),
	QT_TR_NOOP("Disable")
};
const QStringList minimization_function{
	QT_TR_NOOP("Gibbs energy"),
	QT_TR_NOOP("Entropy")
};
constexpr double min_Kelvin = 0.0;
constexpr double min_Celsius = -273.15;
constexpr double min_Fahrenheit = -459.67;
constexpr double max_temperature = 1E6;
constexpr double max_composition = 1E10;
constexpr double min_pressure = 0.0;
constexpr double max_pressure = 1E10;
constexpr double min_range_step = 1E-4;

Parameters::Parameters()
	: threads{MaxThreadsCount()}
{

}

static double FixTemperature(const double temperature, const TemperatureUnit tu)
{
	switch(tu) {
	case TemperatureUnit::Kelvin:
		return std::clamp(temperature, min_Kelvin, max_temperature);
	case TemperatureUnit::Celsius:
		return std::clamp(temperature, min_Celsius, max_temperature);
	case TemperatureUnit::Fahrenheit:
		return std::clamp(temperature, min_Fahrenheit, max_temperature);
	}
	return temperature;
}

static double FixComposition(const double val, const CompositionUnit cu)
{
	switch(cu) {
	case CompositionUnit::AtomicPercent:
	case CompositionUnit::WeightPercent:
		return std::clamp(val, 0.0, 100.0);
	case CompositionUnit::Mol:
	case CompositionUnit::Gram:
		return std::clamp(val, 0.0, max_composition);
	}
	return val;
}

static void FixRange(Range& range)
{
	if(range.start > range.stop)
		std::swap(range.start, range.stop);
	range.step = std::clamp(range.step, min_range_step, range.stop);
}

void Parameters::FixInputParameters()
{
	temperature_initial = FixTemperature(temperature_initial, temperature_initial_unit);
	pressure_initial = std::clamp(pressure_initial, min_pressure, max_pressure);

	temperature_range.start = FixTemperature(temperature_range.start, temperature_range_unit);
	temperature_range.stop = FixTemperature(temperature_range.stop, temperature_range_unit);
	FixRange(temperature_range);

	composition_range.start = FixComposition(composition_range.start, composition_range_unit);
	composition_range.stop = FixComposition(composition_range.stop, composition_range_unit);
	FixRange(composition_range);

	FixRange(pressure_range);
}

int Parameters::MaxThreadsCount() noexcept
{
	return QThread::idealThreadCount();
}

} // namespace ParametersNS

