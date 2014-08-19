/*
 *  This file is part of EasyTimeTracker.
 *
 *  EasyTimeTracker is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EasyTimeTracker is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with EasyTimeTracker.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  (C) by 2BlackCoffees: http://www.twoblackcoffees.com/
 *
 **/

#include "mainwindow.h"
#include <QApplication>
#include <qdebug.h>
#include <QSplashScreen>
#include <windows.h> // for Sleep

#include <QPixmap>
#include <QMainWindow>

class TTQApplication : public QApplication {
public:
    TTQApplication(int argc, char *argv[]) : QApplication(argc, argv) {

    }

    virtual bool notify(QObject * receiver, QEvent * event)
    {
        try
        {
            return QApplication::notify(receiver, event);
        }
        catch(std::exception& e)
        {
            qDebug() << "Exception thrown:" << e.what();
            QMessageBox::warning(NULL, "Exception thrown",
                                 e.what(),
                                 QMessageBox::Ok);
        }
        return false;
    }
};

#ifndef UNIT_TEST_MODE
int main(int argc, char *argv[])
{
    try {
        TTQApplication a(argc, argv);
#define SHOW_SPLASH
#ifdef SHOW_SPLASH
        QSplashScreen splash(QPixmap(":/images/tt_splash.png"));
        splash.show();
#endif
        a.processEvents();

#ifdef SHOW_SPLASH
        Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
        QString info = QObject::tr("Checkout http://www.twoblackcoffees.com   ***  ");
        for(int time = 0; time < 60; time++) {
            splash.showMessage(info,
                                topRight, Qt::white);
            info = info.mid(1, info.size() - 1) + info.at(0);
            Sleep(uint(70));
        }
#endif

        MainWindow *mainWin = new MainWindow;

        mainWin->show();

#ifdef SHOW_SPLASH
        splash.finish(mainWin);
#endif
        return a.exec();
    }
    catch(std::exception& e)
    {
        qDebug() << "Exception thrown:" << e.what();
        QMessageBox::warning(NULL, "Exception thrown",
                             e.what(),
                             QMessageBox::Ok);
    }

    return 0;

}
#else
#include "UnitTest/testeasytimetracker.h"
QTEST_MAIN(TestEasyTimeTracker)

#endif
