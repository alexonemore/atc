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

#ifndef PLOTS_H
#define PLOTS_H

#include <unordered_map>

struct GraphIdImpl {
	int substance_id{0};
	int option{0};
	int database{0};
};

bool operator==(const GraphIdImpl& lhs, const GraphIdImpl& rhs);

namespace std {
template<> struct hash<GraphIdImpl> {
	std::size_t operator()(const GraphIdImpl& rhs) const noexcept {
		constexpr int n = 10;
		return std::hash<int>{}((rhs.substance_id * n +
			static_cast<int>(rhs.option)) * n +
			static_cast<int>(rhs.database));
	}
};
}

using GraphId = GraphIdImpl;
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
