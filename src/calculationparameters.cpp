#include "calculationparameters.h"
#include "ui_calculationparameters.h"

CalculationParameters::CalculationParameters(QWidget *parent) :
	QGroupBox(parent),
	ui(new Ui::CalculationParameters)
{
	ui->setupUi(this);
	setTitle(QStringLiteral("Calculation parameters"));
}

CalculationParameters::~CalculationParameters()
{
	delete ui;
}
