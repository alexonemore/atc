/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2022 Alexandr Shchukin
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

#include "utilities.h"
#include <QString>
#include <QColor>
#include <QMetaProperty>
#include <unordered_map>
#include <vector>
#include <set>

namespace static_tests {
static_assert(is_mapping_v<std::unordered_map<int, double>>, "");
static_assert(is_mapping_v<std::vector<std::pair<int, double>>>, "");
static_assert(!is_mapping_v<std::vector<int>>, "");
static_assert(is_mapping_v<std::set<std::pair<int, float>>>, "");
static_assert(!is_mapping_v<std::set<int>>, "");
using vt = typename std::iterator_traits<decltype(std::declval<
			std::unordered_map<int, double>>().cbegin())>::value_type;
static_assert(is_pair_v<vt>, "is_pair");
static_assert(EqualsAnyOf(1, 4, 1.0, 5.7), "");
static_assert(EqualsAnyOf(1, 4, 's', 1.0f), "");
static_assert(EqualsAllOf(1, 1, 1, 1.0, 1.0f), "");
static_assert(!EqualsAllOf(1, 1, 1, 9, 1), "");
static_assert(!EqualsAllOf(1, 1, 1, 1, '1'), "");
}

QString Timer::duration()
{
	auto mt = minutes();
	auto st = seconds();
	auto h = hours();
	auto m = mt - h * 60;
	auto s = st - mt * 60;
	auto ms = milliseconds() - st * 1000;
	QString dur;
	if(h > 0) { dur += QString::number(h) + " h"; }
	if(m > 0) { dur += " " + QString::number(m) + " m"; }
	if(s > 0) { dur += " " + QString::number(s) + " s "; }
	dur += QString::number(ms) + " ms";
	return dur;
}

void PrintAllProperties(const QObject* const obj)
{
#ifndef NDEBUG
	const QMetaObject* metaobject = obj->metaObject();
	int count = metaobject->propertyCount();
	for(int i{0}; i < count; ++i) {
		QMetaProperty metaproperty = metaobject->property(i);
		const char* name = metaproperty.name();
		QVariant value = obj->property(name);
		qDebug() << name << ": " << value.toString();
	}
#else
	Q_UNUSED(obj)
#endif
}

constexpr static unsigned int random_numbers[] {923, 336, 326};

QColor GetRandomColor()
{
	static RandomInt hue(35, 90, random_numbers[0]); // 0 - 359
	static RandomInt sat(180, 255, random_numbers[1]); // 0 - 255
	static RandomInt val(150, 255, random_numbers[2]); // 0 - 255
	static int nonrandhue = 300;
	QColor color;
	nonrandhue += hue();
	if(nonrandhue > 359) nonrandhue -= 359;
	color.setHsv(nonrandhue, sat(), val());
	return color;
}

