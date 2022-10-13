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

#include "optimization.h"
#include "thermodynamics.h"
#include <nlopt.hpp>


namespace Optimization {
constexpr static double epsilon_log = 1E-9;
constexpr static double epsilon_accuracy = 1E-6;
constexpr static double maxtime_of_minimize = 1; // second

static double Log_eps(const double x) // noexcept
{
	return ((std::log(x*x + epsilon_log)) / 2);
}

static double ConstraintFunction(const std::vector<double>& x,
								 std::vector<double>& grad, void* data)
{
	const Constraint* cnt = reinterpret_cast<const Constraint*>(data);
	if(!grad.empty()) {
		assert(cnt->a_j.size() == grad.size() && "grad.size error");
		std::copy(cnt->a_j.cbegin(), cnt->a_j.cend(), grad.begin());
	}
	assert(x.size() == cnt->a_j.size() && "x.size error");
	return std::transform_reduce(x.cbegin(), x.cend(), cnt->a_j.cbegin(),
								 -cnt->b_j); /* minus of bj */
}
#if 0
static double ThermodinamicFunction(const std::vector<double>& n,
									std::vector<double>& grad, void* data)
{
	using DT = std::vector<double>::difference_type;
	const ThermodynamicCalculator::TCNode* tcn =
			reinterpret_cast<const ThermodynamicCalculator::TCNode*>(data);
	auto&& c = tcn->GetC();
	auto&& numbers = tcn->GetNumbers();
	auto n_gas = n.cbegin();
	auto n_liq = std::next(n_gas, static_cast<DT>(numbers.gases));
	auto n_ind = std::next(n_liq, static_cast<DT>(numbers.liquids));
	auto c_gas = c.cbegin();
	auto c_liq = std::next(c_gas, static_cast<DT>(numbers.gases));
	auto c_ind = std::next(c_liq, static_cast<DT>(numbers.liquids));
	double lsg = Log_eps(std::accumulate(n_gas, n_liq, double{0.0}));
	double lsl = Log_eps(std::accumulate(n_liq, n_ind, double{0.0}));
	double result{0.0};
	if(grad.empty()) {
		LOGV("grad.empty()")
		result = std::transform_reduce(n_gas, n_liq, c_gas, double{0.0},
									   std::plus<>(), [lsg](auto ni, auto ci){
			return ni * (ci + Log_eps(ni) - lsg);});
		result = std::transform_reduce(n_liq, n_ind, c_liq, result,
									   std::plus<>(), [lsl](auto ni, auto ci){
			return ni * (ci + Log_eps(ni) - lsl);});
		result = std::transform_reduce(n_ind, n.end(), c_ind, result);
	} else {
		LOGV("!grad.empty()")
		auto grad_gas = grad.begin();
		auto grad_liq = std::next(grad_gas, static_cast<DT>(numbers.gases));
		auto grad_ind = std::next(grad_liq, static_cast<DT>(numbers.liquids));
		std::transform(n_gas, n_liq, c_gas, grad_gas, [lsg](auto ni, auto ci){
			return ci + Log_eps(ni) - lsg;});
		std::transform(n_liq, n_ind, c_liq, grad_liq, [lsl](auto ni, auto ci){
			return ci + Log_eps(ni) - lsl;});
		std::copy(c_ind, c.end(), grad_ind);
		result = std::transform_reduce(n_gas, n.cend(), grad_gas, double{0.0});
	}
	return result;
}
#endif
#ifndef NDEBUG
static const char* NLoptResultToString(nlopt::result result)
{
	switch(result) {
	case(nlopt::result::FAILURE):			return "FAILURE";
	case(nlopt::result::INVALID_ARGS):		return "INVALID_ARGS";
	case(nlopt::result::OUT_OF_MEMORY):		return "OUT_OF_MEMORY";
	case(nlopt::result::ROUNDOFF_LIMITED):	return "ROUNDOFF_LIMITED";
	case(nlopt::result::FORCED_STOP):		return "FORCED_STOP";
	case(nlopt::result::NUM_FAILURES):		return "NUM_FAILURES";
	case(nlopt::result::SUCCESS):			return "SUCCESS";
	case(nlopt::result::STOPVAL_REACHED):	return "STOPVAL_REACHED";
	case(nlopt::result::FTOL_REACHED):		return "FTOL_REACHED";
	case(nlopt::result::XTOL_REACHED):		return "XTOL_REACHED";
	case(nlopt::result::MAXEVAL_REACHED):	return "MAXEVAL_REACHED";
	case(nlopt::result::MAXTIME_REACHED):	return "MAXTIME_REACHED";
	case(nlopt::result::NUM_RESULTS):		return "NUM_RESULTS";
	}
	return "RESULT_IS_OUT_OF_ENUM_VALUES";
}
#endif

OptimizationItemsMaker::OptimizationItemsMaker(
		const ParametersNS::Parameters& parameters_,
		const std::vector<int>& elements,
		const SubstancesTempRangeData& temp_ranges,
		const SubstancesElementComposition& subs_element_composition,
		const SubstanceWeights& weights,
		const Composition& amounts)
	: parameters{parameters_}
	, number_of_elements{elements.size()}
	, number_of_substances{static_cast<size_t>(weights.size())}
{
	LOG()
	assert(number_of_substances == subs_element_composition.size());
	assert(number_of_substances == amounts.size());
	assert(number_of_substances == temp_ranges.size());



	switch(parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint: {
		items.reserve(1);
		auto temperature = Thermodynamics::ToKelvin(parameters.temperature_initial,
													parameters.temperature_initial_unit);
		items.push_back(OptimizationItem{parameters, elements, temp_ranges,
						subs_element_composition, weights, amounts,
						temperature});
	}
		break;
	case ParametersNS::Workmode::TemperatureRange: {
		auto temperatures = MakeTemperatureVector();
		items.reserve(temperatures.size());
		for(const auto& temperature : temperatures) {
			items.push_back(OptimizationItem{parameters, elements,
							temp_ranges, subs_element_composition,
							weights, amounts, temperature});
		}

	}
		break;
	case ParametersNS::Workmode::CompositionRange: {
		auto composition = MakeCompositionVector(parameters.composition1_range);
		auto temperature = Thermodynamics::ToKelvin(parameters.temperature_initial,
													parameters.temperature_initial_unit);

		std::vector<Composition> new_amounts; // = make_new_amounts
		items.reserve(new_amounts.size());
		for(const auto& new_amount : new_amounts) {
			items.push_back(OptimizationItem{parameters, elements,
							temp_ranges, subs_element_composition,
							weights, std::move(new_amount), temperature});
		}

	}
		break;
	case ParametersNS::Workmode::DoubleCompositionRange: {
		auto compositon1 = MakeCompositionVector(parameters.composition1_range);
		auto compositon2 = MakeCompositionVector(parameters.composition2_range);
		auto temperature = Thermodynamics::ToKelvin(parameters.temperature_initial,
													parameters.temperature_initial_unit);
		std::vector<Composition> new_amounts; // = make_new_amounts_2
		items.reserve(new_amounts.size());
		for(const auto& new_amount : new_amounts) {
			items.push_back(OptimizationItem{parameters, elements,
							temp_ranges, subs_element_composition,
							weights, std::move(new_amount), temperature});
		}


	}
		break;
	case ParametersNS::Workmode::TemperatureCompositionRange: {
		auto temperatures = MakeTemperatureVector();
		auto composition = MakeCompositionVector(parameters.composition1_range);
		std::vector<Composition> new_amounts; // = make_new_amounts
		items.reserve(new_amounts.size() * temperatures.size());
		for(const auto& temperature : temperatures) {
			for(const auto& new_amount : new_amounts) {
				items.push_back(OptimizationItem{parameters, elements,
								temp_ranges, subs_element_composition,
								weights, std::move(new_amount), temperature});
			}
		}

	}
		break;
	}

}

std::vector<double> OptimizationItemsMaker::MakeTemperatureVector()
{
	std::vector<double> temperature;
	RangeTabulator(parameters.temperature_range, temperature);
	std::transform(temperature.cbegin(), temperature.cend(),
				   temperature.begin(),
				   [tu = parameters.temperature_range_unit](double t){
		return Thermodynamics::ToKelvin(t, tu); });
	return temperature;
}

std::vector<double> OptimizationItemsMaker::MakeCompositionVector(
		ParametersNS::Range range)
{
	std::vector<double> composition;
	RangeTabulator(range, composition);
	return composition;
}

OptimizationItem::OptimizationItem(
		const ParametersNS::Parameters& parameters_,
		const std::vector<int>& elements_,
		const SubstancesTempRangeData& temp_ranges_,
		const SubstancesElementComposition& subs_element_composition_,
		const SubstanceWeights& weights_,
		const Composition& amounts_,
		const double initial_temperature_K)
	: parameters{parameters_}
	, elements{elements_}
	, temp_ranges{temp_ranges_}
	, subs_element_composition{subs_element_composition_}
	, weights{weights_}
	, amounts{amounts_}
	, temperature_K_initial{initial_temperature_K}
{
	temperature_K_current = temperature_K_initial;
	number_of_substances = weights.size();
	substances_id_order.resize(number_of_substances);
	n.resize(number_of_substances);
	c.resize(number_of_substances);
	ub_ini.resize(number_of_substances);
	ub_cur.resize(number_of_substances);
	constraints.resize(elements.size());


	// Order of substances changes every time when current temperature changes
	// then changes order in A matrix, i.e. needs to remake constraints vector




}

void OptimizationItem::DefineOrderOfSubstances()
{
	std::set<int> gas, liq, ind;
	for(const auto& [id, sub_temp_range] : temp_ranges) {
		auto tr = Thermodynamics::FindCoef(temperature_K_current, sub_temp_range);
		if(tr.phase == QStringLiteral("G")) {
			gas.insert(id);
		} else if(tr.phase == QStringLiteral("L")) {
			switch(parameters.liquid_solution) {
			case ParametersNS::LiquidSolution::No:
				ind.insert(id);
				break;
			case ParametersNS::LiquidSolution::One:
				liq.insert(id);
				break;
			}
		} else { // trange->phase == QStringLiteral("S")
			ind.insert(id);
		}
	}
	number_of_gases = gas.size();
	number_of_liquids = liq.size();
	number_of_individuals = ind.size();
	assert(number_of_gases + number_of_liquids + number_of_individuals ==
		   number_of_substances);
	substances_id_order.clear();
	auto back_ins = std::back_inserter(substances_id_order);
	std::copy(gas.cbegin(), gas.cend(), back_ins);
	std::copy(liq.cbegin(), liq.cend(), back_ins);
	std::copy(ind.cbegin(), ind.cend(), back_ins);
}

void OptimizationItem::MakeConstraints()
{
	// size = N * M
	for(auto&& constraint : constraints) {
		constraint.a_j.resize(number_of_substances);
	}
	size_t sub_id, el_id;
	for(size_t j = 0, maxj = elements.size(); j != maxj; ++j) {
		el_id = elements.at(j);
		for(size_t i = 0; i != number_of_substances; ++i) {
			sub_id = substances_id_order.at(i);
			constraints.at(j).a_j.at(i++) =
					subs_element_composition.at(sub_id).at(el_id);
		}
	}

	// fill b

}



} // namespace Optimization

