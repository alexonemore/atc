/* This file is part of ATC (Adiabatic Temperature Calculator).
 * Copyright (c) 2022 Alexandr Shchukin
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

#ifndef SPECIALDELEGATES_H
#define SPECIALDELEGATES_H

#include <QStyledItemDelegate>

class DoubleNumberDelegate final : public QStyledItemDelegate
{
	Q_OBJECT

public:
	DoubleNumberDelegate(QObject *parent = nullptr);
	~DoubleNumberDelegate() override;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
						  const QModelIndex &index) const override;
	void setEditorData(QWidget *editor,
					   const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
					  const QModelIndex &index) const override;
	void updateEditorGeometry(QWidget *editor,
							  const QStyleOptionViewItem &option,
							  const QModelIndex &index) const override;
};

class ColorPickerDelegate final : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ColorPickerDelegate(QObject *parent = nullptr);
	~ColorPickerDelegate() override;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
						  const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
					  const QModelIndex &index) const override;
};

#if 0
class SpinBoxDelegate final : public QStyledItemDelegate
{
	Q_OBJECT

public:
	SpinBoxDelegate(QObject *parent = nullptr);
	~SpinBoxDelegate() override;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
						  const QModelIndex &index) const override;
	void setEditorData(QWidget *editor,
					   const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
					  const QModelIndex &index) const override;
	void updateEditorGeometry(QWidget *editor,
							  const QStyleOptionViewItem &option,
							  const QModelIndex &index) const override;
};
#endif

#endif // SPECIALDELEGATES_H
