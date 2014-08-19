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

#include "PDFCreator/pdfcreator.h"
#include "misc.h"
#include "timetrackingsettings.h"

#include <timetrackingmodel.h>
#include <QPainter>
#include <QPrinter>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTableView>
#include <QTextTable>


// =============================================================================
PDFCreator::PDFCreator(QTableView * tableView, TimeTrackingModel * timeTrackingModel)
    : QObject(), mTableView(tableView), mTimeTrackingModel(timeTrackingModel)
{
}

// =============================================================================
void PDFCreator::paintPage(QPrinter * printer, bool noUserInteraction)
{
    if (noUserInteraction)
        printer->setPageMargins(25, 25, 25, 25, QPrinter::Millimeter);
    QPainter painter(printer);

   QRect freeSpaceRect = paintTitle(&painter);
   const int nbRows = 33;
   int cellHeight = freeSpaceRect.height() / nbRows;
   if(TimeTrackingSettings::getInstance()->getPrintingShowPicture()) {
       paintImage(&painter, freeSpaceRect, cellHeight);
   }
   paintTable(&painter, freeSpaceRect, cellHeight);

}

// =============================================================================
QRect PDFCreator::paintTitle(QPainter * painter)
{
    // Draw Title
    painter->setFont(QFont("Helvetica", 14, QFont::Bold));
    QRect rectTitle(0, 0, painter->viewport().width(), painter->fontMetrics().height());
    int posYTitle = painter->fontMetrics().lineSpacing() * 3;

    QString userName = TimeTrackingSettings::getInstance()->getPrintingRealName();
    if(userName.isEmpty()) {
        userName = mTimeTrackingModel->getUserName();
    } else {
        userName += " (" + mTimeTrackingModel->getUserName() + ")";
    }

    painter->drawText(rectTitle, "Timesheet of " + userName, QTextOption(Qt::AlignCenter));
    QDate date = mTimeTrackingModel->getBaseDate();
    painter->setFont(QFont("Helvetica", 13));
    rectTitle.moveTop(posYTitle / 2);
    painter->drawText(rectTitle, "Date " + date.toString("MMMM yyyy"), QTextOption(Qt::AlignLeft));

    // Draw Foot
    painter->setFont(QFont("Helvetica", 10));
    int posYFoot = painter->viewport().height() - posYTitle -
            painter->fontMetrics().lineSpacing() * 2;

    QRect rectFoot( 0, posYFoot, painter->viewport().width(), painter->fontMetrics().height());
    painter->drawText(rectFoot, "Prev. Balance: " +
                      MiscDateTime::secondsToHours(mTimeTrackingModel->getPrevMonthlyOverDueSeconds()),
                      QTextOption(Qt::AlignLeft));
    painter->drawText(rectFoot, "Monthly hours: " +
                      MiscDateTime::secondsToHours(mTimeTrackingModel->getMonthlySeconds()) +
                      " - Compensated Hours: " +
                      MiscDateTime::secondsToHours(mTimeTrackingModel->getCompensatedOverDueSeconds()),
                      QTextOption(Qt::AlignCenter));
    painter->drawText(rectFoot, "Balance: " +
                      MiscDateTime::secondsToHours(mTimeTrackingModel->getMonthlyOverDueSeconds()),
                      QTextOption(Qt::AlignRight));

    // TODO: Make this one optional
    rectFoot.moveTop(rectFoot.y() + painter->fontMetrics().lineSpacing());
    painter->setFont(QFont("Helvetica", 8, QFont::StyleItalic));
    painter->drawText(rectFoot, "http://www.twoblackcoffees.com",
                      QTextOption(Qt::AlignCenter));

    return QRect(0, posYTitle, painter->viewport().width(), posYFoot - posYTitle);
}

// =============================================================================
void PDFCreator::paintImage(QPainter * painter, const QRect & freeSpaceRect, int cellHeight) {
    QPixmap pixmap(":/images/tt_2black_coffees_bk_gnd.png");
    pixmap = pixmap.scaledToWidth(freeSpaceRect.width() / 4, Qt::SmoothTransformation);

    QAbstractItemModel * tableModel = mTableView->model();
    QRect rect(freeSpaceRect.x() + freeSpaceRect.width() - pixmap.width(),
               freeSpaceRect.y() + (tableModel->rowCount() + 1) * cellHeight - pixmap.height(),
               pixmap.width(), pixmap.height());
    painter->drawPixmap(rect, pixmap);

}

// =============================================================================
void PDFCreator::paintTable(QPainter * painter, const QRect & freeSpaceRect, int cellHeight) {
    QVector<int> percentWidthsColumn;
    percentWidthsColumn << 0 << 6 << 14 << 22 << 30 << 38 << 46 << 54 << 62 << 100;
    paintEmptyTable(painter, freeSpaceRect, cellHeight, percentWidthsColumn);
    paintTableHeader(painter, freeSpaceRect, cellHeight, percentWidthsColumn);
    paintTableContent(painter, freeSpaceRect, cellHeight, percentWidthsColumn);
}

