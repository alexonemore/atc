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

#include "table.h"
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDateTime>
#include <QBuffer>
#include <QKeyEvent>
#include <set>
#include "utilities.h"

Table::Table(QWidget* parent)
	: QTableView(parent)
{
	context_menu = new QMenu(this);
	auto copy_action = new QAction(QIcon("images/copy.svg"),
								   tr("Copy"), context_menu);
	auto copy_with_headers_action = new QAction(QIcon("images/copy.svg"),
									tr("Copy with Headers"), context_menu);

	context_menu->addAction(copy_action);
	context_menu->addAction(copy_with_headers_action);
	setContextMenuPolicy(Qt::CustomContextMenu);

	// This is only for displaying the shortcut in the context menu.
	// An entry in keyPressEvent is still needed.
	copy_action->setShortcut(QKeySequence::Copy);
	copy_with_headers_action->setShortcut(QKeySequence(tr("Ctrl+Shift+C")));

	// Set up context menu actions
	connect(this, &QTableView::customContextMenuRequested,
			[this](const QPoint& pos) {
		context_menu->popup(viewport()->mapToGlobal(pos));
	});

	connect(copy_action, &QAction::triggered, [this]() {
		Copy(false);
	});
	connect(copy_with_headers_action, &QAction::triggered, [this]() {
		Copy(true);
	});

}

Table::~Table()
{

}

void Table::Copy(const bool with_headers)
{
	LOG(with_headers)
	QMimeData *mime_data = new QMimeData;
	CopyMimeData(selectionModel()->selectedIndexes(), mime_data, with_headers);
	qApp->clipboard()->setMimeData(mime_data);
}

