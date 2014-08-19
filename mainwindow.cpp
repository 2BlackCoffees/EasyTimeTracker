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
#include "Settings/iosettings.h"
#include "timetrackingmodel.h"
#include "DialogAbout/dialogabout.h"

#include <QUrl>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QCloseEvent>

#include <assert.h>
// TODO; Add status bar defining when next action will be started, + hours status,
// TODO: Button browse in preferences
// TODO: When adding actions they are immiediately saved whereas they should be saved only when clicking on OK and cancelled when clicking on cancel - same thing for hours configuration
// TODO: When starting application start current day, when closing application stop current day
// TODO: When starting, previous balance should be computed based on all found files (Search files with regexp) and extract/update all balances
// TODO: Personalisation des noms et prenoms et pied de page dans preference pour l'impression
// TODO: When starting a new sheet with button start, the balance is not updated
// TODO: It should be possible to click start when new month started: a new sheet should open
// TODO: on doit pouvoir effacer une ligne de données
// TODO: Allow to set a line as holiday or full compensation day

MainWindow::MainWindow() :mTimeTrackingAction(new TimeTrackingAction()),  mMdiArea(NULL)
{

    DEBUG_OBJ("");
    setWindowIcon(QIcon(":/images/tt_icon.png"));
    DEBUG("Calling updateAllBalances");
    IOFileAccesses::updateAllBalances(Misc::getUserName());

    IOSettings settings;
    // First start do not allow access to perdiodic actions as the user needs to configure them
    if(!settings.readSettings(TimeTrackingSettings::getInstance())) {
        TimeTrackingSettings::getInstance()->
                    setEnablePeriodDefaultAction(false);

    }

    mMdiArea = new QMdiArea();
    setCentralWidget(mMdiArea);

    ITimeTrackingWindow * curWindow = createSubWindow();
    bool started = curWindow->isStarted();
    bool stopped = curWindow->isStopped();
    mTimeTrackingAction->setUserCurrentlyWorking(started && !stopped);

    connect(mMdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateActions()));


    mRecentFileActions = new vector<QAction*>(MaxRecentFiles);

    // All initialisations
    createActions();
    createMenus();
    createToolBars();

    mDialogTimeFollower.reset(new DialogTimeFollower(NULL, curWindow->getTrackingModel()));
    mDialogTimeFollower->show();
    connect(mDialogTimeFollower.get(), SIGNAL(start()),   mStartAction,   SLOT(trigger()));
    connect(mDialogTimeFollower.get(), SIGNAL(stop()),    mStopAction,    SLOT(trigger()));
    connect(mDialogTimeFollower.get(), SIGNAL(pause()),   mPauseAction,   SLOT(trigger()));
}

// =============================================================================
void MainWindow::openSpreadsheet()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Time tracking"),
                                                    QDir::homePath(),
                                                    tr("Time tracking files (*.xml)"));
    if (!fileName.isEmpty()) {
        fileName.remove(QRegExp("\\.\\w+$"));
        DEBUG("Opening file " + fileName);
        QStringList stringList = fileName.split('-');
        if(stringList.size() != 3) {

            QMessageBox::warning(this, tr("Invalid filename"),
                                 "File " + fileName + " could not be loaded.",
                                 QMessageBox::Ok);
        }
        QString userName = stringList.at(0);
        QDate tmpDate(stringList.at(1).toInt(), stringList.at(2).toInt(), 1);

        createSubWindow(userName, tmpDate);

    }
}

// =============================================================================
ITimeTrackingWindow * MainWindow::createSubWindow(const QString& userName, const QDate& tmpDate) {
    DEBUG_OBJ("");

    ITimeTrackingWindow * timeTrackingWindow = new TimeTrackingWindow(&mSubject, userName, tmpDate, this);
    DEBUG("Opening subWindow " + userName + ", date: " + tmpDate.toString());
    QMdiSubWindow *subWindow = mMdiArea->addSubWindow(timeTrackingWindow);
    // TODO Update events in MDI
    // *windowMenu->addAction(editor->windowMenuAction());
    //  windowActionGroup->addAction(editor->windowMenuAction());

    subWindow->show();
    return timeTrackingWindow;
}

// =============================================================================
void MainWindow::about()
{
    DialogAbout dialogAbout(this);
    dialogAbout.exec();
}

// =============================================================================
void MainWindow::openRecentSpreadsheet()
{
}

// =============================================================================
void MainWindow::copy() {
    ITimeTrackingWindow* curWindow = getActiveWindow();
    if(curWindow != NULL) {
        curWindow->copy();
    }
}

// =============================================================================
void MainWindow::cut() {
    ITimeTrackingWindow* curWindow = getActiveWindow();
    if(curWindow != NULL) {
        curWindow->cut();
    }
}

