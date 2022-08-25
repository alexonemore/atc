#include "periodictable.h"
#include "ui_periodictable.h"

PeriodicTable::PeriodicTable(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PeriodicTable)
{
	ui->setupUi(this);
}

PeriodicTable::~PeriodicTable()
{
	delete ui;
}
