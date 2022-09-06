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
#include "utilities.h"
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

ParametersNS::Range RangeToKelvin(const ParametersNS::Range range,
								  const ParametersNS::TemperatureUnit tu)
{
	ParametersNS::Range new_range{range};
	new_range.start = ToKelvin(new_range.start, tu);
	new_range.stop = ToKelvin(new_range.stop, tu);
	return new_range;
}

ParametersNS::Range RangeFromKelvin(const ParametersNS::Range range,
									const ParametersNS::TemperatureUnit tu)
{
	ParametersNS::Range new_range{range};
	new_range.start = FromKelvin(new_range.start, tu);
	new_range.stop = FromKelvin(new_range.stop, tu);
	return new_range;
}

const TempRangeData& FindCoef(const double temperature_K,
							  const SubstanceTempRangeData& coefs)
{
	assert(coefs.size() > 0 && "coefs.size() == 0");
	if(temperature_K < coefs[0].T_min) {
		return coefs[0];
	}
#if 1
	for(const auto& it : coefs) {
		if(temperature_K < it.T_max) return it;
	}
	return coefs[coefs.size()-1];
#else
	auto f = std::find_if(coefs.cbegin(), coefs.cend(),
						  [temperature_K](const auto& coef){
		return temperature_K < coef.T_max;
	});
	if(f == coefs.cend()) {
		return *coefs.crbegin(); // coefs[coefs.size()-1];
	} else {
		return *f;
	}
#endif
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
	return (coef.H -
			(temperature_K * Thermo::TF_F_J(temperature_K, coef) * 1.0E-03));
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
	return (Thermo::TF_H_kJ(temperature_K, coef) * 1000);
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
			(Thermo::TF_F_J(temperature_K, coef) / Thermodynamics::R));
}

double TF_Tv(const double temperature_K, const TempRangeData& coef)
{
	return (Thermo::TF_H_J(temperature_K, coef) /
			Thermo::TF_S_J(temperature_K, coef));
}

double TF_F_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return TF_F_J(temperature_K, FindCoef(temperature_K, coefs));
}
double TF_G_kJ(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return TF_G_kJ(temperature_K, FindCoef(temperature_K, coefs));
}
double TF_H_kJ(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return TF_H_kJ(temperature_K, FindCoef(temperature_K, coefs));
}
double TF_H_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return TF_H_J(temperature_K, FindCoef(temperature_K, coefs));
}
double TF_S_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return TF_S_J(temperature_K, FindCoef(temperature_K, coefs));
}
double TF_Cp_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return TF_Cp_J(temperature_K, FindCoef(temperature_K, coefs));
}
double TF_c(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return TF_c(temperature_K, FindCoef(temperature_K, coefs));
}
double TF_Tv(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return TF_Tv(temperature_K, FindCoef(temperature_K, coefs));
}
} // namespace Thermo


