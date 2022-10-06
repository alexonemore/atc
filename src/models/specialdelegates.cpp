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

#include "specialdelegates.h"
#include <QColorDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QKeyEvent>
#include "utilities.h"

DoubleNumberDelegate::DoubleNumberDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{

}

DoubleNumberDelegate::~DoubleNumberDelegate()
{

}

QWidget* DoubleNumberDelegate::createEditor(QWidget* parent,
											const QStyleOptionViewItem& option,
											const QModelIndex& index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	QLineEdit *editor = new QLineEdit(parent);
	editor->setFrame(false);
	return editor;
}

void DoubleNumberDelegate::setEditorData(QWidget* editor,
										 const QModelIndex& index) const
{
	QString value = index.data().toString();
	QLineEdit *le = static_cast<QLineEdit*>(editor);
	le->setText(value);
}

void DoubleNumberDelegate::setModelData(
		QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const
{
	QLineEdit *le = static_cast<QLineEdit*>(editor);
	double val = le->text().replace(',', '.').toDouble();
	model->setData(index, val, Qt::EditRole);
}

void DoubleNumberDelegate::updateEditorGeometry(
		QWidget* editor, const QStyleOptionViewItem& option,
		const QModelIndex& index) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

ColorPickerDelegate::ColorPickerDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{

}

ColorPickerDelegate::~ColorPickerDelegate()
{

}

QWidget* ColorPickerDelegate::createEditor(
		QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const
{
	Q_UNUSED(option)
	QColor value = index.data(Qt::EditRole).value<QColor>();
	QColorDialog *editor = new QColorDialog(value, parent);
	editor->setModal(true);
	return editor;
}

void ColorPickerDelegate::setModelData(
		QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const
{
	QColorDialog *cd = static_cast<QColorDialog*>(editor);
	if(cd->result() == QDialog::Accepted) {
		QColor color = cd->currentColor();
		model->setData(index, color, Qt::EditRole);
	}
}

#if 0
SpinBoxDelegate::SpinBoxDelegate(QObject* parent) : QStyledItemDelegate(parent)
{

}

SpinBoxDelegate::~SpinBoxDelegate()
{

}

QWidget* SpinBoxDelegate::createEditor(QWidget* parent,
									   const QStyleOptionViewItem& option,
									   const QModelIndex& index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	QSpinBox *editor = new QSpinBox(parent);
	editor->setFrame(false);
	editor->setMinimum(0);
	editor->setMaximum(10);
	return editor;
}

void SpinBoxDelegate::setEditorData(QWidget* editor,
									const QModelIndex& index) const
{
	int value = index.data().toInt();
	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
								   const QModelIndex& index) const
{
	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();
	model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget* editor,
										   const QStyleOptionViewItem& option,
										   const QModelIndex& index) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}
#endif
