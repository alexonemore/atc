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

#ifndef THERMODYNAMICS_H
#define THERMODYNAMICS_H

#include "database.h"

namespace Thermodynamics {

constexpr double R = 8.31441; // J/molK

double ToKelvin(const double t, const ParametersNS::TemperatureUnit tu);
double FromKelvin(const double t, const ParametersNS::TemperatureUnit tu);

namespace Thermo {
double TF_F_J(const double temperature_K, const TempRangeData& coef);
double TF_G_kJ(const double temperature_K, const TempRangeData& coef);
double TF_H_kJ(const double temperature_K, const TempRangeData& coef);
double TF_H_J(const double temperature_K, const TempRangeData& coef);
double TF_S_J(const double temperature_K, const TempRangeData& coef);
double TF_Cp_J(const double temperature_K, const TempRangeData& coef);
double TF_c(const double temperature_K, const TempRangeData& coef);
double TF_Tv(const double temperature_K, const TempRangeData& coef);
}

namespace HSC {
double TF_F_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_G_kJ(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_H_kJ(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_H_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_S_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_Cp_J(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_c(const double temperature_K, const SubstanceTempRangeData& coefs);
double TF_Tv(const double temperature_K, const SubstanceTempRangeData& coefs);
}


} // namespace Thermodynamics



#endif // THERMODYNAMICS_H
