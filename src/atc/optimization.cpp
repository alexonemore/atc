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

#include "optimization.h"
#include "thermodynamics.h"

#ifndef NDEBUG
std::atomic_int32_t i_maker{0};
std::atomic_int32_t i_items{0};
#endif

#if __MINGW32__ && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define transform_reduce inner_product
#endif

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

static double ThermodinamicFunction(const std::vector<double>& n,
									std::vector<double>& grad, void* data)
{
	using DT = std::vector<double>::difference_type;
	const OptimizationItem* tcn = reinterpret_cast<const OptimizationItem*>(data);
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
static double ThermodinamicFunctionMinus(const std::vector<double>& n,
									std::vector<double>& grad, void* data)
{
	return -ThermodinamicFunction(n, grad, data);
}

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

auto OptimizationItemsMaker::MakeNewAmounts(const Composition& amounts,
											const SubstanceWeights& weights)
{
	sum = SumCompositionMolAndGram(amounts);
	std::vector<double> composition;
	if(sum.group_2_mol > 0.0) {
		composition = MakeCompositionVector();
	} else {
		LOG("Variable composition is empty, calculate only 1 element")
		composition.push_back(0.0);
	}
	std::vector<Composition> new_amounts(composition.size());
	std::transform(composition.cbegin(), composition.cend(), new_amounts.begin(),
				   [&](auto&& val){return MakeNewAmount(amounts, weights, val);});
	assert(new_amounts.size() == composition.size());
	assert(new_amounts.size() > 0);
	return std::make_pair(composition, new_amounts);
}

OptimizationItemsMaker::OptimizationItemsMaker(
		const ParametersNS::Parameters& parameters_,
		const std::vector<int>& elements,
		const SubstancesTempRangeData& temp_ranges,
		const SubstancesElementComposition& subs_element_composition,
		const SubstanceWeights& weights,
		const Composition& amounts)
	: parameters{parameters_}
	, number_of_substances{static_cast<size_t>(weights.size())}
{
	LOG(i = ++i_maker)
	assert(number_of_substances == subs_element_composition.size());
	assert(number_of_substances == amounts.size());
	assert(number_of_substances == temp_ranges.size());

	switch(parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint: {
		items.reserve(1);
		auto temperature = Thermodynamics::ToKelvin(parameters.temperature_initial,
													parameters.temperature_initial_unit);
		x_size = 1;
		y_size = 1;
		items.emplace_back(parameters, elements, temp_ranges,
						   subs_element_composition, weights, amounts,
						   temperature);
	}
		break;
	case ParametersNS::Workmode::TemperatureRange: {
		auto temperatures = MakeTemperatureVector();
		x_size = temperatures.size();
		y_size = 1;
		items.reserve(x_size);
		for(const auto& temperature : temperatures) {
			items.emplace_back(parameters, elements,
							   temp_ranges, subs_element_composition,
							   weights, amounts, temperature);
		}
	}
		break;
	case ParametersNS::Workmode::CompositionRange: {
		auto temperature = Thermodynamics::ToKelvin(parameters.temperature_initial,
													parameters.temperature_initial_unit);
		auto [composition, new_amounts] = MakeNewAmounts(amounts, weights);
		x_size = new_amounts.size();
		y_size = 1;
		items.reserve(x_size);
		// std::transform makes copy, emplace_back doesn't
		size_t i = 0;
		for(const auto& new_amount : new_amounts) {
			items.emplace_back(parameters, elements,
							   temp_ranges, subs_element_composition,
							   weights, new_amount, temperature,
							   composition.at(i++));
		}

	}
		break;
	case ParametersNS::Workmode::TemperatureCompositionRange: {
		auto temperatures = MakeTemperatureVector();
		auto [composition, new_amounts] = MakeNewAmounts(amounts, weights);
		x_size = temperatures.size();
		y_size = new_amounts.size();
		items.reserve(x_size * y_size);
		for(const auto& temperature : temperatures) {
			size_t i = 0;
			for(const auto& new_amount : new_amounts) {
				items.emplace_back(parameters, elements,
								   temp_ranges, subs_element_composition,
								   weights, new_amount, temperature,
								   composition.at(i++));
			}
		}
	}
		break;
	}
}

#ifndef NDEBUG
OptimizationItemsMaker::~OptimizationItemsMaker()
{
	LOG(i, "/", i_maker)
}
#endif

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

std::vector<double> OptimizationItemsMaker::MakeCompositionVector()
{
	std::vector<double> composition;
	Thermodynamics::RangeTabulator(parameters.composition_range, composition);
	return composition;
}

OptimizationItem::OptimizationItem(
		const ParametersNS::Parameters& parameters_,
		const std::vector<int>& elements_,
		const SubstancesTempRangeData& temp_ranges_,
		const SubstancesElementComposition& subs_element_composition_,
		const SubstanceWeights& weights_,
		const Composition& amounts_,
		const double initial_temperature_K,
		const double variable_composition)
	: parameters{parameters_}
	, elements{elements_}
	, temp_ranges{temp_ranges_}
	, subs_element_composition{subs_element_composition_}
	, weights{weights_}
	, amounts{amounts_}
	, temperature_K_initial{initial_temperature_K}
	, temperature_K_current{initial_temperature_K}
	, composition_variable{variable_composition}
{
	LOG(i = ++i_items)
	number.elements = elements.size();
	number.substances = weights.size();
	substances_id_order.resize(number.substances);
	n.resize(number.substances);
	c.resize(number.substances);
	ub.resize(number.substances);
	constraints.resize(number.elements);
	for(auto&& constraint : constraints) {
		constraint.a_j.resize(number.substances);
	}
	// Do not resize anything later

	// TODO change initial data in this class for shared_ptr

	// Order of substances changes every time when current temperature changes
	// then changes order in A matrix, i.e. needs to remake constraints vector.

	// When extrapolation is disabled, the maximum value for the substance
	// that does not exist at the current temperature is zero.

}

#ifndef NDEBUG
OptimizationItem::~OptimizationItem()
{
	LOG(i, "/", i_items)
}
#endif

void OptimizationItem::Calculate()
{
	LOGV()
	MakeConstraintsB(); // vector B depends on amounts
	H_kJ_Initial();

	switch(parameters.target) {
	case ParametersNS::Target::Equilibrium:
		Equilibrium(temperature_K_initial);
		H_current = H_kJ_Current();
		break;
	case ParametersNS::Target::AdiabaticTemperature:
		AdiabaticTemperature();
		break;
	}

	MakeAmountsOfEquilibrium();
	sum_of_initial = GetSumAndRecalculate(amounts);
}

void OptimizationItem::DefineOrderOfSubstances()
{
	std::set<int> gas, liq, ind;
	for(const auto& [id, sub_temp_range] : temp_ranges) {
		auto&& tr = Thermodynamics::FindCoef(temperature_K_current, sub_temp_range);
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
	number.gases = gas.size();
	number.liquids = liq.size();
	number.individuals = ind.size();
	assert(number.gases + number.liquids + number.individuals ==
		   number.substances);
	substances_id_order.clear();
	auto back_ins = std::back_inserter(substances_id_order);
	std::copy(gas.cbegin(), gas.cend(), back_ins);
	std::copy(liq.cbegin(), liq.cend(), back_ins);
	std::copy(ind.cbegin(), ind.cend(), back_ins);
}

void OptimizationItem::MakeConstraintsMatrixA()
{
	// It depends on order of substances
	// size = N * M
	size_t j = 0;
	for(auto&& element_id : elements) {
		auto&& a_j = constraints.at(j++).a_j;
		size_t i = 0;
		for(auto&& substance_id : substances_id_order) {
			const auto& sub = subs_element_composition.at(substance_id);
			auto element_it = sub.find(element_id);
			a_j.at(i++) = (element_it == sub.cend()) ? 0.0 : element_it->second;
		}
	}
}

void OptimizationItem::MakeConstraintsB()
{
	std::unordered_map<int, double> el_id_amount;
	for(const auto& [sub_id, el_cmp] : subs_element_composition) {
		auto sub_mol = amounts.at(sub_id).sum_mol;
		for(const auto& [el_id, el_value] : el_cmp) {
			el_id_amount[el_id] += sub_mol * el_value;
		}
	}
	size_t el_id;
	for(size_t j = 0; j != number.elements; ++j) {
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
						   c.begin(), [this](const int id){
				return Thermodynamics::Thermo::TF_c(temperature_K_current,
													temp_ranges.at(id));});
			break;
		case ParametersNS::Database::HSC:
			std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
						   c.begin(), [this](const int id){
				return Thermodynamics::HSC::TF_c(temperature_K_current,
												 temp_ranges.at(id));});
			break;
		}
		break;
	case ParametersNS::MinimizationFunction::Entropy:
		// TODO Replace thermodynamic functions with correct ones
		// TF_S_J is not correct
		switch(parameters.database) {
		case ParametersNS::Database::Thermo:
			std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
						   c.begin(), [this](const int id){
				return Thermodynamics::Thermo::TF_S_J(temperature_K_current,
													  temp_ranges.at(id));});
			break;
		case ParametersNS::Database::HSC:
			std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
						   c.begin(), [this](const int id){
				return Thermodynamics::HSC::TF_S_J(temperature_K_current,
												   temp_ranges.at(id));});
			break;
		}
		break;
	}
}

