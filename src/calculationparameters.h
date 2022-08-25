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

#ifndef CALCULATIONPARAMETERS_H
#define CALCULATIONPARAMETERS_H

#include <QWidget>
#include "parameters.h"

namespace Ui {
class CalculationParameters;
}

class CalculationParameters : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(CalculationParameters)

public:
	explicit CalculationParameters(QWidget *parent = nullptr);
	~CalculationParameters() override;

public slots:
	void SetEnabledElements(const QStringList& elements);

private:
	Ui::CalculationParameters *ui;

private:
	void SetupInitialParameters();

private slots:
	void GetParametersFromGUI();

signals:
	void UpdateParameters(ParametersNS::Parameters parameters);
	void StartCalculate();
};

#endif // CALCULATIONPARAMETERS_H
