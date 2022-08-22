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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractItemView>
#include <QProgressDialog>
#include <QFuture>
#include <QtDataVisualization/QSurfaceDataArray>
#include "plots.h"
#include "heavycontainer.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtDataVisualization;
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(MainWindow)

private:
	Ui::MainWindow *ui;
	QProgressDialog* dialog;
	QString database_path;
	QFutureWatcher<void>* fw;

public:
	MainWindow(QWidget *parent = nullptr);
	virtual ~MainWindow() override;

	QProgressDialog* GetProgressDialog() { return dialog; }
	void SetModel_1(QAbstractItemModel* model);
	void SetModel_2(QAbstractItemModel* model);
	void SetSelectonModel(QItemSelectionModel* selection);

public slots:
	void SlotHeavyComputations(QVector<HeavyContainer>& ho);
	void SlotShowResponse(const QString& text);
	void SlotShowRequest(const QString& text);
	void SlotShowError(const QString& text);
	void SlotShowStatusBarText(const QString& text);
	void SlotAdd2DGraph(const GraphId& id, QVector<double>& x, QVector<double>& y);
	void SlotAddHeatMap(const QString& name, QVector<double>& x,
						QVector<double>& y, QVector<QVector<double>>& z);
	void SlotAdd3DGraph(QSurfaceDataArray* data);
	void SlotLoadDatabase();

signals:
	void SignalSendRequest(int);
	void SignalPushButtonClicked(QString);
	void SignalHeavyCalculationsStart();
	void SignalNeed2DGraphData();
	void SignalNeedHeatMapData();
	void SignalNeed3DGraphData();

private slots:
	void MenuOpenDatabase();
	void MenuShowAbout();

private:
	void SetupMenu();
	void CheckButtonHandler();
	void PushButtonHandler();
	void PushButtonHeavyHandler();

};
#endif // MAINWINDOW_H
