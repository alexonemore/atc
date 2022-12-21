/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2022 Alexandr Shchukin
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

#ifndef THERMODYNAMICS_H
#define THERMODYNAMICS_H

#include "database.h"
#include "parameters.h"
#include "utilities.h"

namespace Thermodynamics {

constexpr double R = 8.31441; // J/molK
constexpr double T0 = 298.15; // K

double ToKelvin(const double t, const ParametersNS::TemperatureUnit tu);
double FromKelvin(const double t, const ParametersNS::TemperatureUnit tu);
ParametersNS::Range RangeToKelvin(const ParametersNS::Range range,
								  const ParametersNS::TemperatureUnit tu);
ParametersNS::Range RangeFromKelvin(const ParametersNS::Range range,
									const ParametersNS::TemperatureUnit tu);
const TempRangeData& FindCoef(const double temperature_K,
							  const SubstanceTempRangeData& coefs);

namespace Thermo {
double TF_F_J(const double temperature_K, const TempRangeData& coef);
double TF_G_kJ(const double temperature_K, const TempRangeData& coef);
double TF_H_kJ(const double temperature_K, const TempRangeData& coef);
double TF_H_J(const double temperature_K, const TempRangeData& coef);
double TF_S_J(const double temperature_K, const TempRangeData& coef);
double TF_Cp_J(const double temperature_K, const TempRangeData& coef);
double TF_c(const double temperature_K, const TempRangeData& coef);
double TF_Tv(const double temperature_K, const TempRangeData& coef);

double TF_F_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_G_kJ(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_H_kJ(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_H_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_S_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_Cp_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_c(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_Tv(const double temperature_K, const SubstanceTempRangeData& coefs);
} // namespace Thermo

namespace HSC {
double TF_F_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_G_kJ(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_H_kJ(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_H_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_S_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_Cp_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_c(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_Tv(const double temperature_K, const SubstanceTempRangeData& coefs);
} // namespace HSC

SubstancesTabulatedTFData
Tabulate(const ParametersNS::Range& temperature_range,
		 const ParametersNS::TemperatureUnit& unit,
		 const ParametersNS::Extrapolation& extrapolation,
		 const ParametersNS::Database& database,
		 const SubstanceTempRangeData& coefs);
void TabulateOneTF(const ParametersNS::Range& temperature_range,
				   const ParametersNS::TemperatureUnit& unit,
				   const ParametersNS::Extrapolation& extrapolation,
				   const ParametersNS::Database& database,
				   const ParametersNS::ThermodynamicFunction& tf,
				   const SubstanceTempRangeData& coefs,
				   QVector<double>& x, QVector<double>& y);


template<typename Container>
void RangeTabulator(ParametersNS::Range range, Container&& x)
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

#endif // THERMODYNAMICS_H