namespace HSC {
double IntgralOfCp_kJ(const double temperature_K, const TempRangeData& coef)
{
	const double T = temperature_K;
	const double T2 = T*T;
	const double T3 = T2*T;
	const double T4 = T3*T;
	const double A = coef.f1;
	const double B = coef.f2;
	const double C = coef.f3;
	const double D = coef.f4;
	const double E = coef.f5;
	const double F = coef.f6;
	return 1.0E-3 * ((A * T) + (2.0E-3 * B * T2) - (1.0E5 * C / T) +
			(1.0E-6 * D * T3 / 3) - (5.0E7 * E / T2) + (2.5E-10 * F * T4));
}
double IntegralOfCpByT_kJ(const double temperature_K, const TempRangeData& coef)
{
	const double T = temperature_K;
	const double T2 = T*T;
	const double T3 = T2*T;
	const double A = coef.f1;
	const double B = coef.f2;
	const double C = coef.f3;
	const double D = coef.f4;
	const double E = coef.f5;
	const double F = coef.f6;
	return 1.0E-3 * ((A * std::log(T)) + (1.0E-3 * B * T) - (5.0E4 * C / T2) +
			(5.0E-7 * D * T2) + (((-1.0E8 * E / T3) + (1.0E-9 * F * T3)) / 3));
}
double TF_F_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return ((1.0E3 * HSC::TF_G_kJ(temperature_K, coefs) -
			 HSC::TF_H_J(298.15, coefs)) / temperature_K);
}
double TF_G_kJ(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return (HSC::TF_H_kJ(temperature_K, coefs) -
			1.0E-3 * temperature_K * HSC::TF_S_J(temperature_K, coefs));
}
double TF_H_kJ(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	double H{0};

	auto coef_first = coefs.cbegin();
	if(temperature_K < coef_first->T_min) {
		H += coef_first->H;
		H -= HSC::IntgralOfCp_kJ(coef_first->T_min, *coef_first) -
				HSC::IntgralOfCp_kJ(temperature_K, *coef_first);
		return H;
	}

	auto coef_last = coefs.crbegin();
	if(temperature_K > coef_last->T_max) {
		for(auto&& coef : coefs) {
			H += coef.H;
			H += HSC::IntgralOfCp_kJ(coef.T_max, coef) -
					HSC::IntgralOfCp_kJ(coef.T_min, coef);
		}
		H += HSC::IntgralOfCp_kJ(temperature_K, *coef_last) -
				HSC::IntgralOfCp_kJ(coef_last->T_max, *coef_last);
		return H;
	}

	for(auto&& coef : coefs) {
		H += coef.H;
		if(temperature_K > coef.T_max) {
			H += HSC::IntgralOfCp_kJ(coef.T_max, coef) -
					HSC::IntgralOfCp_kJ(coef.T_min, coef);
		} else {
			H += HSC::IntgralOfCp_kJ(temperature_K, coef) -
					HSC::IntgralOfCp_kJ(coef.T_min, coef);
			break;
		}
	}
	return H;
}
double TF_H_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return (HSC::TF_H_kJ(temperature_K, coefs) * 1.0E3);
}
double TF_S_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	double S{0};

	auto coef_first = coefs.cbegin();
	if(temperature_K < coef_first->T_min) {
		S += coef_first->S;
		S -= HSC::IntegralOfCpByT_kJ(coef_first->T_min, *coef_first) -
				HSC::IntegralOfCpByT_kJ(temperature_K, *coef_first);
		return S;
	}

	auto coef_last = coefs.crbegin();
	if(temperature_K > coef_last->T_max) {
		for(auto&& coef : coefs) {
			S += coef.S;
			S += HSC::IntegralOfCpByT_kJ(coef.T_max, coef) -
					HSC::IntegralOfCpByT_kJ(coef.T_min, coef);
		}
		S += HSC::IntegralOfCpByT_kJ(temperature_K, *coef_last) -
				HSC::IntegralOfCpByT_kJ(coef_last->T_max, *coef_last);
		return S;
	}

	for(auto&& coef : coefs) {
		S += coef.S;
		if(temperature_K > coef.T_max) {
			S += HSC::IntegralOfCpByT_kJ(coef.T_max, coef) -
					HSC::IntegralOfCpByT_kJ(coef.T_min, coef);
		} else {
			S += HSC::IntegralOfCpByT_kJ(temperature_K, coef) -
					HSC::IntegralOfCpByT_kJ(coef.T_min, coef);
			break;
		}
	}
	return S;
}
double TF_Cp_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	auto&& f = FindCoef(temperature_K, coefs);
	const double A = f.f1;
	const double B = f.f2;
	const double C = f.f3;
	const double D = f.f4;
	const double E = f.f5;
	const double F = f.f6;
	const double T = temperature_K;
	const double T2 = T*T;
	const double T3 = T2*T;
	return (A + (B * T * 1.0E-03) + (C * 1.0E05 / T2) + (D * T2 * 1.0E-06)
			+ (E * 1.0E08 / T3) + (F * T3 * 1.0E-9));
}
double TF_c(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return (1.0E3 * HSC::TF_G_kJ(temperature_K, coefs) /
			(Thermodynamics::R * temperature_K));
}
double TF_Tv(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return (HSC::TF_H_J(temperature_K, coefs) /
			HSC::TF_S_J(temperature_K, coefs));
}
} // namespace HSC

