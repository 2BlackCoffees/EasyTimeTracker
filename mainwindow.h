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
 *  EasyTimeTracker: First creation by 2BlackCoffees: http://www.twoblackcoffees.com/
 *
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "itimetrackingwindow.h"
#include "timetrackingwindow.h"
#include "observersubject.h"
#include "Preferences/preferencesdialog.h"
#include "debug.h"
#include "DialogTimeFollower/dialogtimefollower.h"

#include <QMainWindow>
#include <QMdiArea>

#include <vector>
#include <stdlib.h>
#include <boost/scoped_ptr.hpp>

using namespace std;

class QAction;
class QLabel;
class QDate;
class FindDialog;

class MainWindow : public QMainWindow, public IConnection
{
    Q_OBJECT

public:
    MainWindow();

public slots:
    virtual void somethingChanged();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void updateActions();
    void lockUnlock(bool unlock);

    void openSpreadsheet();
    void about();
    void openRecentSpreadsheet();
    void exportSpreadsheet();
    void printPreviewSpreadsheet();
    void printSpreadsheet();
    void preferences();
    void onShowDialogTimeFollower();

    void copy();
    void cut();
    void paste();

    void startTT();
    void stopTT();
    void pauseTT();

private:

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void updateTrackingActions();

    ITimeTrackingWindow* getActiveWindow() const;
    ITimeTrackingWindow* getCurrentMonthWindow(bool forceCreateNewWindow = false);

    void updateRecentFileActions();

    ITimeTrackingWindow * createSubWindow(const QString& userName = "",
                         const QDate& tmpDate = MiscDateTime::getInitDate());
    void updateTimer();

    boost::scoped_ptr<DialogTimeFollower>       mDialogTimeFollower;
    boost::scoped_ptr<TimeTrackingAction>       mTimeTrackingAction;
    QTimer *                                    mTimerDefaultAction;
    QTimer *                                    mTimerPause;

    ObserverSubject mSubject;

    QStringList     mRecentFiles;
    QString         mCurFile;
    QMdiArea*       mMdiArea;

    enum { MaxRecentFiles = 5 };
    vector<QAction*>* mRecentFileActions;
    QAction* mSeparatorAction;

    QMenu* mFileMenu;
    QMenu* mEditMenu;
    QMenu* mTimeTracker;
    QMenu* mHelpMenu;

    QToolBar* mFileToolBar;
    QToolBar* mEditToolBar;
    QToolBar* mTimeTrackerToolBar;

    QAction* mOpenSpreadSheet;
    QAction* mSaveSpreadSheet;
    QAction* mExportSpreadsheet;
    QAction* mPrintSpreadsheet;
    QAction* mPrintPreviewSpreadsheet;
    QAction* mExitAction;

    QAction* mCopyAction;
    QAction* mPasteAction;

    QAction* mModifySpreadSheet;
    QAction* mPreferences;
    QAction* mAboutAction;
    QAction* mAboutQtAction;

    QAction* mStartAction;
    QAction* mStopAction;
    QAction* mPauseAction;
    QAction* mShowDialogTimeFollower;

};

#endif
