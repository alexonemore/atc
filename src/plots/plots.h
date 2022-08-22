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

#ifndef PLOTS_H
#define PLOTS_H

#include <unordered_map>

using GraphId = int;
static_assert(std::is_invocable_v<std::hash<GraphId>, GraphId>, "");

template<typename GraphPointer,
		 typename = std::void_t<decltype(std::hash<GraphId>{}(GraphId{}))>,
		 typename = std::enable_if_t<std::is_pointer_v<GraphPointer>>
		 >
using GraphMap = std::unordered_map<GraphId, GraphPointer>;

namespace Plot {
struct Range {
	double x_min{}, x_max{}, y_min{}, y_max{};
};
}

#endif // PLOTS_H