void Table::CopyMimeData(const QModelIndexList& from_indices,
						 QMimeData* mime_data, const bool with_headers)
{
	// A slightly simplified version of same function from sqlitebrowser

	QModelIndexList indices = from_indices;

	// Remove all indices from hidden columns, because if we don't we might
	// copy data from hidden columns as well which is very unintuitive;
	// especially copying the rowid column when selecting all columns of a
	// table is a problem because pasting the data won't work as expected.
	QMutableListIterator<QModelIndex> it(indices);
	while (it.hasNext()) {
		if (isColumnHidden(it.next().column()))
			it.remove();
	}

	// Abort if there's nothing to copy
	if (indices.isEmpty())
		return;

	// If a single cell is selected which contains an image,
	// copy it to the clipboard
	if (!with_headers && indices.size() == 1) {
		QImage img;
		QVariant varData = model()->data(indices.first(), Qt::DisplayRole);

		if (img.loadFromData(varData.toByteArray())) {
			// If it's an image, copy the image data to the clipboard
			mime_data->setImageData(img);
			return;
		}
	}

	// If we got here, a non-image cell was or multiple cells were selected,
	// or copy with headers was requested. In this case, we copy selected data
	// into internal copy-paste buffer and then we write a table both in HTML
	// and text formats to the system clipboard.

	// Copy selected data into internal copy-paste buffer
	int last_row = indices.first().row();

	std::vector<std::vector<QByteArray>> buffer;
	std::vector<QByteArray> lst;
	for(int i=0;i<indices.size();i++)
	{
		if(indices.at(i).row() != last_row)
		{
			buffer.push_back(lst);
			lst.clear();
		}
		lst.push_back(indices.at(i).data(Qt::DisplayRole).toByteArray());
		last_row = indices.at(i).row();
	}
	buffer.push_back(lst);

	QString result;
	QString html_result = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">";
	html_result.append("<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">");
	html_result.append("<title></title>");

	// The generator-stamp is later used to know whether the data in the system clipboard is still ours.
	// In that case we will give precedence to our internal copy buffer.
	QString now = QDateTime::currentDateTime().toString("YYYY-MM-DDTHH:mm:ss.zzz");
	auto generator_stamp = QString("<meta name=\"generator\" content=\"%1\"><meta name=\"date\" content=\"%2\">")
			.arg(QApplication::applicationName().toHtmlEscaped(), now);
	html_result.append(generator_stamp);
	// TODO: is this really needed by Excel, since we use <pre> for multi-line cells?
	html_result.append("<style type=\"text/css\">br{mso-data-placement:same-cell;}</style></head><body>"
					  "<table border=1 cellspacing=0 cellpadding=2>");

	// Insert the columns in a set, since they could be non-contiguous.
	std::set<int> cols_in_indexes, rows_in_indexes;
	for(const QModelIndex & idx : qAsConst(indices)) {
		cols_in_indexes.insert(idx.column());
		rows_in_indexes.insert(idx.row());
	}

	int current_row = indices.first().row();

	const QString fieldSepText = "\t";
	const QString rowSepText = "\n";

	// Table headers
	if (with_headers) {
		html_result.append("<tr><th>");
		int firstColumn = *cols_in_indexes.begin();

		for(int col : cols_in_indexes) {
			QByteArray headerText = model()->headerData(col,
								Qt::Horizontal, Qt::DisplayRole).toByteArray();
			if (col != firstColumn) {
				result.append(fieldSepText);
				html_result.append("</th><th>");
			}
			result.append(headerText);
			html_result.append(headerText);
		}
		result.append(rowSepText);
		html_result.append("</th></tr>");
	}

	// Iterate over rows x cols checking if the index actually exists
	// when needed, in order to support non-rectangular selections.
	for(const int row : rows_in_indexes) {
		for(const int column : cols_in_indexes) {
			const QModelIndex index = indices.first().sibling(row, column);
			QString style;
			if(indices.contains(index)) {
				QFont font{index.data(Qt::FontRole).value<QFont>()};
				const QString fontStyle(font.italic() ? "italic" : "normal");
				const QString fontWeigth(font.bold() ? "bold" : "normal");
				const QString fontDecoration(font.underline() ? " text-decoration: underline;" : "");
				const QColor bgColor(index.data(Qt::BackgroundRole).value<QBrush>().color());
				const QColor fgColor(index.data(Qt::ForegroundRole).value<QBrush>().color());
				const Qt::Alignment align(index.data(Qt::TextAlignmentRole).toInt());
				style = QString("style=\"font-family: '%1'; font-size: %2pt; font-style: %3; font-weight: %4;%5 "
								"background-color: %6; color: %7\"").arg(
									font.family().toHtmlEscaped(),
									QString::number(font.pointSize()),
									fontStyle,
									fontWeigth,
									fontDecoration,
									bgColor.name(),
									fgColor.name());
			}

			// Separators. For first cell, only opening table row tags
			// must be added for the HTML and nothing for the text version.
			if (index.row() == *rows_in_indexes.begin() && index.column() == *cols_in_indexes.begin()) {
				html_result.append(QString("<tr><td %1>").arg(style));
			} else if (index.row() != current_row) {
				result.append(rowSepText);
				html_result.append(QString("</td></tr><tr><td %1>").arg(style));
			} else {
				result.append(fieldSepText);
				html_result.append(QString("</td><td %1>").arg(style));
			}

			current_row = index.row();

			QImage img;
			QVariant bArrdata = indices.contains(index) ? index.data(Qt::DisplayRole) : QVariant();

			// Table cell data: image? Store it as an embedded image in HTML
			if (img.loadFromData(bArrdata.toByteArray()))
			{
				QByteArray ba;
				QBuffer buffer(&ba);
				buffer.open(QIODevice::WriteOnly);
				img.save(&buffer, "PNG");
				buffer.close();

				QString imageBase64 = ba.toBase64();
				html_result.append("<img src=\"data:image/png;base64,");
				html_result.append(imageBase64);
				result.append(QString());
				html_result.append("\" alt=\"Image\">");
			} else {
				QByteArray text = bArrdata.toByteArray();

				// Table cell data: text
				if (text.contains('\n') || text.contains('\t')) {
					html_result.append("<pre>" + QString(text).toHtmlEscaped() + "</pre>");
				} else {
					html_result.append(QString(text).toHtmlEscaped());
				}
				result.append(text);
			}
		}
	}

	mime_data->setHtml(html_result + "</td></tr></table></body></html>");
	mime_data->setText(result);
}

void Table::keyPressEvent(QKeyEvent* event)
{
#if 0
	if(event->matches(QKeySequence::Copy))
	{
		Copy(false);
	} else if(event->modifiers().testFlag(Qt::ControlModifier) &&
			  event->modifiers().testFlag(Qt::ShiftModifier) &&
			  (event->key() == Qt::Key_C))
	{
		// Call copy with headers when Ctrl-Shift-C is pressed
		Copy(true);
	}
#endif
	switch(event->key()) {
	case Qt::Key_Delete:
	case Qt::Key_Backspace:
		if(hasFocus()) {
			auto selected = selectionModel()->selectedIndexes();
			emit Delete(selected);
		}
		break;
	case Qt::Key_C:
		if(event->modifiers().testFlag(Qt::ControlModifier)) {
			if(event->modifiers().testFlag(Qt::ShiftModifier)) {
				// Call copy with headers when Ctrl-Shift-C is pressed
				Copy(true);
			} else {
				// Call copy without headers when Ctrl-C is pressed
				Copy(false);
			}
		}
		break;
	default:
		break;
	}
	QTableView::keyPressEvent(event);
}
