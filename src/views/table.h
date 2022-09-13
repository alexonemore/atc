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

#ifndef TABLE_H
#define TABLE_H

#include <QTableView>

class Table : public QTableView
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(Table)
private:
	QMenu* context_menu;

public:
	explicit Table(QWidget* parent = nullptr);
private:
	void Copy(const bool with_headers);
	void CopyMimeData(const QModelIndexList& from_indices, QMimeData* mime_data,
					  const bool with_headers);


	// QWidget interface
protected:
	void keyPressEvent(QKeyEvent* event) override;
};

#endif // TABLE_H
