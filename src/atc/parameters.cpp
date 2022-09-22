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
	QT_TR_NOOP("Double composition range"),
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
	QStringLiteral("databases/thermo_database.db"),
	QStringLiteral("databases/hsc_database.db")
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

Parameters::Parameters()
	: threads{QThread::idealThreadCount()}
{

}

} // namespace ParametersNS