void OptimizationItem::MakeUB()
{
	// depends on constraints
	std::fill(ub.begin(), ub.end(), std::numeric_limits<double>::max());
	for(const auto& constraint : constraints) {
		std::transform(constraint.a_j.cbegin(), constraint.a_j.cend(),
					   ub.cbegin(), ub.begin(),
					   [bj = constraint.b_j](auto aji, auto ubi){
			if(aji > 0) {
				auto tmp = bj / aji;
				return tmp < ubi ? tmp : ubi;
			} else {
				return ubi;
			}
		});
	}

	// check for extrapolation and zeroize if it not exist
	switch(parameters.extrapolation) {
	case ParametersNS::Extrapolation::Disable:
		std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
					   ub.cbegin(), ub.begin(),
					   [this](const int sub_id, const double ubi){
			return IsExistAtCurrentTemperature(sub_id) ? ubi : 0.0;
		});
		break;
	case ParametersNS::Extrapolation::Enable:
		break;
	}
}

void OptimizationItem::MakeN()
{
	std::transform(ub.cbegin(), ub.cend(), n.begin(), [](double n){
		return n / 2;
	});
}

void OptimizationItem::Equilibrium()
{
	DefineOrderOfSubstances();
	MakeConstraintsMatrixA();
	MakeUB(); // extrapolation is taken into account here
	MakeC(); // depends on current temperature
	MakeN(); // set to half of ub

	nlopt::result result;
	result_of_optimization = Minimize(nlopt::LD_SLSQP, result);
	if(result == nlopt::XTOL_REACHED) return;
	result_of_optimization = Minimize(nlopt::LD_AUGLAG_EQ, result);
	if(result == nlopt::XTOL_REACHED) return;
	result_of_optimization = Minimize(nlopt::LD_SLSQP, result);
}

