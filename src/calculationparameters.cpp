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

#include "calculationparameters.h"
#include "ui_calculationparameters.h"
#include "utilities.h"
#include "optimization.h"
#include <QMessageBox>

CalculationParameters::CalculationParameters(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CalculationParameters)
{
	ui->setupUi(this);

	ui->workmode->addItems(ParametersNS::workmode);
	ui->target->addItems(ParametersNS::target);
	ui->liquid_solution->addItems(ParametersNS::liquid_solution);
	ui->database->addItems(ParametersNS::databases);
	ui->choose_substances->addItems(ParametersNS::choose_substances);
	ui->extrapolation->addItems(ParametersNS::extrapolation);
	ui->minimization_function->addItems(ParametersNS::minimization_function);
	ui->composition_units->addItems(ParametersNS::composition_units);
	ui->temperature_initial_units->addItems(ParametersNS::temperature_units);
	ui->temperature_units->addItems(ParametersNS::temperature_units);
	ui->pressure_initial_units->addItems(ParametersNS::pressure_units);
	ui->pressure_units->addItems(ParametersNS::pressure_units);
	ui->temperature_result_units->addItems(ParametersNS::temperature_units);
	ui->composition_result_units->addItems(ParametersNS::composition_units);

	connect(ui->update, &QPushButton::clicked,
			this, &CalculationParameters::UpdateButtonHandler);

	connect(ui->calculate, &QPushButton::clicked,
			this, &CalculationParameters::CalculateButtonHandler);

	connect(ui->clear, &QPushButton::clicked,
			this, &CalculationParameters::Clear);

	connect(ui->periodic_table, &PeriodicTable::SignalClickedElementButton,
			this, &CalculationParameters::UpdateSelectedElements);

	SetupParameters();
}

CalculationParameters::~CalculationParameters()
{
	delete ui;
}

static double FixDelimeter(const QString& str)
{
	return str.simplified().replace(",", ".").toDouble();
}

ParametersNS::Parameters CalculationParameters::GetCurrentParameters()
{
	ParametersNS::Parameters p;

	p.workmode = static_cast<ParametersNS::Workmode>(ui->workmode->currentIndex());
	p.target = static_cast<ParametersNS::Target>(ui->target->currentIndex());
	p.liquid_solution = static_cast<ParametersNS::LiquidSolution>(ui->liquid_solution->currentIndex());
	p.H_initial_by = static_cast<ParametersNS::H_Initial_By>(ui->choose_substances->currentIndex());
	p.database = static_cast<ParametersNS::Database>(ui->database->currentIndex());
	p.minimization_function = static_cast<ParametersNS::MinimizationFunction>(ui->minimization_function->currentIndex());
	p.extrapolation = static_cast<ParametersNS::Extrapolation>(ui->extrapolation->currentIndex());
	p.composition_range_unit = static_cast<ParametersNS::CompositionUnit>(ui->composition_units->currentIndex());
	p.temperature_initial_unit = static_cast<ParametersNS::TemperatureUnit>(ui->temperature_initial_units->currentIndex());
	p.pressure_initial_unit = static_cast<ParametersNS::PressureUnit>(ui->pressure_initial_units->currentIndex());
	p.temperature_range_unit = static_cast<ParametersNS::TemperatureUnit>(ui->temperature_units->currentIndex());
	p.pressure_range_unit = static_cast<ParametersNS::PressureUnit>(ui->pressure_units->currentIndex());

	p.temperature_initial = FixDelimeter(ui->temperature_initial->text());
	p.pressure_initial = FixDelimeter(ui->pressure_initial->text());

	p.composition_range.start = FixDelimeter(ui->composition_start->text());
	p.composition_range.stop = FixDelimeter(ui->composition_stop->text());
	p.composition_range.step = FixDelimeter(ui->composition_step->text());

	p.temperature_range.start = FixDelimeter(ui->temperature_start->text());
	p.temperature_range.stop = FixDelimeter(ui->temperature_stop->text());
	p.temperature_range.step = FixDelimeter(ui->temperature_step->text());

	p.pressure_range.start = FixDelimeter(ui->pressure_start->text());
	p.pressure_range.stop = FixDelimeter(ui->pressure_stop->text());
	p.pressure_range.step = FixDelimeter(ui->pressure_step->text());

	p.threads = ui->threads->value();
	p.at_accuracy = ui->at_accuracy->value();

	p.show_phases.gas = ui->show_gas->isChecked();
	p.show_phases.liquid = ui->show_liquid->isChecked();
	p.show_phases.solid = ui->show_solid->isChecked();
	p.show_phases.aqueous = ui->show_aqueous->isChecked();
	p.show_phases.ions = ui->show_ions->isChecked();

	p.checked_elements = ui->periodic_table->GetCheckedElements();

	p.temperature_result_unit = static_cast<ParametersNS::TemperatureUnit>(ui->temperature_result_units->currentIndex());
	p.composition_result_unit = static_cast<ParametersNS::CompositionUnit>(ui->composition_result_units->currentIndex());
	p.show_initial_in_result = ui->show_initial_in_result->isChecked();

	p.FixInputParameters();
	SetupParameters(p);
	return p;
}

void CalculationParameters::SetEnabledElements(const QStringList& elements)
{
	ui->periodic_table->SetEnabledElements(elements);
}

