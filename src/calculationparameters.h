#ifndef CALCULATIONPARAMETERS_H
#define CALCULATIONPARAMETERS_H

#include <QGroupBox>

namespace Ui {
class CalculationParameters;
}

class CalculationParameters : public QGroupBox
{
	Q_OBJECT

public:
	explicit CalculationParameters(QWidget *parent = nullptr);
	~CalculationParameters() override;

private:
	Ui::CalculationParameters *ui;
};

#endif // CALCULATIONPARAMETERS_H
