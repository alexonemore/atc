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
#include "parameters.h"
#include "utilities.h"

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

struct OptimizationItem
{
	ParametersNS::Parameters parameters;
	std::vector<int> elements;
	SubstancesTempRangeData temp_ranges;
	SubstancesElementComposition subs_element_composition;
	SubstanceWeights weights;
	Composition amounts;

	std::vector<double> n, c;				// size = N, number_of_substances
	std::vector<double> ub_ini, ub_cur;		// size = N, ub = upper_bounds
	std::vector<Constraint> constraints;	// size = M, number_of_elements
	std::vector<int> substances_id_order;	// size = N, in order
	double temperature_K_initial{0};
	double temperature_K_current{0};
	double temperature_K_adiabatic{0};
	size_t number_of_gases{0};
	size_t number_of_liquids{0};
	size_t number_of_individuals{0};
	size_t number_of_substances{0};			// N

	OptimizationItem(const ParametersNS::Parameters& parameters_,
					 const std::vector<int>& elements_,
					 const SubstancesTempRangeData& temp_ranges_,
					 const SubstancesElementComposition& subs_element_composition_,
					 const SubstanceWeights& weights_,
					 const Composition& amounts_,
					 const double initial_temperature_K);
	void Calculate() {}
private:
	void DefineOrderOfSubstances();
	void MakeConstraints();
};

using OptimizationVector = QVector<OptimizationItem>;

class OptimizationItemsMaker
{
	ParametersNS::Parameters parameters;
	size_t number_of_elements{0};	// M
	size_t number_of_substances{0};	// N


	OptimizationVector items;
public:
	OptimizationItemsMaker(const ParametersNS::Parameters& parameters_,
						   const std::vector<int>& elements,
						   const SubstancesTempRangeData& temp_ranges,
						   const SubstancesElementComposition& subs_element_composition,
						   const SubstanceWeights& weights,
						   const Composition& amounts);
	OptimizationVector&& GetData() && { return std::move(items); }

private:
	std::vector<double> MakeTemperatureVector();
	std::vector<double> MakeCompositionVector(ParametersNS::Range range);

};

template<typename Container>
void RangeTabulator(ParametersNS::Range range, Container& x)
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

} // namespace Optimization

#endif // OPTIMIZATION_H
