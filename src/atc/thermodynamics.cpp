/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2023 Alexandr Shchukin
 * Corresponding email: alexonemoreemail@gmail.com
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
	double Cp = (f2 + 2 * (((2 * f7 * x + f6) * 3 * x + f5) * x + f3 / (x * x)));
	return Cp < 0.0 ? 0.0 : Cp;
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
double IntegralOfCp_kJ(const double temperature_K, const TempRangeData& coef)
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
	return 1.0E-3 * ((A * T) + (5.0E-4 * B * T2) - (1.0E5 * C / T) +
			(1.0E-6 * D * T3 / 3) - (5.0E7 * E / T2) + (2.5E-10 * F * T4));
}
double IntegralOfCpByT_J(const double temperature_K, const TempRangeData& coef)
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
	return ((A * std::log(T)) + (1.0E-3 * B * T) - (5.0E4 * C / T2) +
			(5.0E-7 * D * T2) + (((-1.0E8 * E / T3) + (1.0E-9 * F * T3)) / 3));
}
double TF_F_J(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return -((1.0E3 * HSC::TF_G_kJ(temperature_K, coefs) -
			 HSC::TF_H_J(298.15, coefs)) / temperature_K);
}
double TF_G_kJ(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	return (HSC::TF_H_kJ(temperature_K, coefs) -
			1.0E-3 * temperature_K * HSC::TF_S_J(temperature_K, coefs));
}
double TF_H_kJ(const double temperature_K, const SubstanceTempRangeData& coefs)
{
	auto first = coefs.cbegin();
	auto last = std::prev(coefs.cend());

	double H{first->H};

	if(temperature_K < Thermodynamics::T0)
	{
		if(temperature_K < first->T_min) {
			H -= HSC::IntegralOfCp_kJ(first->T_min, *first) -
					HSC::IntegralOfCp_kJ(temperature_K, *first);
		}
		// temperature_K >= first->T_min
		for(auto coef = coefs.cbegin(), end = coefs.cend(); coef != end; ++coef)
		{
			if(coef->T_min >= Thermodynamics::T0) {
				break;
			}
			// coef->T_min < Thermodynamic::T0
			if(temperature_K >= coef->T_max) {
				continue;
			}
			// temperature_K < coef->T_max
			if(coef != first) {
				H -= coef->H;
			}
			auto T_min = std::max(coef->T_min, temperature_K);
			auto T_max = std::min(coef->T_max, Thermodynamics::T0);
			assert(T_max >= T_min);

			auto H_min = HSC::IntegralOfCp_kJ(T_min, *coef);
			auto H_max = HSC::IntegralOfCp_kJ(T_max, *coef);
			H -= H_max - H_min;
		}
	} else { // temperature_K >= Thermodynamics::T0
		for(auto coef = coefs.cbegin(), end = coefs.cend(); coef != end; ++coef)
		{
			if(coef->T_max <= Thermodynamics::T0) {
				continue;
			}
			// coef->T_max > Thermodynamics::T0
			if(coef->T_min >= temperature_K) {
				break;
			}
			// coef->T_min <= temperature_K

			if(coef != first && coef->T_min > Thermodynamics::T0) {
				H += coef->H;
			}
			auto T_min = std::max(coef->T_min, Thermodynamics::T0);
			auto T_max = std::min(coef->T_max, temperature_K);
			assert(T_max >= T_min);

			auto H_min = HSC::IntegralOfCp_kJ(T_min, *coef);
			auto H_max = HSC::IntegralOfCp_kJ(T_max, *coef);
			H += H_max - H_min;

			if(coef == last && temperature_K > coef->T_max) {
				H += HSC::IntegralOfCp_kJ(temperature_K, *coef) -
						HSC::IntegralOfCp_kJ(coef->T_max, *coef);
			}
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
	auto first = coefs.cbegin();
	auto last = std::prev(coefs.cend());

	double S{first->S};

	if(temperature_K < Thermodynamics::T0)
	{
		if(temperature_K < first->T_min) {
			S -= HSC::IntegralOfCpByT_J(first->T_min, *first) -
					HSC::IntegralOfCpByT_J(temperature_K, *first);
		}
		for(auto coef = coefs.cbegin(), end = coefs.cend(); coef != end; ++coef)
		{
			if(coef->T_min >= Thermodynamics::T0) {
				break;
			}
			// coef->T_min < Thermodynamic::T0
			if(temperature_K >= coef->T_max) {
				continue;
			}
			// temperature_K < coef->T_max
			if(coef != first) {
				S -= coef->S;
			}
			auto T_min = std::max(coef->T_min, temperature_K);
			auto T_max = std::min(coef->T_max, Thermodynamics::T0);
			assert(T_max >= T_min);

			auto S_min = HSC::IntegralOfCpByT_J(T_min, *coef);
			auto S_max = HSC::IntegralOfCpByT_J(T_max, *coef);
			S -= S_max - S_min;
		}
	} else { // temperature_K >= Thermodynamics::T0
		for(auto coef = coefs.cbegin(), end = coefs.cend(); coef != end; ++coef)
		{
			if(coef->T_max <= Thermodynamics::T0) {
				continue;
			}
			// coef->T_max > Thermodynamics::T0
			if(coef->T_min >= temperature_K) {
				break;
			}
			// coef->T_min <= temperature_K

			if(coef != first && coef->T_min > Thermodynamics::T0) {
				S += coef->S;
			}
			auto T_min = std::max(coef->T_min, Thermodynamics::T0);
			auto T_max = std::min(coef->T_max, temperature_K);
			assert(T_max >= T_min);

			auto S_min = HSC::IntegralOfCpByT_J(T_min, *coef);
			auto S_max = HSC::IntegralOfCpByT_J(T_max, *coef);
			S += S_max - S_min;

			if(coef == last && temperature_K > coef->T_max) {
				S += HSC::IntegralOfCpByT_J(temperature_K, *coef) -
						HSC::IntegralOfCpByT_J(coef->T_max, *coef);
			}
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
	double Cp = (A + (B * T * 1.0E-03) + (C * 1.0E05 / T2) + (D * T2 * 1.0E-06)
			+ (E * 1.0E08 / T3) + (F * T3 * 1.0E-9));
	return Cp < 0.0 ? 0.0 : Cp;
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

void TabulateOneTF(const ParametersNS::Range& temperature_range,
				   const ParametersNS::TemperatureUnit& unit,
				   const ParametersNS::Extrapolation& extrapolation,
				   const ParametersNS::Database& database,
				   const ParametersNS::ThermodynamicFunction& tf,
				   const SubstanceTempRangeData& coefs,
				   QVector<double>& x, QVector<double>& y)
{
	ParametersNS::Range range_in_unit{temperature_range};
	if(extrapolation == ParametersNS::Extrapolation::Disable) {
		auto T1 = FromKelvin(coefs.cbegin()->T_min, unit);
		auto T2 = FromKelvin(coefs.crbegin()->T_max, unit);
		range_in_unit.start = std::clamp(range_in_unit.start, T1, T2);
		range_in_unit.stop = std::clamp(range_in_unit.stop, T1, T2);
	}

	x.clear();
	RangeTabulator(range_in_unit, x);
	auto size = x.size();
	y.clear();
	y.resize(size);
	QVector<double> kelvins(size);
	std::transform(x.cbegin(), x.cend(), kelvins.begin(), [unit](auto t){
		return ToKelvin(t, unit); });
	switch(database) {
	case ParametersNS::Database::Thermo:
		switch(tf) {
		case ParametersNS::ThermodynamicFunction::G_kJ:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return Thermo::TF_G_kJ(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::H_kJ:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return Thermo::TF_H_kJ(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::F_J:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return Thermo::TF_F_J(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::S_J:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return Thermo::TF_S_J(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::Cp_J:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return Thermo::TF_Cp_J(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::c:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return Thermo::TF_c(t, coefs);});
			break;
		}
		break;
	case ParametersNS::Database::HSC:
		switch(tf) {
		case ParametersNS::ThermodynamicFunction::G_kJ:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return HSC::TF_G_kJ(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::H_kJ:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return HSC::TF_H_kJ(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::F_J:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return HSC::TF_F_J(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::S_J:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return HSC::TF_S_J(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::Cp_J:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return HSC::TF_Cp_J(t, coefs);});
			break;
		case ParametersNS::ThermodynamicFunction::c:
			std::transform(kelvins.cbegin(), kelvins.cend(), y.begin(),
						   [coefs](auto t){return HSC::TF_c(t, coefs);});
			break;
		}
		break;
	}
}

} // namespace Thermodynamics

