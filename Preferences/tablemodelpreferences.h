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

#ifndef TABLEMODELPREFERENCES_H
#define TABLEMODELPREFERENCES_H

#include "TableModeladapter.h"
#include <QAbstractTableModel>

class TableModelPreferences : public QAbstractTableModel
{
    Q_OBJECT
private:
    TableModelAdapter* mTableModelAdapter;
public:
    TableModelPreferences(QObject *parent);
    virtual ~TableModelPreferences();

    int rowCount(const QModelIndex &/*parent = QModelIndex()*/) const {
        return mTableModelAdapter->rowCount();
    }
    int columnCount(const QModelIndex &/*parent = QModelIndex()*/) const {
        return mTableModelAdapter->columnCount();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &) const;


};


#endif // TableModelPreferences_H
