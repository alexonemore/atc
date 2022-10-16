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
		auto temperature = Thermodynamics::ToKelvin(parameters.temperature_initial,
													parameters.temperature_initial_unit);
		std::vector<Composition> new_amounts = MakeNewAmounts(amounts, weights);
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
		std::vector<Composition> new_amounts = MakeNewAmounts(amounts, weights);
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
	Thermodynamics::RangeTabulator(parameters.temperature_range, temperature);
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
	Thermodynamics::RangeTabulator(range, composition);
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
	for(auto&& constraint : constraints) {
		constraint.a_j.resize(number_of_substances);
	}

	// Order of substances changes every time when current temperature changes
	// then changes order in A matrix, i.e. needs to remake constraints vector

	FillB(); // vector B depends on amounts



}

void OptimizationItem::Calculate()
{
	switch(parameters.target) {
	case ParametersNS::Target::Equilibrium:
		AdiabaticTemperature();
		break;
	case ParametersNS::Target::AdiabaticTemperature:
		Equilibrium(temperature_K_initial);
		break;
	}
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
	// It depends on order of substances
	// size = N * M
	size_t sub_id, el_id;
	for(size_t j = 0, maxj = elements.size(); j != maxj; ++j) {
		el_id = elements.at(j);
		auto&& a_j = constraints.at(j).a_j;
		for(size_t i = 0; i != number_of_substances; ++i) {
			sub_id = substances_id_order.at(i);
			a_j.at(i) = subs_element_composition.at(sub_id).at(el_id);
		}
	}
}

void OptimizationItem::FillB()
{
	std::unordered_map<int, double> el_id_amount;
	for(const auto& [sub_id, el_cmp] : subs_element_composition) {
		auto sub_mol = amounts.at(sub_id).sum_mol;
		for(const auto& [el_id, el_value] : el_cmp) {
			el_id_amount[el_id] += sub_mol * el_value;
		}
	}
	size_t el_id;
	for(size_t j = 0, maxj = elements.size(); j != maxj; ++j) {
		el_id = elements.at(j);
		constraints.at(j).b_j = el_id_amount.at(el_id);
	}
}

void OptimizationItem::MakeC()
{
	// depends on substances_id_order
	switch(parameters.minimization_function) {
	case ParametersNS::MinimizationFunction::GibbsEnergy:
		switch(parameters.database) {
		case ParametersNS::Database::Thermo:
			std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
						   c.begin(), [this](int id){
				return Thermodynamics::Thermo::TF_c(temperature_K_current,
													temp_ranges.at(id));});
			break;
		case ParametersNS::Database::HSC:
			std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
						   c.begin(), [this](int id){
				return Thermodynamics::HSC::TF_c(temperature_K_current,
												 temp_ranges.at(id));});
			break;
		}
		break;
	case ParametersNS::MinimizationFunction::Entropy:
		switch(parameters.database) {
		case ParametersNS::Database::Thermo:
			std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
						   c.begin(), [this](int id){
				return Thermodynamics::Thermo::TF_S_J(temperature_K_current,
													  temp_ranges.at(id));});
			break;
		case ParametersNS::Database::HSC:
			std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
						   c.begin(), [this](int id){
				return Thermodynamics::HSC::TF_S_J(temperature_K_current,
												   temp_ranges.at(id));});
			break;
		}
		break;
	}
}

void OptimizationItem::MakeUBini()
{
	// depends on constraints
	ub_ini.resize(number_of_substances, std::numeric_limits<double>::max());
	for(const auto& cnti : constraints) {
		std::transform(cnti.a_j.cbegin(), cnti.a_j.cend(), ub_ini.cbegin(),
					   ub_ini.begin(), [bj = cnti.b_j](auto aji, auto ubi){
			if(aji > 0) {
				auto tmp = bj / aji;
				return tmp < ubi ? tmp : ubi;
			} else {
				return ubi;
			}
		});
	}
}

void OptimizationItem::MakeUBcur()
{
	// depends on ub_ini
	switch(parameters.extrapolation) {
	case ParametersNS::Extrapolation::Disable:
		std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
					   ub_ini.cbegin(), ub_cur.begin(),
					   [this](const int sub_id, const double ubi){
			return IsExistAtCurrentTemperature(sub_id) ? ubi : 0.0;
		});
		break;
	case ParametersNS::Extrapolation::Enable:
		std::copy(ub_ini.cbegin(), ub_ini.cend(), ub_cur.begin());
		break;
	}
}

void OptimizationItem::MakeN()
{
	std::transform(ub_cur.cbegin(), ub_cur.cend(), n.begin(), [](double n){
		return n / 2;
	});
}

void OptimizationItem::Equilibrium()
{

}

void OptimizationItem::Equilibrium(const double temperature_K)
{

}

void OptimizationItem::AdiabaticTemperature()
{

}

double OptimizationItem::H_kJ_Initial()
{

}

double OptimizationItem::H_kJ_Current()
{

}

