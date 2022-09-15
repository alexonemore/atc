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

#include "periodictable.h"
#include "utilities.h"
#include "ui_periodictable.h"
#include <QDebug>

PeriodicTable::PeriodicTable(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PeriodicTable)
{
	ui->setupUi(this);
	buttons = QVector<QPushButton*>{ui->pushButton_1,
			ui->pushButton_2,
			ui->pushButton_3,
			ui->pushButton_4,
			ui->pushButton_5,
			ui->pushButton_6,
			ui->pushButton_7,
			ui->pushButton_8,
			ui->pushButton_9,
			ui->pushButton_10,
			ui->pushButton_11,
			ui->pushButton_12,
			ui->pushButton_13,
			ui->pushButton_14,
			ui->pushButton_15,
			ui->pushButton_16,
			ui->pushButton_17,
			ui->pushButton_18,
			ui->pushButton_19,
			ui->pushButton_20,
			ui->pushButton_21,
			ui->pushButton_22,
			ui->pushButton_23,
			ui->pushButton_24,
			ui->pushButton_25,
			ui->pushButton_26,
			ui->pushButton_27,
			ui->pushButton_28,
			ui->pushButton_29,
			ui->pushButton_30,
			ui->pushButton_31,
			ui->pushButton_32,
			ui->pushButton_33,
			ui->pushButton_34,
			ui->pushButton_35,
			ui->pushButton_36,
			ui->pushButton_37,
			ui->pushButton_38,
			ui->pushButton_39,
			ui->pushButton_40,
			ui->pushButton_41,
			ui->pushButton_42,
			ui->pushButton_43,
			ui->pushButton_44,
			ui->pushButton_45,
			ui->pushButton_46,
			ui->pushButton_47,
			ui->pushButton_48,
			ui->pushButton_49,
			ui->pushButton_50,
			ui->pushButton_51,
			ui->pushButton_52,
			ui->pushButton_53,
			ui->pushButton_54,
			ui->pushButton_55,
			ui->pushButton_56,
			ui->pushButton_57,
			ui->pushButton_58,
			ui->pushButton_59,
			ui->pushButton_60,
			ui->pushButton_61,
			ui->pushButton_62,
			ui->pushButton_63,
			ui->pushButton_64,
			ui->pushButton_65,
			ui->pushButton_66,
			ui->pushButton_67,
			ui->pushButton_68,
			ui->pushButton_69,
			ui->pushButton_70,
			ui->pushButton_71,
			ui->pushButton_72,
			ui->pushButton_73,
			ui->pushButton_74,
			ui->pushButton_75,
			ui->pushButton_76,
			ui->pushButton_77,
			ui->pushButton_78,
			ui->pushButton_79,
			ui->pushButton_80,
			ui->pushButton_81,
			ui->pushButton_82,
			ui->pushButton_83,
			ui->pushButton_84,
			ui->pushButton_85,
			ui->pushButton_86,
			ui->pushButton_87,
			ui->pushButton_88,
			ui->pushButton_89,
			ui->pushButton_90,
			ui->pushButton_91,
			ui->pushButton_92,
			ui->pushButton_93,
			ui->pushButton_94,
			ui->pushButton_95,
			ui->pushButton_96,
			ui->pushButton_97,
			ui->pushButton_98,
			ui->pushButton_99,
			ui->pushButton_100,
			ui->pushButton_101,
			ui->pushButton_102,
			ui->pushButton_103};
	for(auto&& button : buttons) {
		buttons_map[button->text()] = button;
	}
	for(auto&& button : buttons) {
		connect(button, &QPushButton::clicked,
				this, &PeriodicTable::ClickButtonHandler);
	}
}

PeriodicTable::~PeriodicTable()
{
	delete ui;
}

QStringList PeriodicTable::GetCheckedElements() const
{
	QStringList checked_elements;
	for(const auto& button : buttons) {
		if(button->isChecked()) {
			checked_elements.push_back(button->text());
		}
	}
	LOG(checked_elements)
	return checked_elements;
}

void PeriodicTable::SetEnabledElements(const QStringList& elements)
{
	for(auto&& button : buttons) {
		button->setEnabled(false);
	}
	for(const auto& el : elements) {
		auto f = buttons_map.find(el);
		if(f != buttons_map.end()) {
			f->second->setEnabled(true);
		}
	}
	for(auto&& button : buttons) {
		if(!button->isEnabled()) {
			button->setChecked(false);
		}
	}
}

void PeriodicTable::Clear()
{
	for(auto&& button : buttons) {
		button->setChecked(false);
	}
}

void PeriodicTable::ClickButtonHandler()
{
	emit SignalClickedElementButton();
}
