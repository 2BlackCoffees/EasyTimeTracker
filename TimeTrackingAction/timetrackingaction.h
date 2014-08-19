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

#ifndef TIMETRACKINGACTION_H
#define TIMETRACKINGACTION_H

#include <QObject>
#include <QTimer>
#include <QUrl>
#include <QString>
#include <QMultiHash>

#include <vector>
#include <boost/scoped_ptr.hpp>

class TimeTrackingAction : public QObject
{
    Q_OBJECT

public:
    TimeTrackingAction(void);
    virtual ~TimeTrackingAction() {
    }
    void resetActions();

public slots:
    void setUserCurrentlyWorking(bool userCurrentlyWorking);
    void startTT() {
       setUserCurrentlyWorking(true);
    }

    void stopTT() {
        setUserCurrentlyWorking(false);
    }

    void pauseTT() {
        setUserCurrentlyWorking(false);
    }

    void updateTimerDefaultAction();
    void updateTimerPause();

private:
    boost::scoped_ptr<QTimer> mTimerDefaultAction;
    boost::scoped_ptr<QTimer> mTimerPause;
    std::vector<QUrl> mFilmUrl;
    int mIndex;
    bool mUserCurrentlyWorking;
    QMultiHash<QString, QString> mActions;

    void action();
    bool userCurrentlyWorking();
    void resetTimerDefaultAction();
    void resetTimerPause();

};

#endif // TIMETRACKINGACTION_H
