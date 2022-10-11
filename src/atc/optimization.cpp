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
#include "utilities.h"
#include <nlopt.hpp>

namespace Optimization {
static double Log_eps(const double x) // noexcept
{
	return ((std::log(x*x + Constants::epsilon_log)) / 2);
}
static double ConstraintFunction(const std::vector<double>& x,
								 std::vector<double>& grad, void* data)
{
	const Constraint* cnt = reinterpret_cast<const Constraint*>(data);
	if(!grad.empty()) {
		assert(cnt->a_j->size() == grad.size() && "grad.size error");
		std::copy(cnt->a_j->cbegin(), cnt->a_j->cend(), grad.begin());
	}
	assert(x.size() == cnt->a_j->size() && "x.size error");
	return std::transform_reduce(x.cbegin(), x.cend(), cnt->a_j->cbegin(),
								 -cnt->b_j); /* minus of bj */
}
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


} // namespace Optimization
