#include "periodictable.h"
#include "ui_periodictable.h"
#include <QDebug>

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

QStringList PeriodicTable::GetCheckedElements() const
{
	QStringList checked_elements;
	for(const auto& button : children()) {
		auto&& p = static_cast<QPushButton*>(button);
		if(p->isChecked()) {
			checked_elements.push_back(p->text());
		}
	}
	qDebug() << checked_elements;
	return checked_elements;
}
