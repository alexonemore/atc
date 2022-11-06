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

#include "substancestableview.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>
#include "utilities.h"
#include "substancestablemodel.h"

SubstancesTableView::SubstancesTableView(QWidget* parent)
	: QWidget(parent)
{
	substances = new Table(this);
	ranges = new Table(this);
	tabulated_tf = new Table(this);
	label = new QLabel(QStringLiteral("Nothing selected"), this);
	auto splitter = new QSplitter(this);
	auto right_splitter = new QSplitter(this);
	auto layout = new QVBoxLayout;
	auto right_layout = new QVBoxLayout;
	auto right_widget = new QWidget(this);

	setLayout(layout);
	layout->addWidget(splitter);

	splitter->addWidget(substances);
	splitter->addWidget(right_widget);

	right_widget->setLayout(right_layout);
	right_layout->addWidget(label);
	right_layout->addWidget(right_splitter);

	right_splitter->addWidget(ranges);
	right_splitter->addWidget(tabulated_tf);

	splitter->setChildrenCollapsible(false);
	right_splitter->setChildrenCollapsible(false);
	right_splitter->setOrientation(Qt::Vertical);

	substances->setSelectionMode(QAbstractItemView::SingleSelection);
	substances->setSelectionBehavior(QAbstractItemView::SelectRows);
	substances->verticalHeader()->setVisible(false);
	tabulated_tf->verticalHeader()->setVisible(false);

	label->setTextInteractionFlags(Qt::TextSelectableByMouse);
}

SubstancesTableView::~SubstancesTableView()
{

}

void SubstancesTableView::Initialize()
{
	connect(substances->selectionModel(),
			&QItemSelectionModel::selectionChanged,
			this, &SubstancesTableView::SelectionChanged);
	substances->setColumnHidden(0, true);
}

void SubstancesTableView::SetSubstancesTableModel(QAbstractItemModel* model)
{
	substances->setModel(model);
}

void SubstancesTableView::SetSubstancesTempRangeModel(QAbstractItemModel* model)
{
	ranges->setModel(model);
}

void SubstancesTableView::SetSubstancesTabulatedModel(QAbstractItemModel* model)
{
	tabulated_tf->setModel(model);
}

void SubstancesTableView::SetSelectedSubstanceLabel(const QString& name)
{
	label->setText(tr("Selected substance: <b>%1</b>").arg(name));
}

void SubstancesTableView::SelectionChanged(const QItemSelection& selected,
										   const QItemSelection& deselected)
{
	Q_UNUSED(deselected)
	if(!selected.isEmpty()) {
		auto&& index = selected.first().topLeft();
		constexpr auto id_col = static_cast<int>(SubstanceFields::ID);
		auto id = index.sibling(index.row(), id_col).data().toInt();
		LOG("id:", id)
		emit SelectSubstance(id);
	}
}