void CalculationParameters::SetupParameters(const ParametersNS::Parameters p)
{
	ui->workmode->setCurrentIndex(static_cast<int>(p.workmode));
	ui->target->setCurrentIndex(static_cast<int>(p.target));
	ui->liquid_solution->setCurrentIndex(static_cast<int>(p.liquid_solution));
	ui->choose_substances->setCurrentIndex(static_cast<int>(p.H_initial_by));
	ui->database->setCurrentIndex(static_cast<int>(p.database));
	ui->minimization_function->setCurrentIndex(static_cast<int>(p.minimization_function));
	ui->extrapolation->setCurrentIndex(static_cast<int>(p.extrapolation));
	ui->temperature_initial_units->setCurrentIndex(static_cast<int>(p.temperature_initial_unit));
	ui->pressure_initial_units->setCurrentIndex(static_cast<int>(p.pressure_initial_unit));
	ui->composition_units->setCurrentIndex(static_cast<int>(p.composition_range_unit));
	ui->temperature_units->setCurrentIndex(static_cast<int>(p.temperature_range_unit));
	ui->pressure_units->setCurrentIndex(static_cast<int>(p.pressure_range_unit));

	ui->at_accuracy->setRange(ParametersNS::at_accuracy_min,
							  ParametersNS::at_accuracy_max);
	ui->at_accuracy->setValue(p.at_accuracy);
	ui->threads->setRange(1, p.MaxThreadsCount());
	ui->threads->setValue(p.threads);

	ui->temperature_initial->setText(QString::number(p.temperature_initial));
	ui->pressure_initial->setText(QString::number(p.pressure_initial));
	ui->composition_start->setText(QString::number(p.composition_range.start));
	ui->composition_stop->setText(QString::number(p.composition_range.stop));
	ui->composition_step->setText(QString::number(p.composition_range.step));
	ui->temperature_start->setText(QString::number(p.temperature_range.start));
	ui->temperature_stop->setText(QString::number(p.temperature_range.stop));
	ui->temperature_step->setText(QString::number(p.temperature_range.step));
	ui->pressure_start->setText(QString::number(p.pressure_range.start));
	ui->pressure_stop->setText(QString::number(p.pressure_range.stop));
	ui->pressure_step->setText(QString::number(p.pressure_range.step));

	ui->show_gas->setChecked(p.show_phases.gas);
	ui->show_liquid->setChecked(p.show_phases.liquid);
	ui->show_solid->setChecked(p.show_phases.solid);
	ui->show_aqueous->setChecked(p.show_phases.aqueous);
	ui->show_ions->setChecked(p.show_phases.ions);

	ui->temperature_result_units->setCurrentIndex(static_cast<int>(p.temperature_result_unit));
	ui->composition_result_units->setCurrentIndex(static_cast<int>(p.composition_result_unit));
	ui->show_initial_in_result->setChecked(p.show_initial_in_result);
}

void CalculationParameters::UpdateSelectedElements()
{
	emit UpdateParameters(GetCurrentParameters());
}

void CalculationParameters::UpdateButtonHandler()
{
	emit UpdateButtonClicked(GetCurrentParameters());
}

void CalculationParameters::Clear()
{
	SetupParameters();
	ui->periodic_table->Clear();
	emit UpdateParameters(ParametersNS::Parameters{});
}

namespace {
double NumberOfCalculations(const ParametersNS::Parameters& parameters)
{
	double size_temperature = 1;
	double size_composition = 1;
	auto number = [](ParametersNS::Range range) -> double {
		return 1 + std::ceil((range.stop - range.start) / range.step);
	};
	switch (parameters.workmode) {
	case ParametersNS::Workmode::SinglePoint:
		break;
	case ParametersNS::Workmode::TemperatureRange:
		size_temperature = number(parameters.temperature_range);
		break;
	case ParametersNS::Workmode::CompositionRange:
		size_composition = number(parameters.composition_range);
		break;
	case ParametersNS::Workmode::TemperatureCompositionRange:
		size_temperature = number(parameters.temperature_range);
		size_composition = number(parameters.composition_range);
		break;
	}
	return size_temperature * size_composition;
}
}

void CalculationParameters::CalculateButtonHandler()
{
	auto parameters = GetCurrentParameters();
	auto number_of_calculations = NumberOfCalculations(parameters);
	auto number = QString::number(number_of_calculations, 'f', 100)
			.replace(',', '.').split('.').first();
	constexpr double max_number_of_calculations = 100'000;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	const
#else
	constexpr
#endif
	auto max_capasity = std::min(std::vector<double>().max_size(),
								 Optimization::OptimizationVector().max_size()) ;
	LOG(number_of_calculations)
	LOG(max_capasity)

	if(number_of_calculations >= static_cast<double>(max_capasity))
	{
		QMessageBox::critical(
					this,
					tr("Error!"),
					tr("You are going to count\n\n") +
					number +
					tr("\n\ncombinations.\nThis cannot be done on this computer.\n"
					   "Reduce the range parameters."));
		return;
	}
	if (number_of_calculations >= max_number_of_calculations) {
		int ret = QMessageBox::warning(
					this,
					tr("Warning!"),
					tr("You are going to count\n\n") +
					number +
					tr("\n\ncombinations. this can take a long time\n"
					   "and requires a lot of memory.\n"
					   "Continue only if you are confident in your computer."
					   "Or you can reduce the range parameters.\n\n"
					   "Do you want to continue the calculation?"),
					QMessageBox::Yes | QMessageBox::Cancel,
					QMessageBox::Cancel);
		if(ret != QMessageBox::Yes)
			return;
	}

	emit UpdateParameters(parameters);
	emit StartCalculate();
}
