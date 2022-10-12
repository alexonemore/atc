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

#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "database.h"
#include "amountsmodel.h"

/* Order of substunces in vector n, size = N
|------gas------|---------liq-------|------ind------|
|0	|1	|2	|3	|0	|1	|2	|3	|4	|0	|1	|2	|3	|
|0	|1	|2	|3	|4	|5	|6	|7	|8	|9	|10	|11	|12	|
*/

namespace Optimization {

struct Constraint
{
	const std::vector<double> *a_j;
	double b_j{0};
};

struct OptimizationItem
{
	const ParametersNS::Parameters* parameters;
	std::vector<double> n, c;				// size = N, number_of_substances
	std::vector<double> ub_ini, ub_cur;		// size = N, ub = upper_bounds
	std::vector<Constraint> constraints;	// size = M, number_of_elements
	size_t number_of_gases{0};
	size_t number_of_liquids{0};
	size_t number_of_individuals{0};

	OptimizationItem(const ParametersNS::Parameters* parameters_ = nullptr)
		: parameters{parameters_}
	{}
	void Calculate() {}
};

using OptimizationVector = QVector<OptimizationItem>;

class OptimizationItemsMaker
{
	const ParametersNS::Parameters& parameters;
	size_t number_of_elements{0};	// M
	size_t number_of_substances{0};	// N


	OptimizationVector vec;
public:
	OptimizationItemsMaker(const ParametersNS::Parameters& parameters_,
						   const std::vector<int>& elements,
						   const SubstancesTempRangeData& temp_ranges,
						   const SubstancesElementComposition& subs_element_composition,
						   const SubstanceWeights& weights,
						   const Composition& amounts);
	OptimizationVector GetData() { return std::move(vec); }

private:


};



} // namespace Optimization

#endif // OPTIMIZATION_H