bool OptimizationItem::IsExistAtCurrentTemperature(const int sub_id)
{
	auto&& temp_range = temp_ranges.at(sub_id);
	auto min = temp_range.cbegin()->T_min;
	auto max = temp_range.crbegin()->T_max;
	if(min <= temperature_K_current && temperature_K_current <= max) {
		return true;
	} else {
		return false;
	}
}

Composition OptimizationItemsMaker::MakeNewAmount(const Composition& amounts,
			const SubstanceWeights& weights, const double value)
{
	// Group 1 - main composition
	// Group 2 - variable composition
	Composition new_amount;
	switch(parameters.composition_range_unit) {
	case ParametersNS::CompositionUnit::AtomicPercent: {
		if(sum.group_1_mol > 0.0 && sum.group_2_mol > 0.0) {
			new_amount = amounts;
			auto new_sum_group2 = sum.sum_mol * value / 100;
			auto new_sum_group1 = sum.sum_mol - new_sum_group2;
			auto coef1 = new_sum_group1 / sum.group_1_mol;
			auto coef2 = new_sum_group2 / sum.group_2_mol;
			for(const auto& weight : weights) {
				auto id = weight.id;
				auto w = weight.weight;
				auto&& new_amount_at = new_amount.at(id);
				new_amount_at.group_1_mol *= coef1;
				new_amount_at.group_1_gram = new_amount_at.group_1_mol * w;
				new_amount_at.group_2_mol *= coef2;
				new_amount_at.group_2_gram = new_amount_at.group_2_mol * w;
				new_amount_at.sum_mol = new_amount_at.group_1_mol +
						new_amount_at.group_2_mol;
				new_amount_at.sum_gram = new_amount_at.group_1_gram +
						new_amount_at.group_2_gram;
			}
			SumRecalculate(new_amount);
		}
	}
		break;
	case ParametersNS::CompositionUnit::WeightPercent: {
		if(sum.group_1_gram > 0.0 && sum.group_2_gram > 0.0) {
			new_amount = amounts;
			auto new_sum_group2 = sum.sum_gram * value / 100;
			auto new_sum_group1 = sum.sum_gram - new_sum_group2;
			auto coef1 = new_sum_group1 / sum.group_1_gram;
			auto coef2 = new_sum_group2 / sum.group_2_gram;
			for(const auto& weight : weights) {
				auto id = weight.id;
				auto w = weight.weight;
				auto&& new_amount_at = new_amount.at(id);
				new_amount_at.group_1_gram *= coef1;
				new_amount_at.group_1_mol = new_amount_at.group_1_gram / w;
				new_amount_at.group_2_gram *= coef2;
				new_amount_at.group_2_mol = new_amount_at.group_2_gram / w;
				new_amount_at.sum_mol = new_amount_at.group_1_mol +
						new_amount_at.group_2_mol;
				new_amount_at.sum_gram = new_amount_at.group_1_gram +
						new_amount_at.group_2_gram;
			}
			SumRecalculate(new_amount);
		}
	}
		break;
	case ParametersNS::CompositionUnit::Mol: {
		if(sum.group_2_mol > 0.0) {
			new_amount = amounts;
			auto coef = value / sum.group_2_mol;
			for(const auto& weight : weights) {
				auto id = weight.id;
				auto w = weight.weight;
				auto&& new_amount_at = new_amount.at(id);
				new_amount_at.group_2_mol *= coef;
				new_amount_at.group_2_gram = new_amount_at.group_2_mol * w;
				new_amount_at.sum_mol = new_amount_at.group_1_mol +
						new_amount_at.group_2_mol;
				new_amount_at.sum_gram = new_amount_at.group_1_gram +
						new_amount_at.group_2_gram;
			}
			SumRecalculate(new_amount);
		}
	}
		break;
	case ParametersNS::CompositionUnit::Gram: {
		if(sum.group_2_gram > 0.0) {
			new_amount = amounts;
			auto coef = value / sum.group_2_gram;
			for(const auto& weight : weights) {
				auto id = weight.id;
				auto w = weight.weight;
				auto&& new_amount_at = new_amount.at(id);
				new_amount_at.group_2_gram *= coef;
				new_amount_at.group_2_mol = new_amount_at.group_2_gram / w;
				new_amount_at.sum_mol = new_amount_at.group_1_mol +
						new_amount_at.group_2_mol;
				new_amount_at.sum_gram = new_amount_at.group_1_gram +
						new_amount_at.group_2_gram;
			}
			SumRecalculate(new_amount);
		}
	}
		break;
	}
	return new_amount;
}

std::vector<Composition> OptimizationItemsMaker::MakeNewAmounts(
		const Composition& amounts, const SubstanceWeights& weights)
{
	auto composition = MakeCompositionVector(parameters.composition_range);
	sum = SumComposition(amounts);
	std::vector<Composition> new_amounts;
	for(auto&& val : composition) {
		new_amounts.emplace_back(MakeNewAmount(amounts, weights, val));
	}
	return new_amounts;
}

} // namespace Optimization

