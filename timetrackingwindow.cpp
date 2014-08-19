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

#include "tablemodel.h"
#include "timetrackingwindow.h"
#include "iofileaccesses.h"
#include "tableviewdelegate.h"
#include "timetrackingmodel.h"
#include "Pause/pausedialog.h"
#include "Misc/disconnecttemporarily.h"

#include <QDir>
#include <QTableView>
#include <QModelIndex>
#include <QPrinter>
#include <QTextDocument>
#include <QClipboard>
#include <QPrintDialog>
#include <QDesktopWidget>

#include <assert.h>

TimeTrackingWindow::TimeTrackingWindow(ObserverSubject* subject,
                                       const QString& userName,
                                       const QDate& baseDate,
                                       IConnection* parent) :
    mParent(parent),
    mSubject(subject),
    mTableModel(NULL),
    mInPause(false)
{
    DEBUG_OBJ("");
    setupUi(this);
    mSubject->attach(this);

    mAction = new QAction(this);
    mAction->setCheckable(true);

    connect(mAction, SIGNAL(triggered()), this, SLOT(show()));
    connect(mAction, SIGNAL(triggered()), this, SLOT(setFocus()));

    setAttribute(Qt::WA_DeleteOnClose);
    Ui::MainWindow::lineEditCompenstedHours->setText("0:00");

    mTableView = Ui::MainWindow::mTableView;
    mTableModel = new TableModel(this);

    bool silent = false;
    if(baseDate != MiscDateTime::getInitDate()) {
        mTableModel->setNewBaseDate(baseDate);
    } else {
        silent = true;
    }

    mIOFileAccesses = new IOFileAccesses(mTableModel->getViewModel()->getTimeTrackingModel());
    //mIOFileAccesses->setPreviousBalance();
    mTableView->setModel(mTableModel->getViewModel());
    mTableView->setItemDelegate(new TableViewDelegate(mTableView));

    if(!userName.isEmpty()) {
        mTableModel->setUserName(userName);
    } else {
        mTableModel->setUserName(Misc::getUserName());
    }



    // If a file exists read it otherwise set all initialisations
    QDir::setCurrent(QDir::homePath());

    // When saving a prevously created sheet all next sheets must be updated!
    if(!openSpreadsheet(silent))
    {
        DEBUG("Setting default as file could not be opened: " + mTableModel->getViewModel()->getTimeTrackingModel()->getDefaultFileName());
        updateMainFields();
    }

    createContextMenu();

    connect(Ui::MainWindow::lineEditCompenstedHours, SIGNAL (returnPressed()),
            this, SLOT(somethingChanged()));
    connect(Ui::MainWindow::lineEditCompenstedHours,
            SIGNAL (textChanged(const QString &)),
            this, SLOT(onCompensatedHours(const QString &)));
    connect(mTableModel->getViewModel()->getTimeTrackingModel(),
            SIGNAL(somethingChanged()), this, SLOT(informSomethingChanged()));



}

// =============================================================================
TimeTrackingWindow::~TimeTrackingWindow() {
    mSubject->detach(this);

}

// =============================================================================
void TimeTrackingWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    adjustTableSize();
}

// =============================================================================
void TimeTrackingWindow::adjustTableSize()
{
    int columnCount = mTableModel->getViewModel()->columnCount(QModelIndex());
    int fullColumnWidth = 0;
    for(int column = 0; column < columnCount; ++column) {
        if(column != Col::COMMENT) {
            fullColumnWidth += mTableView->columnWidth(column);
        }
    }
    mTableView->setColumnWidth(Col::COMMENT,
                               size().width() - fullColumnWidth -
                               mTableView->verticalHeader()->width() - 42 /* slide bar */);
}

// =============================================================================
void TimeTrackingWindow::updateMainFields() {

    DEBUG("updateMainFields: " + MiscDateTime::secondsToHours(mTableModel->getViewModel()->getTimeTrackingModel()->getPrevMonthlyOverDueSeconds()));
    Ui::MainWindow::userNameEdit->setText(mTableModel->getViewModel()->getTimeTrackingModel()->getUserName());
    Ui::MainWindow::monthLineEdit->setText(mTableModel->getViewModel()->getTimeTrackingModel()->getBaseDate().toString("MMMM"));
    Ui::MainWindow::yearLineEdit->setText(mTableModel->getViewModel()->getTimeTrackingModel()->getBaseDate().toString("yyyy"));
    Ui::MainWindow::lineEditPreviousBalance->setText(MiscDateTime::secondsToHours(mTableModel->getViewModel()->getTimeTrackingModel()->getPrevMonthlyOverDueSeconds()));
    Ui::MainWindow::lineEditCompenstedHours->setText(MiscDateTime::secondsToHours(mTableModel->getViewModel()->getTimeTrackingModel()->getCompensatedOverDueSeconds()));
}