// =============================================================================
void MainWindow::paste() {
    ITimeTrackingWindow* curWindow = getActiveWindow();
    if(curWindow != NULL) {
        curWindow->paste();
    }
}

// =============================================================================
void MainWindow::startTT() {
    ITimeTrackingWindow* curWindow = getCurrentMonthWindow(true);
    if(curWindow != NULL) {
        curWindow->startTT();
    }
}

// =============================================================================
void MainWindow::stopTT() {
    ITimeTrackingWindow* curWindow = getCurrentMonthWindow();
    if(curWindow != NULL) {
        curWindow->stopTT();
    }
}

// =============================================================================
void MainWindow::pauseTT() {
    ITimeTrackingWindow* curWindow = getCurrentMonthWindow();
    if(curWindow != NULL) {
        curWindow->pauseTT();
    }
}

// =============================================================================
void MainWindow::lockUnlock(bool unlock) {
    Q_UNUSED(unlock);
}

// =============================================================================
void MainWindow::createActions()
{
    mCopyAction = new QAction(tr("&Copy"), this);
    mCopyAction->setIcon(QIcon(":/images/tt_copy.png"));
    mCopyAction->setShortcut(QKeySequence::Copy);
    mCopyAction->setStatusTip(tr("Copy the current selection's contents "
                                 "to the clipboard"));
    connect(mCopyAction, SIGNAL(triggered()), this, SLOT(copy()));

    mPasteAction = new QAction(tr("&Paste"), this);
    mPasteAction->setIcon(QIcon(":/images/tt_paste.png"));
    mPasteAction->setShortcut(QKeySequence::Paste);
    mPasteAction->setStatusTip(tr("Paste the clipboard's contents "
                                  "to the spread sheet"));
    connect(mPasteAction, SIGNAL(triggered()), this, SLOT(paste()));


    mStartAction = new QAction(tr("&Start"), this);
    mStartAction->setIcon(QIcon(":/images/tt_start.png"));
    mStartAction->setShortcut(QKeySequence::ZoomIn);
    mStartAction->setStatusTip(tr("Start counting"));
    connect(mStartAction, SIGNAL(triggered()), this, SLOT(startTT()));
    connect(mStartAction, SIGNAL(triggered()), this, SLOT(updateActions()));
    connect(mStartAction, SIGNAL(triggered()), mTimeTrackingAction.get(), SLOT(startTT()));

    mStopAction = new QAction(tr("&Stop"), this);
    mStopAction->setIcon(QIcon(":/images/tt_stop.png"));
    mStopAction->setShortcut(QKeySequence::ZoomOut);
    mStopAction->setStatusTip(tr("Stop counting"));
    connect(mStopAction, SIGNAL(triggered()), this, SLOT(stopTT()));
    connect(mStopAction, SIGNAL(triggered()), this, SLOT(updateActions()));
    connect(mStopAction, SIGNAL(triggered()), mTimeTrackingAction.get(), SLOT(stopTT()));

    mPauseAction = new QAction(tr("&Pause"), this);
    mPauseAction->setIcon(QIcon(":/images/tt_pause.png"));
    mPauseAction->setShortcut(QKeySequence::ZoomOut);
    mPauseAction->setStatusTip(tr("Set pause"));
    connect(mPauseAction, SIGNAL(triggered()), mTimeTrackingAction.get(), SLOT(pauseTT()));
    connect(mPauseAction, SIGNAL(triggered()), this, SLOT(pauseTT()));

    mShowDialogTimeFollower = new QAction(tr("&Tracker"), this);
    mShowDialogTimeFollower->setIcon(QIcon(":/images/tt_glass.png"));
    mShowDialogTimeFollower->setShortcut(QKeySequence::ZoomIn);
    mShowDialogTimeFollower->setStatusTip(tr("Show small tracking window"));
    connect(mShowDialogTimeFollower, SIGNAL(triggered()), this, SLOT(onShowDialogTimeFollower()));

    mOpenSpreadSheet = new QAction(tr("&Open..."), this);
    mOpenSpreadSheet->setIcon(QIcon(":/images/tt_open.png"));
    mOpenSpreadSheet->setShortcut(QKeySequence::Open);
    mOpenSpreadSheet->setStatusTip(tr("Open an existing time tracking file"));
    connect(mOpenSpreadSheet, SIGNAL(triggered()), this, SLOT(openSpreadsheet()));

    mExportSpreadsheet = new QAction(tr("&Export"), this);
    mExportSpreadsheet->setIcon(QIcon(":/images/tt_export.png"));
    mExportSpreadsheet->setShortcut(QKeySequence::SelectNextWord);
    mExportSpreadsheet->setStatusTip(tr("Export current time tracking information to PDF"));
    connect(mExportSpreadsheet, SIGNAL(triggered()), this, SLOT(exportSpreadsheet()));

    mPrintPreviewSpreadsheet = new QAction(tr("P&rint preview"), this);
    mPrintPreviewSpreadsheet->setIcon(QIcon(":/images/tt_print_preview.png"));
    mPrintPreviewSpreadsheet->setStatusTip(tr("Preview current sheet"));
    connect(mPrintPreviewSpreadsheet, SIGNAL(triggered()), this, SLOT(printPreviewSpreadsheet()));

    mPrintSpreadsheet = new QAction(tr("&Print"), this);
    mPrintSpreadsheet->setIcon(QIcon(":/images/tt_print.png"));
    mPrintSpreadsheet->setStatusTip(tr("Print current sheet"));
    connect(mPrintSpreadsheet, SIGNAL(triggered()), this, SLOT(printSpreadsheet()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        QAction* tmpRecentFileActions = new QAction(this);
        tmpRecentFileActions->setVisible(false);
        connect(tmpRecentFileActions, SIGNAL(triggered()),
                this, SLOT(openRecentSpreadsheet()));
        mRecentFileActions->push_back(tmpRecentFileActions);
    }

    mExitAction = new QAction(tr("E&xit"), this);
    mExitAction->setShortcut(tr("Ctrl+Q"));
    mExitAction->setStatusTip(tr("Exit the application"));
    mExitAction->setIcon(QIcon(":/images/tt_cross.png"));
    connect(mExitAction, SIGNAL(triggered()), this, SLOT(close()));

    mAboutAction = new QAction(tr("&About"), this);
    mAboutAction->setIcon(QIcon(":/images/tt_2black_coffees_bk_gnd_30x30.png"));
    mAboutAction->setStatusTip(tr("Show the application's About box"));
    connect(mAboutAction, SIGNAL(triggered()), this, SLOT(about()));

    mModifySpreadSheet = new QAction(tr("&Lock/Unlock Sheet"), this);
    mModifySpreadSheet->setIcon(QIcon(":/images/tt_unlock.png"));
    mModifySpreadSheet->setStatusTip(tr("Allow modification on spreadsheet (Lock/Unlock)"));
    mModifySpreadSheet->setCheckable(true);
    connect(mModifySpreadSheet, SIGNAL(toggled(bool)), this, SLOT(lockUnlock(bool)));

    mPreferences = new QAction(tr("&Preferences..."), this);
    mPreferences->setStatusTip(tr("Change settings of the program"));
    mPreferences->setIcon(QIcon(":/images/tt_preferences.png"));
    connect(mPreferences, SIGNAL(triggered()), this, SLOT(preferences()));

    mAboutQtAction = new QAction(tr("About &Qt"), this);
    mAboutQtAction->setStatusTip(tr("Qt library's About box"));
    connect(mAboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

// =============================================================================
void MainWindow::updateActions() {

    ITimeTrackingWindow* curWindow = getActiveWindow();

    // TODO: Use here an observer: if any window started then the action should be false
    if(curWindow != NULL) {
        bool started = curWindow->isStarted();
        bool stopped = curWindow->isStopped();
        mStartAction->setEnabled(!started || stopped);
        mStopAction->setEnabled(started && !stopped);
    }
}

// =============================================================================
void MainWindow::createMenus()
{
    mFileMenu = QMainWindow::menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mOpenSpreadSheet);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mExportSpreadsheet);
    mFileMenu->addAction(mPrintPreviewSpreadsheet);
    mFileMenu->addAction(mPrintSpreadsheet);

    mFileMenu->addSeparator();

    mFileMenu->addAction(mExitAction);

    mEditMenu = QMainWindow::menuBar()->addMenu(tr("&Edit"));
    mEditMenu->addAction(mCopyAction);
    mEditMenu->addAction(mPasteAction);
    mEditMenu->addAction(mModifySpreadSheet);
    mEditMenu->addAction(mPreferences);


    mTimeTracker = QMainWindow::menuBar()->addMenu(tr("&Time tracker"));
    mTimeTracker->addAction(mStartAction);
    mTimeTracker->addAction(mStopAction);
    mTimeTracker->addAction(mPauseAction);
    mTimeTracker->addAction(mShowDialogTimeFollower);

    QMainWindow::menuBar()->addSeparator();

    mHelpMenu = QMainWindow::menuBar()->addMenu(tr("&Help"));
    mHelpMenu->addAction(mAboutAction);
    mHelpMenu->addAction(mAboutQtAction);

}

// =============================================================================
void MainWindow::createToolBars()
{
    mFileToolBar = addToolBar(tr("&File"));
    mFileToolBar->addAction(mOpenSpreadSheet);
    mFileToolBar->addAction(mExportSpreadsheet);
    mFileToolBar->addAction(mPrintPreviewSpreadsheet);
    mFileToolBar->addAction(mPrintSpreadsheet);

    mEditToolBar = addToolBar(tr("&Edit"));
    mEditToolBar->addAction(mModifySpreadSheet);
    mEditToolBar->addAction(mPreferences);

    mTimeTrackerToolBar = addToolBar(tr("&Time tracker"));
    mTimeTrackerToolBar->addAction(mStartAction);
    mTimeTrackerToolBar->addAction(mStopAction);
    mTimeTrackerToolBar->addAction(mPauseAction);
    mTimeTrackerToolBar->addAction(mShowDialogTimeFollower);

}

// =============================================================================
void MainWindow::updateRecentFileActions()
{
    QMutableStringListIterator i(mRecentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < MaxRecentFiles; ++j) {
        if (j < mRecentFiles.count()) {
            QString text = tr("&%1 %2")
                    .arg(j + 1)
                    .arg(mRecentFiles[j]);
            (mRecentFileActions->at(j))->setText(text);
            (mRecentFileActions->at(j))->setData(mRecentFiles[j]);
            (mRecentFileActions->at(j))->setVisible(true);
        } else {
            (mRecentFileActions->at(j))->setVisible(false);
        }
    }
    mSeparatorAction->setVisible(!mRecentFiles.isEmpty());
}

// =============================================================================
void MainWindow::exportSpreadsheet() {
    ITimeTrackingWindow* curWindow = getActiveWindow();

    if(curWindow != NULL) {
        QString path = QFileDialog::getSaveFileName(this, tr("Export a PDF file"),
                                     curWindow->getTrackingModel()->getDefaultFileName("", true /* export to PDF */),
                                     "PDF (*.pdf)");
        if(!path.isEmpty()) {
            if(!path.endsWith(".pdf")) path += ".pdf";
            curWindow->exportSheet(path);
        }
    }
}

// =============================================================================
void MainWindow::printSpreadsheet() {
    ITimeTrackingWindow* curWindow = getActiveWindow();

    if(curWindow != NULL) {
        curWindow->printSheet();
    }
}

// =============================================================================
void MainWindow::printPreviewSpreadsheet() {
    ITimeTrackingWindow* curWindow = getActiveWindow();

    if(curWindow != NULL) {
        curWindow->printPreview();
    }
}

// =============================================================================
void MainWindow::preferences()
{
    PreferencesDialog preferences(this);
    preferences.exec();

    // Update all time tracking timers
    updateTrackingActions();
}

// =============================================================================
void MainWindow::updateTrackingActions() {
    bool userCurrentlyWorking = false;
    ITimeTrackingWindow* curWindow = getActiveWindow();
    if(curWindow != NULL) {
        bool started = curWindow->isStarted();
        bool stopped = curWindow->isStopped();
        userCurrentlyWorking = started && !stopped;
    }
    mTimeTrackingAction->setUserCurrentlyWorking(userCurrentlyWorking);
}

// =============================================================================
void MainWindow::onShowDialogTimeFollower()
{
    mDialogTimeFollower->show();
}

// =============================================================================
ITimeTrackingWindow* MainWindow::getActiveWindow() const
{
    QMdiSubWindow *subWindow = mMdiArea->activeSubWindow();
    if (subWindow) {
        return qobject_cast<TimeTrackingWindow*>(subWindow->widget());
    }
    return NULL;
}

// =============================================================================
ITimeTrackingWindow* MainWindow::getCurrentMonthWindow(bool forceCreateNewWindow) {
    QList<QMdiSubWindow *> subWindowList = mMdiArea->subWindowList();
    foreach(QMdiSubWindow * subWindow, subWindowList) {
        if(qobject_cast<TimeTrackingWindow*>(subWindow->widget())->isSheetOfCurMonth()) {
            return qobject_cast<TimeTrackingWindow*>(subWindow->widget());
        }
    }

    if(forceCreateNewWindow == false && 
	   QMessageBox::question(NULL, tr("Sheet of current month is not open"),
                             tr("The sheet of current month is not open, you can either open an existing sheet or apply a new one"),
                             QMessageBox::Open, QMessageBox::Apply) == QMessageBox::Open) {
        openSpreadsheet();
    } else {
        createSubWindow();
    }
    ITimeTrackingWindow * timeTrackingWindow = getActiveWindow();
    if(!timeTrackingWindow->isSheetOfCurMonth()) return NULL;
    return timeTrackingWindow;
}

// =============================================================================
// TODO: Check if hierarchy IConnection can be removed from MainWindow
void MainWindow::somethingChanged() {

}

// =============================================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
    IOSettings settings;
    settings.saveSettings(TimeTrackingSettings::getInstance());

    mMdiArea->closeAllSubWindows();
    if (!mMdiArea->subWindowList().isEmpty()) {
        event->ignore();
    } else {
        event->accept();
    }
}

