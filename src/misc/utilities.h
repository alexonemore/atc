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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <chrono>
#include <QString>
#include <QObject>
#include <QThread>
#include <QMetaEnum>
#include <type_traits>
#include <random>
#ifndef NDEBUG
#include <QDebug>
#endif

#ifdef __GNUC__
	#define FUNCTIONNAME __PRETTY_FUNCTION__
#else
	#ifdef VERBOSE_DEBUG
		#define FUNCTIONNAME __FUNCSIG__
	#else
		#define FUNCTIONNAME __FUNCTION__
	#endif
#endif
#ifndef NDEBUG
	#ifdef __GNUC__
		#define LOG(...) {DebugLog(FUNCTIONNAME, QThread::currentThread(), #__VA_ARGS__, ##__VA_ARGS__);}
	#else
		#define LOG(...) {DebugLog(FUNCTIONNAME, QThread::currentThread(), ##__VA_ARGS__);}
	#endif
#else
	#define LOG(...) //{void(#__VA_ARGS__);}
#endif

#ifndef NDEBUG
template<typename ... Ts>
void DebugLog(const Ts& ... t)
{
	(qDebug() << ... << t);
}
#endif

void PrintAllProperties(const QObject* const obj);

class Timer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> tstart, tstop;

public:
	void start() { tstart = std::chrono::high_resolution_clock::now(); }
	void stop() { tstop = std::chrono::high_resolution_clock::now(); }
	auto milliseconds() {
		return std::chrono::duration_cast<std::chrono::milliseconds>
				(tstop - tstart).count();
	}
	auto seconds() {
		return std::chrono::duration_cast<std::chrono::seconds>
				(tstop - tstart).count();
	}
	auto minutes() {
		return std::chrono::duration_cast<std::chrono::minutes>
				(tstop - tstart).count();
	}
	auto hours() {
		return std::chrono::duration_cast<std::chrono::hours>
				(tstop - tstart).count();
	}
	QString duration();
};

template<typename T>
struct is_pair : std::false_type {};
template<typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};
template<typename T>
constexpr bool is_pair_v = is_pair<T>::value;

// templates for determining if the container contain std::pair
template<typename, typename = void>
struct is_mapping : std::false_type {};
template <typename Container>
struct is_mapping<Container, std::enable_if_t<is_pair_v<typename
		std::iterator_traits<typename Container::iterator>::value_type>>>
		: std::true_type {};
template <typename T>
constexpr bool is_mapping_v = is_mapping<T>::value;

template<typename T1, typename T2, typename ...TN,
	typename = std::void_t<decltype(std::declval<T1&>() == std::declval<T1&>()),
						   decltype(std::declval<T1&>() == std::declval<TN&>())...>>
constexpr bool EqualsAnyOf(const T1& t1, const T2& t2, const TN& ...tN)
{
	return ((t1 == t2) || ... || (t1 == tN));
}

template<typename T1, typename T2, typename ...TN,
	typename = std::void_t<decltype(std::declval<T1&>() == std::declval<T1&>()),
						   decltype(std::declval<T1&>() == std::declval<TN&>())...>>
constexpr bool EqualsAllOf(const T1& t1, const T2& t2, const TN& ...tN)
{
	return ((t1 == t2) && ... && (t1 == tN));
}


template<typename Distribution,
		 typename Type = typename Distribution::result_type>
class RandomNumberImpl
{
	std::default_random_engine re;
	Distribution dist;
public:
	explicit RandomNumberImpl(const Type low, const Type high,
				 const unsigned int random_number = std::random_device{}())
		: re{random_number}, dist{low, high}
	{}
	Type operator()() { return dist(re); }
	void seed(const unsigned int random_number = std::random_device{}()) {
		re.seed(random_number);
	}
};

using RandomInt = RandomNumberImpl<std::uniform_int_distribution<>>;
using RandomDouble = RandomNumberImpl<std::uniform_real_distribution<>>;


class QColor;
QColor GetRandomColor();


template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
QStringList MetaEnumToQStringList() {
	auto me = QMetaEnum::fromType<T>();
	auto max = me.keyCount();
	QStringList list;
	for(int i = 0; i != max; ++i) {
		list.append(me.valueToKey(me.value(i)));
	}
	return list;
}

#endif // UTILITIES_H