// =============================================================================
bool TimeTrackingWindow::openSpreadsheet(bool silent) {

//    SomethingChangedDisconnectTemporarily somethingChangedDisconnectTemporarily(mTableModel->getViewModel()->getTimeTrackingModel(), this);
    // TODO: Change this with a smart pointer
    /*disconnect(mTableModel->getViewModel()->getTimeTrackingModel(),
            0, this, 0);*/
    std::unique_ptr<DisconnectTemporarily> disconnectTemporarily(
                new DisconnectTemporarily(
                    mTableModel->getViewModel()->getTimeTrackingModel(), SIGNAL(somethingChanged()),
                    this, SLOT(informSomethingChanged())));
    bool returnValue = false;
    if(mIOFileAccesses->openSheetInformation(NULL, silent)) {

        updateMainFields();
        somethingChanged();
        returnValue = true;

    }
    /*connect(mTableModel->getViewModel()->getTimeTrackingModel(),
            SIGNAL(somethingChanged()), this, SLOT(informSomethingChanged()));*/

    return returnValue;

}

// =============================================================================
void TimeTrackingWindow::update() {
    DEBUG("Calling update for " +mTableModel->getViewModel()->getTimeTrackingModel()->getBaseDate().toString("yyyy-MM"));

    openSpreadsheet(false);
}

// =============================================================================
void TimeTrackingWindow::somethingChanged() {

    DEBUG("Checing month time for month " + mTableModel->getViewModel()->getTimeTrackingModel()->getBaseDate().toString("MMM / yyyy"));
    Ui::MainWindow::lineEditMonthlyHours->setText(MiscDateTime::secondsToHours(
                                                      mTableModel->getViewModel()->getTimeTrackingModel()->getMonthlySeconds()));

    QString compensatedHours = "0";
    if(!Ui::MainWindow::lineEditCompenstedHours->text().isEmpty()) {
        compensatedHours = Ui::MainWindow::lineEditCompenstedHours->text();
    }
    try {
        mTableModel->getViewModel()->getTimeTrackingModel()->setCompensatedOverDueSeconds(MiscDateTime::stringHoursToSeconds(compensatedHours));
    } catch (TTExceptionWrongBaseTime& e) {
        QMessageBox::warning(this, tr("Bad entry in compensated hours"),
                             e.what(),
                             QMessageBox::Ok);
        Ui::MainWindow::lineEditCompenstedHours->setText("0:00");
    }

    Ui::MainWindow::lineEditBalance->setText(MiscDateTime::secondsToHours(mTableModel->getViewModel()->getTimeTrackingModel()->getMonthlyOverDueSeconds()));
    mIOFileAccesses->saveSheetInformation();

    if(!mSubject->notifying()) {
        DEBUG("Notifying all");
        mSubject->notifyAll(this);
    }
}

// =============================================================================
void TimeTrackingWindow::onCompensatedHours(const QString & string)
{
    Q_UNUSED(string);
    somethingChanged();
}

// =============================================================================
void TimeTrackingWindow::createContextMenu()
{
#ifdef CONTEXT_MENU
    addAction(mCopyAction);
    addAction(mPasteAction);
    //mSpreadsheet->addSeparator();
    addAction(mStartAction);
    addAction(mStopAction);
    addAction(mPauseAction);
    //mSpreadsheet->addSeparator();
    addAction(mModifySpreadSheet);
    addAction(mPreferences);
    setContextMenuPolicy(Qt::ActionsContextMenu);
#endif
}

// =============================================================================
void TimeTrackingWindow::startTT() {
    try {
        mTableModel->startTT();
        somethingChanged();
    } catch (std::exception& e) {
        QMessageBox::warning(this, tr("Could not start tracking time"),
                             e.what());

    }
}

// =============================================================================
void TimeTrackingWindow::stopTT() {
    try {
        mTableModel->stopTT();
        somethingChanged();
    } catch (std::exception& e) {
        QMessageBox::warning(this, tr("Could not end tracking time"),
                             e.what());

    }
}

// =============================================================================
bool TimeTrackingWindow::isSheetOfCurMonth() const {
    return mTableModel->sheetOfCurMonth();
}

// =============================================================================
bool TimeTrackingWindow::isStarted() const {
    return mTableModel->isStarted();
}

// =============================================================================
bool TimeTrackingWindow::isStopped() const {
    return mTableModel->isStopped();
}

// =============================================================================
void TimeTrackingWindow::pauseTT() {
    if(mInPause) return;
    mInPause = true;
    PauseDialog pauseDialog(NULL, getTrackingModel()->getPause());
    pauseDialog.exec();
    if(pauseDialog.isPauseValid()) {
        getTrackingModel()->setPause(pauseDialog.getPause());
    }
    mInPause = false;
}

// =============================================================================
void TimeTrackingWindow::lockUnlock(bool unlock) {
    //mTableView->lockUnlock(unlock);
}