void OptimizationItem::Equilibrium(const double temperature_K)
{
	temperature_K_current = temperature_K;
	Equilibrium();
}

void OptimizationItem::AdiabaticTemperature()
{
	double T_min = 298.15;
	double T_max = 10000;
	double T_cur;
	Equilibrium(T_min);
	H_current = H_kJ_Current();
	if(H_initial < H_current) {
		return;
	}
	Equilibrium(T_max);
	H_current = H_kJ_Current();
	if(H_initial > H_current) {
		return;
	}
	T_cur = (T_min + T_max) / 2;
	Equilibrium(T_cur);
	H_current = H_kJ_Current();
	double at_epsilon = std::pow(10, -parameters.at_accuracy)/2;
#if !defined(NDEBUG) && defined(VERBOSE_DEBUG)
	int n = 0;
#endif
	while((std::abs(T_max - T_min) > at_epsilon))
	{
		if(H_initial < H_current) {
			T_max = T_cur;
		} else {
			T_min = T_cur;
		}
		T_cur = (T_min + T_max) / 2;
		Equilibrium(T_cur);
		H_current = H_kJ_Current();

#if !defined(NDEBUG) && defined(VERBOSE_DEBUG)
		qDebug() << Qt::fixed << qSetRealNumberPrecision(10) << ++n
				 << "H_current:" << H_current << "\tT_cur" << T_cur
				 << "\tdelta_H:" << std::abs(H_current - H_initial)
				 << "\tdelta_T:" << std::abs(T_max - T_min);
#endif
	}
}

