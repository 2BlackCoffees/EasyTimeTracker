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

#ifndef PDF_CREATOR_H
#define PDF_CREATOR_H


#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextTableCell>
#include <QTextTableFormat>
#include <qnamespace.h>

class QFont;
class QPainter;
class QPrinter;
class QRect;
class QTextCharFormat;
class QTextCursor;
class QTextDocument;
class QTableView;
class TimeTrackingModel;
class QTextTableCell;
class QAbstractItemModel;
class QTextTable;

class PDFCreator : public QObject
{
    Q_OBJECT

public:
    explicit PDFCreator(QTableView * tableView, TimeTrackingModel * timeTrackingModel);

public slots:
    void paintPage(QPrinter *printer, bool noUserInteraction = true);

#ifdef NOT_REQUIRED_QDOCUMENT
public:
    void populateDocument(QTextDocument * document);
#endif

private:
    QTableView * mTableView;
    TimeTrackingModel * mTimeTrackingModel;

    QRect paintTitle(QPainter * painter);
    void paintImage(QPainter *painter, const QRect & freeSpaceRect, int cellHeight);
    void paintTable(QPainter * painter, const QRect & freeSpaceRect, int cellHeight);
    void paintEmptyTable(QPainter *painter, const QRect & freeSpaceRect, int cellHeight, const QVector<int> & percentWidthsColumn);
    void paintTableHeader(QPainter *painter, const QRect & freeSpaceRect, int cellHeight, const QVector<int> & percentWidthsColumn);
    void paintTableContent(QPainter *painter, const QRect & freeSpaceRect, int cellHeight, const QVector<int> & percentWidthsColumn);

#ifdef NOT_REQUIRED_QDOCUMENT
    void addTitleToDocument(QTextCursor * cursor, const QString & titlePart, Qt::AlignmentFlag alignment);
    void addTableToDocument(QTextCursor * cursor);
    void createHeaderCellContent(QTextTable * textTable,
                                 int pos,
                                 QAbstractItemModel * tableModel,
                                 Qt::Orientation orientationprivate);
    void createCellContent(QTextTable * textTable, int col, int row, QAbstractItemModel * tableModel);
    QTextTableFormat getTextTableFormat();
#endif
};



#endif // PDF_CREATOR_H