// =============================================================================
void PDFCreator::paintEmptyTable(QPainter * painter, const QRect & freeSpaceRect,
                                 int cellHeight, const QVector<int> & percentWidthsColumn) {
    int width = freeSpaceRect.width();
    QAbstractItemModel * tableModel = mTableView->model();
    QPen myPen(Qt::black, 2, Qt::SolidLine);
    painter->setPen(myPen);
    for(int row = 0; row < tableModel->rowCount() + 2; ++row) {
        painter->drawLine(freeSpaceRect.x(),
                          freeSpaceRect.y() + row * cellHeight,
                          freeSpaceRect.x() + width,
                          freeSpaceRect.y() + row * cellHeight);
    }
    int tableHeight = (tableModel->rowCount() + 1) * cellHeight;
    for(int col = 0; col < percentWidthsColumn.size(); ++col) {
        int colX = width * percentWidthsColumn[col] / 100;
        painter->drawLine(freeSpaceRect.x() + colX,
                          freeSpaceRect.y(),
                          freeSpaceRect.x() + colX,
                          freeSpaceRect.y() + tableHeight);
    }
 }

// =============================================================================
void PDFCreator::paintTableHeader(QPainter * painter, const QRect & freeSpaceRect,
                                 int cellHeight, const QVector<int> & percentWidthsColumn) {

    int width = freeSpaceRect.width();
    QAbstractItemModel * tableModel = mTableView->model();

    for(int col = 1; col < percentWidthsColumn.size() - 1; ++col) {

        QRect rect(freeSpaceRect.x() + width * percentWidthsColumn[col] / 100,
                   freeSpaceRect.y(),
                   width * (percentWidthsColumn[col + 1] - percentWidthsColumn[col]) / 100,
                   cellHeight);
        QFont font(tableModel->headerData(col - 1, Qt::Horizontal, Qt::FontRole).value<QFont>());
        font.setPointSize(8);
        painter->setFont(font);
        QPen myPen(tableModel->headerData(col - 1, Qt::Horizontal, Qt::TextColorRole).value<QColor>(), 2, Qt::SolidLine);
        painter->setPen(myPen);
        painter->drawText(rect,
                          tableModel->headerData(col - 1, Qt::Horizontal).toString(),
                          QTextOption(Qt::AlignCenter));
    }

    for(int row = 1; row < tableModel->rowCount() + 1; ++row) {
        QRect rect(freeSpaceRect.x(),
                   freeSpaceRect.y() + row * cellHeight,
                   width * percentWidthsColumn[1] / 100,
                   cellHeight);
        QFont font(tableModel->headerData(row - 1, Qt::Vertical, Qt::FontRole).value<QFont>());
        font.setPointSize(8);
        painter->setFont(font);
        QPen myPen(tableModel->headerData(row - 1, Qt::Vertical, Qt::TextColorRole).value<QColor>(), 2, Qt::SolidLine);
        painter->setPen(myPen);
        painter->drawText(rect, tableModel->headerData(row - 1, Qt::Vertical).toString(),
                          QTextOption(Qt::AlignCenter));
    }
}

// =============================================================================
void PDFCreator::paintTableContent(QPainter * painter, const QRect & freeSpaceRect,
                                     int cellHeight, const QVector<int> & percentWidthsColumn) {

    int width = freeSpaceRect.width();
    QAbstractItemModel * tableModel = mTableView->model();
    for(int row = 1; row < tableModel->rowCount() + 1; ++row) {
        for(int col = 1; col < percentWidthsColumn.size() - 1; ++col) {
            QRect rect(freeSpaceRect.x() + width * percentWidthsColumn[col] / 100,
                       freeSpaceRect.y() + row * cellHeight,
                       width * (percentWidthsColumn[col + 1] - percentWidthsColumn[col]) / 100,
                       cellHeight);
            QModelIndex index = tableModel->index(row - 1, col - 1);
            QFont font(tableModel->data(index, Qt::FontRole).value<QFont>());
            font.setPointSize(8);
            painter->setFont(font);
            QPen myPen(tableModel->data(index, Qt::TextColorRole).value<QColor>(), 2, Qt::SolidLine);
            painter->setPen(myPen);
            painter->drawText(rect, tableModel->data(index).toString(),
                              QTextOption(Qt::AlignCenter));
        }
    }
}

#ifdef NOT_REQUIRED_QDOCUMENT
// =============================================================================
void PDFCreator::populateDocument(QTextDocument *document)
{
    document->setDefaultFont(QFont("Times", 15));
    QTextCursor cursor(document);

    addTitleToDocument(&cursor, "Timesheet of " + mTimeTrackingModel->getUserName(),    Qt::AlignHCenter);
    QDate date = mTimeTrackingModel->getBaseDate();
    addTitleToDocument(&cursor, "Date: " + date.toString("MMM / yyyy"),                 Qt::AlignLeft);

    addTableToDocument(&cursor);
    /*addItemsToDocument(cursor, thePage);
    cursor->insertHtml(thePage.descriptionHtml);
    addRuleToDocument(cursor);*/
}


