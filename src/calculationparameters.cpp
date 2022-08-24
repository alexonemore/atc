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
#include "parameters.h"

CalculationParameters::CalculationParameters(QWidget *parent) :
	QGroupBox(parent),
	ui(new Ui::CalculationParameters)
{
	ui->setupUi(this);
	setTitle(QStringLiteral("Calculation parameters"));

	ui->workmode->insertItems(0, ParametersNS::workmode);
	ui->target->insertItems(1, ParametersNS::target);


}

CalculationParameters::~CalculationParameters()
{
	delete ui;
}
