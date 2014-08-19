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

#include "tablemodelpreferences.h"

#include <QDebug>
#include <QFont>
#include <QColor>

TableModelPreferences::TableModelPreferences(QObject *parent): QAbstractTableModel(parent),
    mTableModelAdapter(new TableModelAdapter())
{
}

// =============================================================================
TableModelPreferences::~TableModelPreferences() {
    if(mTableModelAdapter != NULL) {
        delete mTableModelAdapter;
    }
}

// =============================================================================
QVariant TableModelPreferences::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            return mTableModelAdapter->horizontalHeaderData(section);
        } else {
            return mTableModelAdapter->verticalHeaderData(section);
        }
    } else if(role == Qt::FontRole && orientation == Qt::Vertical) {

        QFont tmpFont = QAbstractTableModel::headerData(section, orientation,role).value<QFont>();
        if(!mTableModelAdapter->isWorkingDay(section)) tmpFont.setBold(true);
        return tmpFont;

    } else if(role == Qt::TextColorRole && orientation == Qt::Vertical) {

        QColor color = QAbstractTableModel::headerData(section, orientation,role).value<QColor>();
        if(!mTableModelAdapter->isWorkingDay(section)) color = QColor(200, 10, 10);
        return color;

    }
    return QVariant();
}

// =============================================================================
QVariant TableModelPreferences::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    switch(role){
    case Qt::DisplayRole:
        switch(col) {
        case 0:
            return mTableModelAdapter->isWorkingDay(row);
            break;
        case 1:
            return mTableModelAdapter->getRequiredTime(row);
            break;
        case 2:
            return mTableModelAdapter->getPause(row);
            break;
        }
        break;


    case Qt::TextAlignmentRole:

        return Qt::AlignCenter + Qt::AlignVCenter;

        break;
    }
    return QVariant();
}

// =============================================================================
bool TableModelPreferences::setData(const QModelIndex & index, const QVariant & value, int role) {
    int row = index.row();
    int col = index.column();

    switch(role) {
    case Qt::EditRole:
        switch(col) {
        case 0:
            mTableModelAdapter->setWorkingDay(row, value.toBool());
            break;
        case 1:
            mTableModelAdapter->setRequiredTime(row, value.toTime());
            break;
        case 2:
            mTableModelAdapter->setPause(row, value.toTime());
            break;
        }
        break;

    }
    return true;
}

// =============================================================================
Qt::ItemFlags TableModelPreferences::flags(const QModelIndex & index) const {
    int col = index.column();
    int row = index.row();
    if(mTableModelAdapter->isWorkingDay(row) || col == 0) {
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::NoItemFlags;
}
