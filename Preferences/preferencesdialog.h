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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "timetrackingsettings.h"

#include <QDialog>

class QResizeEvent;
class QTableView;

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();
    
private slots:
    void onTimeChangedDefaultAction(const QTime &time);
    void onStateChangedDefaultAction(int state);
    void onTimeChangedPauseTime(const QTime &time);
    void onStateChangedPauseTime(int state);
    void onAddAction();
    void onDeleteAction();
    void onTableActionSelectionChanged(const QModelIndex & current, const QModelIndex & previous);
    void onWhatIsThis();
    void onOK();

private:
    Ui::PreferencesDialog *ui;
    QTableView *mTableWorkingHours;

    void init();
    void adjustColumnSizes(int width);

    void resizeEvent(QResizeEvent* event);



};

#endif // PREFERENCESDIALOG_H