// =============================================================================
TimeTrackingModel *TimeTrackingWindow::getTrackingModel() const {
    assert(mTableModel->getViewModel()->getTimeTrackingModel());
    return mTableModel->getViewModel()->getTimeTrackingModel();
}

// =============================================================================
void TimeTrackingWindow::exportSheet(const QString & path) const
{
    PDFCreator pdfCreator(mTableView, getTrackingModel());
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(path);
    printer.setOutputFormat(QPrinter::PdfFormat);
#ifdef PDF_WITH_QTEXTDOCUMENT
    QTextDocument document;
    pdfCreator.populateDocument(&document);
    document.print(&printer);
#else
    pdfCreator.paintPage(&printer, true);
#endif

}

// =============================================================================
void TimeTrackingWindow::printSheet()
{
    QPrintDialog dialog(&mPrinter, this);
    if (dialog.exec()) {
        PDFCreator pdfCreator(mTableView, getTrackingModel());
        pdfCreator.paintPage(&mPrinter, false);
    }

}

// =============================================================================
void TimeTrackingWindow::printPreview()
{
    mPrintPreviewDialog.reset(new QPrintPreviewDialog(&mPrinter, this));
    QSize size = qApp->desktop()->availableGeometry().size();
    size.rwidth() /= 2;
    mPrintPreviewDialog->resize(size);
    mPDFCreator.reset(new PDFCreator(mTableView, getTrackingModel()));
    connect(mPrintPreviewDialog.get(), SIGNAL(paintRequested(QPrinter*)),
            mPDFCreator.get(), SLOT(paintPage(QPrinter*)));
    mPrintPreviewDialog->open();
}

// =============================================================================
void TimeTrackingWindow::copy() const
{
    QItemSelectionModel * itemSelectionModel = mTableView->selectionModel();
    QModelIndexList modelIndexList = itemSelectionModel->selectedIndexes();
    if (modelIndexList.isEmpty()) {
        QMessageBox::warning(NULL, tr("Cannot copy empty selection"),
                             "Please select an area before copying.",
                             QMessageBox::Ok);
        return;
    }

    QString copiedData = mTableView->model()->data(QModelIndex(),
                                                 Qt::UserRole + TableViewModel::USER_ROLE_GET_MONTH_OF_SHEET).toString();

    // Extract coordinates of rectangle selection
    QPoint leftTop = QPoint(mTableView->model()->rowCount(), mTableView->model()->columnCount());
    QPoint rightBottom = QPoint(0, 0);

    for (int idx = 0; idx < modelIndexList.count(); idx++) {
        QModelIndex index = modelIndexList[idx];
        leftTop = QPoint(std::min(leftTop.x(), index.column()),
                         std::min(leftTop.y(), index.row()));
        rightBottom = QPoint(std::max(rightBottom.x(), index.column()),
                             std::max(rightBottom.y(), index.row()));
    }

    // Create header
    for(int col = leftTop.x(); col <= rightBottom.x(); col++) {
        copiedData += "\t|" + mTableView->model()->headerData(col, Qt::Horizontal).toString();
    }
    // Copy selection
    for(int row = leftTop.y(); row <= rightBottom.y(); row++) {
        copiedData += "\n\t";
        for(int col = leftTop.x(); col <= rightBottom.x(); col++) {
            QModelIndex index = mTableView->model()->index(row, col);
            copiedData += "\t|" + mTableView->model()->data(index).toString();
        }
    }


    QApplication::clipboard()->setText(copiedData);
}

// =============================================================================
void TimeTrackingWindow::paste() {

    QString str = QApplication::clipboard()->text();
    QStringList rowList = str.split('\n');
    QItemSelectionModel * itemSelectionModel = mTableView->selectionModel();

    int colInit = itemSelectionModel->currentIndex().column();
    int rowInit = itemSelectionModel->currentIndex().row();
    int maxColList = 0;
    for(int row = 1; row < rowList.size(); row++) {
        DEBUG(QString("paste: rowString %1 - row: %2").arg(rowList.at(row)).arg(row));

        QStringList preColList = rowList.at(row).split('|');
        maxColList = std::max(preColList.size() - 1, maxColList);
        for(int col = 1; col < preColList.size(); col++) {
            QString string = preColList.at(col);
            string = string.remove(QRegExp("\\s+$"));

            int curCol = colInit + col - 1;
            int curRow = rowInit + row - 1;
            QVariant variant;
            if(curCol != Col::COMMENT)  variant = MiscDateTime::stringToTime(string);
            else                        variant = string;
            DEBUG(QString("paste: variant %1, string %4 - curCol: %2, curRow: %3").arg(variant.toString()).arg(curCol).arg(curRow).arg(string));

            mTableView->model()->setData(mTableView->model()->index(curRow, curCol), variant);
        }
    }
    emit mTableView->model()->dataChanged(mTableView->model()->index(rowInit, colInit),
                     mTableView->model()->index(rowInit + rowList.size(), colInit + maxColList));
}

// =============================================================================
void TimeTrackingWindow::cut() {

}


