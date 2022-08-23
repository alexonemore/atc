#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QObject>
#include <QMetaEnum>

namespace ParametersNS {
	Q_NAMESPACE
enum class Workmode {
	SinglePoint,
	TemperatureRange,
//	PressureRange, TODO
	CompositionRange,
	DoubleCompositionRange,
	TemperatureCompositionRange
};
Q_ENUM_NS(Workmode)
enum class Target {
	AdiabaticTemperature,
	Equilibrium
};
Q_ENUM_NS(Target)
enum class LiquidSolution {
	NoLiquidSolution,
	OneLiquidSolution
};
Q_ENUM_NS(LiquidSolution)
enum class CompositionUnit {
	AtomicPercent,
	WeightPercent,
	Mol,
	Gram
};
Q_ENUM_NS(CompositionUnit)
enum class TemperatureUnit {
	Kelvin,
	Celsius,
	Fahrenheit
};
Q_ENUM_NS(TemperatureUnit)
enum class PressureUnit {
	MPa,
	atm,
	bar
};
Q_ENUM_NS(PressureUnit)
}

struct Range {
	double start, stop, step;
};

class Parameters
{

public:
	Parameters();

};

#endif // PARAMETERS_H