// =============================================================================
void PDFCreator::addTitleToDocument(QTextCursor * cursor, const QString & titlePart, Qt::AlignmentFlag alignment)
{
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(alignment);
    blockFormat.setTopMargin(0);
    cursor->insertBlock(blockFormat);

    QTextCharFormat charFormat;
    charFormat.setFont(QFont("Georgia", 14, QFont::Bold));
    cursor->insertText(titlePart, charFormat);
}

// =============================================================================
void PDFCreator::addTableToDocument(QTextCursor * cursor)
{
    QAbstractItemModel * tableModel = mTableView->model();
    QTextTable * textTable = cursor->insertTable(tableModel->rowCount() + 1,
                                                 tableModel->columnCount() + 1,
                                                 getTextTableFormat());
    // Print Header
    for(unsigned int pos = 0; pos < tableModel->rowCount(); ++pos) {
        createHeaderCellContent(textTable, pos, tableModel, Qt::Vertical);
    }
    for(unsigned int pos = 0; pos < tableModel->columnCount(); ++pos) {
        createHeaderCellContent(textTable, pos, tableModel, Qt::Horizontal);
    }

    // Print content
    for(unsigned int col = 0; col < tableModel->columnCount(); ++col) {
        for(unsigned int row = 0; row < tableModel->rowCount(); ++row) {
            createCellContent(textTable, col, row, tableModel);
        }
    }


}

// =============================================================================
void PDFCreator::createHeaderCellContent(QTextTable * textTable,
                                         int pos,
                                         QAbstractItemModel * tableModel,
                                         Qt::Orientation orientation) {

    QTextTableCell textTableCell = textTable->cellAt(
                orientation == Qt::Horizontal ? 0       : pos + 1,
                orientation == Qt::Horizontal ? pos + 1 : 0);

    QTextBlockFormat textBlockFormat;
    textBlockFormat.setAlignment(Qt::AlignCenter);

    QTextCursor cursor = textTableCell.firstCursorPosition();
    cursor.insertBlock(textBlockFormat);

    QTextCharFormat textCharFormat;
    textCharFormat.setFont(tableModel->headerData(pos, orientation, Qt::FontRole).value<QFont>());
    textCharFormat.setForeground(QBrush(tableModel->headerData(pos, orientation, Qt::TextColorRole).value<QColor>()));
    cursor.insertText(tableModel->headerData(pos, orientation).toString(),
                      textCharFormat);
}

// =============================================================================
void PDFCreator::createCellContent(QTextTable * textTable,
                                   int col,
                                   int row,
                                   QAbstractItemModel * tableModel) {

    QTextTableCell textTableCell = textTable->cellAt(row + 1, col + 1);

    QTextBlockFormat textBlockFormat;
    textBlockFormat.setAlignment(Qt::AlignCenter);

    QTextCursor cursor = textTableCell.firstCursorPosition();
    cursor.insertBlock(textBlockFormat);

    QTextCharFormat textCharFormat;
    QModelIndex index = tableModel->index(row, col);
    textCharFormat.setFont(tableModel->data(index, Qt::FontRole).value<QFont>());
    textCharFormat.setForeground(QBrush(tableModel->data(index, Qt::TextColorRole).value<QColor>()));
    cursor.insertText(tableModel->data(index).toString(),
                      textCharFormat);


}

// =============================================================================
QTextTableFormat PDFCreator::getTextTableFormat() {
    QTextTableFormat textTableFormat;
    textTableFormat.setAlignment(Qt::AlignCenter);
    textTableFormat.setCellPadding(0);
    textTableFormat.setBorder(0);
    textTableFormat.setHeaderRowCount(1);
    textTableFormat.setCellSpacing(0);
    textTableFormat.setTopMargin(0);
    textTableFormat.setLeftMargin(0);
    textTableFormat.setRightMargin(0);
    textTableFormat.setBottomMargin(0);
    textTableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    //textTableFormat.setHeight();
    QVector<QTextLength> widths;
    widths << QTextLength(QTextLength::PercentageLength, 8)
           << QTextLength(QTextLength::PercentageLength, 6)
           << QTextLength(QTextLength::PercentageLength, 6)
           << QTextLength(QTextLength::PercentageLength, 6)
           << QTextLength(QTextLength::PercentageLength, 6)
           << QTextLength(QTextLength::PercentageLength, 6)
           << QTextLength(QTextLength::PercentageLength, 6)
           << QTextLength(QTextLength::PercentageLength, 6)
           << QTextLength(QTextLength::PercentageLength, 50);
    textTableFormat.setColumnWidthConstraints(widths);
    return textTableFormat;
}
#endif

