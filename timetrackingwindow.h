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

#ifndef TIMETRACKINGWINDOW_H
#define TIMETRACKINGWINDOW_H

#include "ui_mainwindow.h"
#include "iofileaccesses.h"
#include "iconnection.h"
#include "itimetrackingwindow.h"
#include "iobserver.h"
#include "observersubject.h"
#include "TimeTrackingAction/timetrackingaction.h"
#include "PDFCreator/pdfcreator.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintPreviewDialog>

#include <boost/scoped_ptr.hpp>

class QTableView;
class TableModel;
class TimeTrackingModel;

class TimeTrackingWindow : public ITimeTrackingWindow, public Ui::MainWindow,
        public IConnection, public IObserver
{
    Q_OBJECT
public:   
    explicit TimeTrackingWindow(ObserverSubject* subject,
                                const QString& userName = "",
                                const QDate& baseDate = MiscDateTime::getInitDate(),
                                IConnection* parent = NULL);

    virtual ~TimeTrackingWindow();

    virtual void copy()                                         const;
    virtual void cut();
    virtual void paste();

    virtual void startTT();
    virtual void stopTT();
    virtual void pauseTT();

    virtual bool isStarted() const;
    virtual bool isStopped() const;

    virtual void lockUnlock(bool unlock);
    virtual bool isSheetOfCurMonth()                              const;
    virtual void update();
    TimeTrackingModel * getTrackingModel()                        const;

    virtual void exportSheet(const QString &path)                 const;
    virtual void printSheet();
    virtual void printPreview();


public slots:
    void informSomethingChanged() {
        somethingChanged();
    }

    virtual void somethingChanged();
    void onCompensatedHours(const QString & string);
    virtual void resizeEvent(QResizeEvent* event);


private:
    QAction*                mAction;
    QTableView*             mTableView;
    IIOFileAccesses*        mIOFileAccesses;
    IConnection*            mParent;
    ObserverSubject*        mSubject;
    TableModel *            mTableModel;
    mutable QPrinter        mPrinter;
    bool                    mInPause;

    mutable boost::scoped_ptr<PDFCreator>   mPDFCreator;
    boost::scoped_ptr<QPrintPreviewDialog>  mPrintPreviewDialog;

    void adjustTableSize();
    void updateMainFields();
    bool openSpreadsheet(bool silent);
    void createContextMenu();
    
};

#endif // TIMETRACKINGWINDOW_H
