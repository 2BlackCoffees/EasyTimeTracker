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

#ifndef PAUSE_DIALOG_H
#define PAUSE_DIALOG_H

#include <QDialog>
#include <QTimer>
#include <QTime>
#include <QScopedPointer>
#include <QDateTime>

namespace Ui {
class PauseDialog;
}

class PauseDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PauseDialog(QWidget *parent, const QTime & currentPause);
    ~PauseDialog();
    bool isPauseValid() const {
        return mValidPause;
    }
    QTime getPause() const;
    
private slots:
    void onTimer();
    void onCheckBoxIncludeExistingOrDefaultPause(int newState);

private:
    QScopedPointer<QTimer> mTimer;
    QTime mStartedTime;
    QDate mStartedDate;
    QTime mCurrentPause;
    bool mValidPause;
    Ui::PauseDialog *ui;

    void setNewTime();
};

#endif // PAUSE_DIALOG_H
