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

#include "mainwindow.h"
#include "coreapplication.h"

#include <QApplication>
#include <QThread>

int main(int argc, char *argv[]) try
{
	QApplication a(argc, argv);

	QThread::currentThread()->setObjectName(QStringLiteral("<< GUI TREAD >>"));
	QThread thread;
	thread.setObjectName(QStringLiteral("<< CORE APP THREAD >>"));

	MainWindow main_window;
	CoreApplication core_app(&main_window);

	// connect with initialize need for open databases in core app thread
	QObject::connect(&thread, &QThread::started,
					 &core_app, &CoreApplication::Initialize);

	main_window.show();
	core_app.moveToThread(&thread);
	thread.start();
	int result = a.exec();
	thread.quit();
	thread.wait();
	return result;
} catch(std::exception& e) {
	qDebug() << e.what();
}

/* Kinds of signals-slots:
 *
 * 1. Double connected
 *		MainWindow						CoreApplication
 *		SendRequest (signal)	->		GetRequest (slot)
 *		GetResponse (slot)		<-		SendResponse (signal)
 *
 * 2. Single connected
 *
 *		2.1.
 *		MainWindow						CoreApplication
 *		ChangeModel (signal)	->		ChangeModel (slot)
 *
 *		2.2.
 *		MainWindow						CoreApplication
 *		ShowError (slot)		<-		ShowError (signal)
 *
 * 3. QActions and Menu
 *		QAction							MainMenu
 *		triggered (signal)		->		(slot)
 *
 * 4. Pass through (forwarding)
 *		MainWindow (ui)					MainWindow
 *		clicked (signal)		->		(signal)
 *
 *
 */