SubstancesTabulatedTFData
Tabulate(const ParametersNS::Range& temperature_range,
		 const ParametersNS::TemperatureUnit& unit,
		 const ParametersNS::Extrapolation& extrapolation,
		 const ParametersNS::Database& database,
		 const SubstanceTempRangeData& coefs)
{
	ParametersNS::Range range_in_unit{temperature_range};
	if(extrapolation == ParametersNS::Extrapolation::Disable) {
		auto T1 = FromKelvin(coefs.cbegin()->T_min, unit);
		auto T2 = FromKelvin(coefs.crbegin()->T_max, unit);
		range_in_unit.start = std::clamp(range_in_unit.start, T1, T2);
		range_in_unit.stop = std::clamp(range_in_unit.stop, T1, T2);
	}
	SubstancesTabulatedTFData data;
	data.temperature_unit = unit;

	// TODO tabulate by diapasons
	RangeTabulator(range_in_unit, data.temperatures);
	auto size = data.temperatures.size();
	data.G_kJ.resize(size);
	data.H_kJ.resize(size);
	data.F_J.resize(size);
	data.S_J.resize(size);
	data.Cp_J.resize(size);
	data.c.resize(size);
	QVector<double> kelvins(size);
	std::transform(data.temperatures.cbegin(), data.temperatures.cend(),
				   kelvins.begin(), [unit](auto t){return ToKelvin(t, unit);});
	switch(database) {
	case ParametersNS::Database::Thermo:
		std::transform(kelvins.cbegin(), kelvins.cend(), data.G_kJ.begin(),
					   [coefs](auto t){return Thermo::TF_G_kJ(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.H_kJ.begin(),
					   [coefs](auto t){return Thermo::TF_H_kJ(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.F_J.begin(),
					   [coefs](auto t){return Thermo::TF_F_J(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.S_J.begin(),
					   [coefs](auto t){return Thermo::TF_S_J(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.Cp_J.begin(),
					   [coefs](auto t){return Thermo::TF_Cp_J(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.c.begin(),
					   [coefs](auto t){return Thermo::TF_c(t, coefs);});
		break;
	case ParametersNS::Database::HSC:
		std::transform(kelvins.cbegin(), kelvins.cend(), data.G_kJ.begin(),
					   [coefs](auto t){return HSC::TF_G_kJ(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.H_kJ.begin(),
					   [coefs](auto t){return HSC::TF_H_kJ(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.F_J.begin(),
					   [coefs](auto t){return HSC::TF_F_J(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.S_J.begin(),
					   [coefs](auto t){return HSC::TF_S_J(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.Cp_J.begin(),
					   [coefs](auto t){return HSC::TF_Cp_J(t, coefs);});
		std::transform(kelvins.cbegin(), kelvins.cend(), data.c.begin(),
					   [coefs](auto t){return HSC::TF_c(t, coefs);});
		break;
	}
	return data;
}

void RangeTabulator(const ParametersNS::Range range, QVector<double>& x)
{
	const double start = range.start;
	const double stop = range.stop;
	const double step = range.step;
	constexpr double eps = std::numeric_limits<double>::epsilon();
	x.clear();
	if(std::abs(stop - start) < eps) {
		x.push_back(start);
	} else if(std::abs(step) < eps) {
		x.reserve(2);
		x.push_back(start);
		x.push_back(stop);
	} else {
		int size = static_cast<int>(std::ceil(std::abs((stop-start)/step)))+1;
		x.reserve(size);
		double rounding_decimal = 1/step < 1000 ?
					1000 : std::pow(10, std::ceil(std::log10(1/step))+1);
		int i = 1;
		double current = start;
		while (current < stop) {
			x.push_back(current);
			current = std::round((start + step * (i++)) * rounding_decimal)
					/ rounding_decimal;
		}
		x.push_back(stop);
		LOG("size = ", size)
				LOG("x.size = ", x.size())
				assert(x.size() == size && "Reserve fail");
	}
}

} // namespace Thermodynamics



