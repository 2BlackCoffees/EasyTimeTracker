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

#include "timetrackingsettings.h"
#include "Settings/iosettings.h"

TimeTrackingSettings* TimeTrackingSettings::mTimeTrackingSettings = NULL;

TimeTrackingSettings::TimeTrackingSettings() : mActionSeparator("#**#"),
    mDefaultBrowser("<Default Browser>")
{
    setDefaultValues();
}

// =============================================================================
void TimeTrackingSettings::setDefaultValues() {
    mPeriodDefaultAction = QTime(3, 0, 0);
    mPeriodPauseTime = QTime(1, 0, 0);
    mPauseString = "Pause time!\n\n"
            "Stop working 5 to 10 minutes, go for a walk, relax your shoulders, "
            "your arms, yours hands and your fingers. \n"
            "Try to think about nice private things.\n"
            "Breath some fresh air. \n\n"
            "Come back with a fresh mind, and relaxed body.\n\n"
            "That way you will not damage your health and will improve your efficiency at work.";

    mPauseTimeEnable = true;

    for(int index = 0; index < NB_DAYS_PER_WEEK; index++) {
        if(index < NB_DAYS_PER_WEEK - 2) {
            mIsWorkingDay[index] = true;
            mRequiredWorkTime[index] = QTime(8, 0, 0);
            mPause[index] = QTime(0, 45, 0);
        } else {
            mIsWorkingDay[index] = false;
            mRequiredWorkTime[index] = QTime(0, 0, 0);
            mPause[index] = QTime(0, 0, 0);
        }
    }
}

// =============================================================================
bool TimeTrackingSettings::isWorkingDay(int weekDay) const {
    if(weekDay > NB_DAYS_PER_WEEK || weekDay < 1) {
        DEBUG(QString("Specified wekday is %1 but is should"
                      " be comprised between 1 and %2").arg(weekDay).arg(NB_DAYS_PER_WEEK));
        throw new TTExceptionOverflow(QString("Specified weekday is %1 but is should"
                                              " be comprised between 1 and %2").arg(weekDay).arg(NB_DAYS_PER_WEEK));
    }

    return mIsWorkingDay[weekDay - 1];
}

// =============================================================================
QTime TimeTrackingSettings::getRequiredWorkTime(const QDate& date) const {

    if(!date.isValid()) {
        DEBUG("Invalid date");
        return QTime();
    }

    if(isWorkingDay(date)) {
        return mRequiredWorkTime[date.dayOfWeek() - 1];
    }


    return QTime(0, 0, 0);
}

// =============================================================================
QTime TimeTrackingSettings::getPause(const QDate& date) const {
    if(!date.isValid()) {
        DEBUG("Invalid date");
        throw new TTExceptionWrongBaseDate(QString("Invalid date in getPause"));
    }

    if(isWorkingDay(date)) {
        return mPause[date.dayOfWeek() - 1];
    }


    return QTime(0, 0, 0);
}

// =============================================================================
QTime TimeTrackingSettings::getPeriodDefaultAction() const {
    return mPeriodDefaultAction;
}

// =============================================================================
void TimeTrackingSettings::setWorkingDay(const QDate& date, bool isWorkingDay) {
    if(!date.isValid()) {
        DEBUG("Invalid date");
        throw new TTExceptionWrongBaseDate(QString("Invalid date in setPause"));
    }

    mIsWorkingDay[date.dayOfWeek() - 1] = isWorkingDay;
}

// =============================================================================
void TimeTrackingSettings::setRequiredWorkTime(const QDate& date, const QTime& time) {
    if(!date.isValid()) {
        DEBUG("Invalid date");
        throw new TTExceptionWrongBaseDate(QString("Invalid date in setPause"));
    }

    if(isWorkingDay(date)) {
        mRequiredWorkTime[date.dayOfWeek() - 1] = time;
    }

}

// =============================================================================
void TimeTrackingSettings::setPause(const QDate& date, const QTime& time) {
    if(!date.isValid()) {
        DEBUG("Invalid date");
        throw new TTExceptionWrongBaseDate(QString("Invalid date in setPause"));
    }

    if(isWorkingDay(date)) {
        mPause[date.dayOfWeek() - 1] = time;
    }

}

// =============================================================================
void TimeTrackingSettings::setPeriodDefaultAction(const QTime& time) {
    if(!time.isValid()) {
        DEBUG("Invalid time");
        throw new TTExceptionWrongBaseDate(QString("Invalid time in setPeriodDefaultAction"));
    }
    mPeriodDefaultAction = time;
}

// =============================================================================
bool TimeTrackingSettings::removeAction(const QString &command, const QString &parameter) {

    QMap<QString, QString>::iterator it =
            mActions.find(actionToKey(command, parameter));
    if(it != mActions.end()) {
        QString key = it.key();
        mActions.remove(key);
        IOSettings settings;
        settings.saveSettings(this);

        return true;
    }
    return false;
}