void OptimizationItem::H_kJ_Initial()
{
	auto H = [this](const int id){
		switch(parameters.database) {
		case ParametersNS::Database::Thermo:
			return Thermodynamics::Thermo::TF_H_kJ(temperature_K_initial,
												   temp_ranges.at(id));
		case ParametersNS::Database::HSC:
			return Thermodynamics::HSC::TF_H_kJ(temperature_K_initial,
												temp_ranges.at(id));
		default:
			throw std::logic_error("default in switch");
		}
	};

	switch(parameters.H_initial_by) {
	case ParametersNS::H_Initial_By::AsChecked:
		H_initial = std::accumulate(amounts.cbegin(), amounts.cend(), double{0.0},
									[&H](double sum, decltype(amounts)::const_reference amount){
			if(amount.second.sum_mol > 0.0) {
				return sum + H(amount.first) * amount.second.sum_mol;
			} else {
				return sum;
			}
		});
		break;
	case ParametersNS::H_Initial_By::ByMinimumGibbsEnergy: {
		auto G = [this](const int id){
			switch(parameters.database) {
			case ParametersNS::Database::Thermo:
				return Thermodynamics::Thermo::TF_G_kJ(temperature_K_initial,
													   temp_ranges.at(id));
			case ParametersNS::Database::HSC:
				return Thermodynamics::HSC::TF_G_kJ(temperature_K_initial,
													temp_ranges.at(id));
			default:
				throw std::logic_error("default in switch");
			}
		};
		double H_sum{0.0};
		for(const auto& [sub_id, amount] : amounts) {
			if(amount.sum_mol <= 0.0) continue;
			double G_min = G(sub_id);
			double G_tmp;
			int sub_id_G_min = sub_id;
			const auto& sub_cmp_cur = subs_element_composition.at(sub_id);
			for(const auto& [sub_id_n, sub_cmp_n] :	subs_element_composition) {
				if(sub_id_n == sub_id) continue;
				if(sub_cmp_cur == sub_cmp_n) {
					G_tmp = G(sub_id_n);
					if(G_tmp < G_min) {
						G_min = G_tmp;
						sub_id_G_min = sub_id_n;
					}
				}
			}
			H_sum += H(sub_id_G_min) * amount.sum_mol;
		}
		H_initial = H_sum;
	}
		break;
	default:
		throw std::logic_error("default in switch");
	}
}

