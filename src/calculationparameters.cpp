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

#include "calculationparameters.h"
#include "ui_calculationparameters.h"

CalculationParameters::CalculationParameters(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CalculationParameters)
{
	ui->setupUi(this);

	ui->workmode->addItems(ParametersNS::workmode);
	ui->target->addItems(ParametersNS::target);
	ui->liquid_solution->addItems(ParametersNS::liquid_solution);
	ui->database->addItems(ParametersNS::datebases);
	ui->choose_substances->addItems(ParametersNS::choose_substances);
	ui->extrapolation->addItems(ParametersNS::extrapolation);
	ui->minimization_function->addItems(ParametersNS::minimization_function);
	ui->composition1_units->addItems(ParametersNS::composition_units);
	ui->composition2_units->addItems(ParametersNS::composition_units);
	ui->group1_units->addItems(ParametersNS::composition_units);
	ui->group2_units->addItems(ParametersNS::composition_units);
	ui->temperature_initial_units->addItems(ParametersNS::temperature_units);
	ui->temperature_units->addItems(ParametersNS::temperature_units);
	ui->pressure_initial_units->addItems(ParametersNS::pressure_units);
	ui->pressure_units->addItems(ParametersNS::pressure_units);

	connect(ui->update, &QPushButton::clicked,
			this, &CalculationParameters::GetParametersFromGUI);

	connect(ui->calculate, &QPushButton::clicked,
			this, &CalculationParameters::StartCalculate);

	SetupInitialParameters();
}

CalculationParameters::~CalculationParameters()
{
	delete ui;
}

void CalculationParameters::SetupInitialParameters()
{
	ParametersNS::Parameters p;
	ui->workmode->setCurrentIndex(static_cast<int>(p.workmode));
	ui->target->setCurrentIndex(static_cast<int>(p.target));
	ui->liquid_solution->setCurrentIndex(static_cast<int>(p.liquid_solution));
	ui->choose_substances->setCurrentIndex(static_cast<int>(p.choose_substances));
	ui->database->setCurrentIndex(static_cast<int>(p.database));
	ui->minimization_function->setCurrentIndex(static_cast<int>(p.minimization_function));
	ui->extrapolation->setCurrentIndex(static_cast<int>(p.extrapolation));
	ui->group1_units->setCurrentIndex(static_cast<int>(p.group1_unit));
	ui->group2_units->setCurrentIndex(static_cast<int>(p.group2_unit));
	ui->temperature_initial_units->setCurrentIndex(static_cast<int>(p.temperature_initial_unit));
	ui->pressure_initial_units->setCurrentIndex(static_cast<int>(p.pressure_initial_unit));
	ui->composition1_units->setCurrentIndex(static_cast<int>(p.composition1_unit));
	ui->composition2_units->setCurrentIndex(static_cast<int>(p.composition2_unit));
	ui->temperature_units->setCurrentIndex(static_cast<int>(p.temperature_range_unit));
	ui->pressure_units->setCurrentIndex(static_cast<int>(p.pressure_range_unit));

	ui->at_accuracy->setRange(ParametersNS::at_accuracy_min,
							  ParametersNS::at_accuracy_max);
	ui->at_accuracy->setValue(p.at_accuracy);
	ui->threads->setRange(1, p.threads);
	ui->threads->setValue(p.threads);

	ui->temperature_initial->setText(QString::number(p.temperature_initial));
	ui->pressure_initial->setText(QString::number(p.pressure_initial));
	ui->composition1_start->setText(QString::number(p.composition1_range.start));
	ui->composition1_stop->setText(QString::number(p.composition1_range.stop));
	ui->composition1_step->setText(QString::number(p.composition1_range.step));
	ui->composition2_start->setText(QString::number(p.composition2_range.start));
	ui->composition2_stop->setText(QString::number(p.composition2_range.stop));
	ui->composition2_step->setText(QString::number(p.composition2_range.step));
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
}

void CalculationParameters::GetParametersFromGUI()
{
	ParametersNS::Parameters p;

	p.workmode = static_cast<ParametersNS::Workmode>(ui->workmode->currentIndex());
	p.target = static_cast<ParametersNS::Target>(ui->target->currentIndex());
	p.liquid_solution = static_cast<ParametersNS::LiquidSolution>(ui->liquid_solution->currentIndex());
	p.choose_substances = static_cast<ParametersNS::ChooseSubstances>(ui->choose_substances->currentIndex());
	p.database = static_cast<ParametersNS::Database>(ui->database->currentIndex());
	p.minimization_function = static_cast<ParametersNS::MinimizationFunction>(ui->minimization_function->currentIndex());
	p.extrapolation = static_cast<ParametersNS::Extrapolation>(ui->extrapolation->currentIndex());
	p.group1_unit = static_cast<ParametersNS::CompositionUnit>(ui->group1_units->currentIndex());
	p.group2_unit = static_cast<ParametersNS::CompositionUnit>(ui->group2_units->currentIndex());
	p.composition1_unit = static_cast<ParametersNS::CompositionUnit>(ui->composition1_units->currentIndex());
	p.composition2_unit = static_cast<ParametersNS::CompositionUnit>(ui->composition2_units->currentIndex());
	p.temperature_initial_unit = static_cast<ParametersNS::TemperatureUnit>(ui->temperature_initial_units->currentIndex());
	p.pressure_initial_unit = static_cast<ParametersNS::PressureUnit>(ui->pressure_initial_units->currentIndex());
	p.temperature_range_unit = static_cast<ParametersNS::TemperatureUnit>(ui->temperature_units->currentIndex());
	p.pressure_range_unit = static_cast<ParametersNS::PressureUnit>(ui->pressure_units->currentIndex());

	p.temperature_initial = ui->temperature_initial->text().toDouble();
	p.pressure_initial = ui->pressure_initial->text().toDouble();

	p.composition1_range.start = ui->composition1_start->text().toDouble();
	p.composition1_range.stop = ui->composition1_stop->text().toDouble();
	p.composition1_range.step = ui->composition1_step->text().toDouble();
	p.composition2_range.start = ui->composition2_start->text().toDouble();
	p.composition2_range.stop = ui->composition2_stop->text().toDouble();
	p.composition2_range.step = ui->composition2_step->text().toDouble();

	p.temperature_range.start = ui->temperature_start->text().toDouble();
	p.temperature_range.stop = ui->temperature_stop->text().toDouble();
	p.temperature_range.step = ui->temperature_step->text().toDouble();

	p.pressure_range.start = ui->pressure_start->text().toDouble();
	p.pressure_range.stop = ui->pressure_stop->text().toDouble();
	p.pressure_range.step = ui->pressure_step->text().toDouble();

	p.threads = ui->threads->value();
	p.at_accuracy = ui->at_accuracy->value();

	p.show_phases.gas = ui->show_gas->isChecked();
	p.show_phases.liquid = ui->show_liquid->isChecked();
	p.show_phases.solid = ui->show_solid->isChecked();
	p.show_phases.aqueous = ui->show_aqueous->isChecked();
	p.show_phases.ions = ui->show_ions->isChecked();

	emit UpdateParameters(p);
}
