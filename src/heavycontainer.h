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

#ifndef HEAVYCONTAINER_H
#define HEAVYCONTAINER_H

#include "utilities.h"

namespace HeavyContainerNamespace {
extern std::atomic_int counter;
}

class HeavyContainer
{
	int i{0};
public:
	HeavyContainer() : i{HeavyContainerNamespace::counter++} {LOG("ctor", i) }
	HeavyContainer(const HeavyContainer&) {LOG("copy ctor")}
	HeavyContainer(HeavyContainer&&) noexcept {LOG("move ctor")}
	void operator=(const HeavyContainer&) {LOG("copy =")}
	void operator=(HeavyContainer&&) noexcept {LOG("move =")}
	~HeavyContainer() {LOG(i)}

	int GetValue() const noexcept {LOG() return i;}
	int GetCounter() const {LOG() return HeavyContainerNamespace::counter;}
	void SetCounter(const int value) {LOG() HeavyContainerNamespace::counter = value;}

	void HeavyCalculations() {
		LOG(i)
		QThread::sleep(1);
		i *= 10;
	}
};

#endif // HEAVYCONTAINER_H
