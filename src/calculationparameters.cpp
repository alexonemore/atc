#include "calculationparameters.h"
#include "ui_calculationparameters.h"
#include "parameters.h"

CalculationParameters::CalculationParameters(QWidget *parent) :
	QGroupBox(parent),
	ui(new Ui::CalculationParameters)
{
	ui->setupUi(this);
	setTitle(QStringLiteral("Calculation parameters"));

	auto to_list = [](QMetaEnum&& me) -> QStringList {
		auto max = me.keyCount();
		QStringList list;
		for(int i = 0; i != max; ++i) {
			list.append(me.valueToKey(me.value(i)));
		}
		return list;
	};

	auto workmodes = to_list(QMetaEnum::fromType<ParametersNS::Workmode>());
	auto targets = to_list(QMetaEnum::fromType<ParametersNS::Target>());
	auto liquid_solutions = to_list(QMetaEnum::fromType<ParametersNS::LiquidSolution>());
	auto composition_units = to_list(QMetaEnum::fromType<ParametersNS::CompositionUnit>());
	auto temperature_units = to_list(QMetaEnum::fromType<ParametersNS::TemperatureUnit>());
	auto pressure_units = to_list(QMetaEnum::fromType<ParametersNS::PressureUnit>());


}

CalculationParameters::~CalculationParameters()
{
	delete ui;
}