double OptimizationItem::H_kJ_Current()
{
	return std::transform_reduce(n.cbegin(), n.cend(),
								 substances_id_order.cbegin(), double{0.0},
								 std::plus<>(),
								 [this](const double ni, const int id){
		switch(parameters.database) {
		case ParametersNS::Database::Thermo:
			return ni * Thermodynamics::Thermo::TF_H_kJ(temperature_K_current,
														temp_ranges.at(id));
		case ParametersNS::Database::HSC:
			return ni * Thermodynamics::HSC::TF_H_kJ(temperature_K_current,
													 temp_ranges.at(id));
		default:
			throw std::logic_error("default in switch");
		}
	});
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

double OptimizationItem::Minimize(const nlopt::algorithm algorithm,
								  nlopt::result& result)
{
	double minf;
	nlopt::opt opt(algorithm, static_cast<unsigned>(number.substances));
	opt.set_lower_bounds(0);
	opt.set_upper_bounds(ub);
	switch(parameters.minimization_function) {
	case ParametersNS::MinimizationFunction::GibbsEnergy:
		opt.set_min_objective(Optimization::ThermodinamicFunction, this);
		break;
	case ParametersNS::MinimizationFunction::Entropy:
		opt.set_min_objective(Optimization::ThermodinamicFunctionMinus, this);
		break;
	}
	for(size_t j = 0; j < number.elements; ++j) {
		opt.add_equality_constraint(Optimization::ConstraintFunction,
						&constraints[j], Optimization::epsilon_accuracy);
	}
	// Using ftol's results in a noisy graph.
	/* bug in nlopt:
	 * Class nlopt::opt has private struct nlopt_opt which has field
	 * double *xtol_abs.
	 * Nlopt library does not allocate memory for xtol_abs array by default.
	 * When the NLOPT_LD_AUGLAG_EQ algorithm is used, the nlopt library creates
	 * a local_opt instance of nlopt_opt struct for local optimization and uses
	 * the memcpy function to copy some data from the primary opt to local_opt.
	 * And memcpy try to copy from null pointer xtol_abs to another.
	 * Since xtol_abs is not allocated in primary opt, the program crashes.
	 * File optimize.c line 755 in function nlopt_optimize_ then
	 * file option.c line 698 in function nlopt_set_xtol_abs.
	 * Solution: use the set_xtol_abs function for the primary opt instance,
	 * which will allocate the necessary memory.
	 */
	opt.set_xtol_abs(Optimization::epsilon_accuracy);
	opt.set_xtol_rel(Optimization::epsilon_accuracy);
	opt.set_maxtime(Optimization::maxtime_of_minimize);

	try {
		result = opt.optimize(n, minf);
	}
	catch(std::exception &e) {
#if !defined(NDEBUG)
		qDebug() << "********************************************************";
		qDebug() << "NLopt failed:" << e.what();
		qDebug() << "Result:" << Optimization::NLoptResultToString(result) << result;
		qDebug() << "Algorithm:" << opt.get_algorithm() << opt.get_algorithm_name();
		qDebug() << "minf:" << minf;
		qDebug() << "Error message:" << opt.get_errmsg();
		qDebug() << "Numevals:" << opt.get_numevals();
		qDebug() << "Maxeval:" << opt.get_maxeval();
		qDebug() << "last_optimize_result:" << opt.last_optimize_result() <<
					Optimization::NLoptResultToString(opt.last_optimize_result());
		qDebug() << "last_optimum_value:" << opt.last_optimum_value();
		qDebug() << "num_params:" << opt.num_params();
		qDebug() << "********************************************************";
#endif
		/* bug in nlopt:
		 * Sometimes the result is out of enum values.
		 */
		result = nlopt::FAILURE;
	}
	return minf;
}

void OptimizationItem::MakeAmountsOfEquilibrium()
{
	assert(std::is_sorted(weights.cbegin(), weights.cend(),
						  [](const SubstanceWeight& lhs, const SubstanceWeight& rhs){
			   return lhs.id < rhs.id; }));
	struct EquilibriumComposition {
		int id;
		double mol;
	};
	std::vector<EquilibriumComposition> vec_ec{number.substances};
	std::transform(substances_id_order.cbegin(), substances_id_order.cend(),
				   n.cbegin(), vec_ec.begin(), [](auto&& sub_id, auto&& n_i){
		return EquilibriumComposition{sub_id, n_i};});
	std::sort(vec_ec.begin(), vec_ec.end(), [](auto&& lhs, auto&& rhs){
		return lhs.id < rhs.id;
	});
	assert(vec_ec.size() == weights.size());
	size_t i = 0;
	for(auto&& weight : weights) {
		auto [id, mol] = vec_ec.at(i++);
		assert(id == weight.id);
		auto w = weight.weight;
		auto gram = mol * w;
		amounts_of_equilibrium[id] = Amounts{mol, gram, 0.0, 0.0, mol, gram, 0.0, 0.0};
	}
	sum_of_equilibrium = GetSumAndRecalculate(amounts_of_equilibrium);
}

Composition OptimizationItemsMaker::MakeNewAmount(const Composition& amounts,
			const SubstanceWeights& weights, const double value)
{
	// Group 1 - main composition
	// Group 2 - variable composition
	Composition new_amount{amounts};
	switch(parameters.composition_range_unit) {
	case ParametersNS::CompositionUnit::AtomicPercent: {
		if(sum.group_1_mol > 0.0 && sum.group_2_mol > 0.0) {
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
			GetSumAndRecalculate(new_amount);
		}
	}
		break;
	case ParametersNS::CompositionUnit::WeightPercent: {
		if(sum.group_1_gram > 0.0 && sum.group_2_gram > 0.0) {
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
			GetSumAndRecalculate(new_amount);
		}
	}
		break;
	case ParametersNS::CompositionUnit::Mol: {
		if(sum.group_2_mol > 0.0) {
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
			GetSumAndRecalculate(new_amount);
		}
	}
		break;
	case ParametersNS::CompositionUnit::Gram: {
		if(sum.group_2_gram > 0.0) {
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
			GetSumAndRecalculate(new_amount);
		}
	}
		break;
	}
	return new_amount;
}

} // namespace Optimization

