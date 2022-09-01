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

#include "thermodynamics.h"
#include <cmath>

namespace Thermodynamics {

double ToKelvin(const double t, const ParametersNS::TemperatureUnit tu)
{
	switch(tu) {
	case ParametersNS::TemperatureUnit::Kelvin:
		return t;
	case ParametersNS::TemperatureUnit::Celsius:
		return t + 273.15;
	case ParametersNS::TemperatureUnit::Fahrenheit:
		return (t + 459.67) * 5 / 9;
	}
	return t;
}

double FromKelvin(const double t, const ParametersNS::TemperatureUnit tu)
{
	switch(tu) {
	case ParametersNS::TemperatureUnit::Kelvin:
		return t;
	case ParametersNS::TemperatureUnit::Celsius:
		return t - 273.15;
	case ParametersNS::TemperatureUnit::Fahrenheit:
		return (t * 9 / 5) - 459.67;
	}
	return t;
}

namespace Thermo {
double TF_F_J(const double temperature_K, const TempRangeData& coef)
{
	const double x = temperature_K * 1.0E-04;
	const double f1 = coef.f1;
	const double f2 = coef.f2;
	const double f3 = coef.f3;
	const double f4 = coef.f4;
	const double f5 = coef.f5;
	const double f6 = coef.f6;
	const double f7 = coef.f7;
	return (f1 + (f2 * std::log(x)) + (f3 / (x * x)) + (f4 / x) +
			x * (f5 + x * (f6 + f7 * x)));
}

double TF_G_kJ(const double temperature_K, const TempRangeData& coef)
{
	return (coef.H-(temperature_K * TF_F_J(temperature_K, coef)*1.0E-03));
}

double TF_H_kJ(const double temperature_K, const TempRangeData& coef)
{
	const double x = temperature_K * 1.0E-04;
	const double f2 = coef.f2;
	const double f3 = coef.f3;
	const double f4 = coef.f4;
	const double f5 = coef.f5;
	const double f6 = coef.f6;
	const double f7 = coef.f7;
	return (((((3 * f7 * x + 2 * f6) * x + f5) * x + f2) * x -
			 f4 - (2 * f3 / x)) * 10 + coef.H);
}

double TF_H_J(const double temperature_K, const TempRangeData& coef)
{
	return TF_H_kJ(temperature_K, coef) * 1000;
}

double TF_S_J(const double temperature_K, const TempRangeData& coef)
{
	const double x = temperature_K * 1.0E-04;
	const double f1 = coef.f1;
	const double f2 = coef.f2;
	const double f3 = coef.f3;
	const double f5 = coef.f5;
	const double f6 = coef.f6;
	const double f7 = coef.f7;
	return (f1 + f2 * (1 + std::log(x)) - (f3 / (x * x)) +
			((4 * f7 * x + 3 * f6) * x + 2 * f5) * x);
}

double TF_Cp_J(const double temperature_K, const TempRangeData& coef)
{
	const double x = temperature_K * 1.0E-04;
	const double f2 = coef.f2;
	const double f3 = coef.f3;
	const double f5 = coef.f5;
	const double f6 = coef.f6;
	const double f7 = coef.f7;
	return (f2 + 2 * (((2 * f7 * x + f6) * 3 * x + f5) * x + f3 / (x * x)));
}

double TF_c(const double temperature_K, const TempRangeData& coef)
{
	return ((1000 * coef.H / (Thermodynamics::R * temperature_K)) -
			(TF_F_J(temperature_K, coef) / Thermodynamics::R));
}

double TF_Tv(const double temperature_K, const TempRangeData& coef)
{
	return TF_H_J(temperature_K, coef) / TF_S_J(temperature_K, coef);
}
} // namespace Thermo


} // namespace Thermodynamics



