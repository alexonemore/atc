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


struct Numbers
{
	size_t elements{0};		// M
	size_t substances{0};	// N
	size_t gases{0};		// part of N
	size_t individuals{0};	// part of N
	size_t liquids{0};		// part of N
};

struct Constraint
{
	const std::vector<double> *a_j;
	double b_j{0};
};

struct OptimizationItem
{
	Numbers numbers;



	void Calculate() {}
};

using OptimizationVector = QVector<OptimizationItem>;

OptimizationVector Prepare(const ParametersNS::Parameters parameters,
						   const std::vector<int>& elements,
						   const SubstancesTempRangeData& temp_ranges,
						   const SubstancesElementComposition& subs_element_composition,
						   const SubstanceWeights& weight,
						   const Composition& amounts);

} // namespace Optimization

#endif // OPTIMIZATION_H
