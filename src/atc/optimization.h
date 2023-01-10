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

#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "database.h"
#include "amountsmodel.h"
#include "parameters.h"
#include <nlopt.hpp>

/* Order of substunces in vector n, size = N
|------gas------|---------liq-------|------ind------|
|0	|1	|2	|3	|0	|1	|2	|3	|4	|0	|1	|2	|3	|
|0	|1	|2	|3	|4	|5	|6	|7	|8	|9	|10	|11	|12	|
*/

namespace Optimization {

struct Constraint
{
	std::vector<double> a_j;
	double b_j{0};
};

struct Numbers
{
	size_t elements{0};		// M
	size_t substances{0};	// N
	size_t gases{0};		// part of N
	size_t individuals{0};	// part of N
	size_t liquids{0};		// part of N
};

struct OptimizationItem final
{
	ParametersNS::Parameters parameters;
	std::vector<int> elements;
	SubstancesTempRangeData temp_ranges;
	SubstancesElementComposition subs_element_composition;
	SubstanceWeights weights;
	Composition amounts;
	Composition amounts_of_equilibrium;
	Amounts sum_of_initial;
	Amounts sum_of_equilibrium;

	std::vector<double> n, c;				// size = N, number_of_substances
	std::vector<double> ub;					// size = N, ub = upper_bounds
	std::vector<Constraint> constraints;	// size = M, number_of_elements
	std::vector<int> substances_id_order;	// size = N, in order
	double temperature_K_initial{0};
	double temperature_K_current{0};
	double H_initial{0};
	double H_current{0};
	Numbers number;
	double result_of_optimization;
	double composition_variable;

	OptimizationItem(const ParametersNS::Parameters& parameters_,
					 const std::vector<int>& elements_,
					 const SubstancesTempRangeData& temp_ranges_,
					 const SubstancesElementComposition& subs_element_composition_,
					 const SubstanceWeights& weights_,
					 const Composition& amounts_,
					 const double initial_temperature_K,
					 const double variable_composition = 0.0);
#ifndef NDEBUG
	int i;
	~OptimizationItem();
#endif
	void Calculate();
	const std::vector<double>& GetC() const & { return c; }
	auto GetNumbers() const { return number; }
private:
	void DefineOrderOfSubstances();
	void MakeConstraintsMatrixA();
	void MakeConstraintsB();
	void MakeC();
	void MakeUB();
	void MakeN();
	void Equilibrium();
	void Equilibrium(const double temperature_K);
	void AdiabaticTemperature();
	void H_kJ_Initial();
	double H_kJ_Current();
	bool IsExistAtCurrentTemperature(const int sub_id);
	double Minimize(const nlopt::algorithm algorithm, nlopt::result& result);
	void MakeAmountsOfEquilibrium();
};

/* TODO
class OptimizationVector
{
	common_data_for_all;
	QVector<different_data>;
};*/

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using OptimizationVector = std::vector<OptimizationItem>;
#else
using OptimizationVector = QVector<OptimizationItem>;
#endif

class OptimizationItemsMaker final
{
	ParametersNS::Parameters parameters;
	size_t number_of_substances{0};	// N
	Amounts sum;
	OptimizationVector items;
	int x_size{0};
	int y_size{0};
public:
	OptimizationItemsMaker(const ParametersNS::Parameters& parameters_,
						   const std::vector<int>& elements,
						   const SubstancesTempRangeData& temp_ranges,
						   const SubstancesElementComposition& subs_element_composition,
						   const SubstanceWeights& weights,
						   const Composition& amounts);
#ifndef NDEBUG
	int i;
	~OptimizationItemsMaker();
#endif
	OptimizationVector& GetData() & { return items; }
	auto GetXSize() const { return x_size; }
	auto GetYSize() const { return y_size; }

private:
	std::vector<double> MakeTemperatureVector();
	std::vector<double> MakeCompositionVector();
	Composition MakeNewAmount(const Composition& amounts,
							  const SubstanceWeights& weights,
							  const double value);
	auto MakeNewAmounts(const Composition& amounts,
						const SubstanceWeights& weights);
};

} // namespace Optimization

#endif // OPTIMIZATION_H
