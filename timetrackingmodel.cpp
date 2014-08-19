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

#include "timetrackingmodel.h"
#include "debug.h"

#include <QDir>

#include <stdexcept>
#include <assert.h>

const QDateTime TimeTrackingModel::mINVALID_DATE = QDateTime(QDate(-5000, 0,0));

// =============================================================================
QString TimeTrackingModel::getDefaultFileName(const QString& userName, bool exportToPDF) const {
    return getPrevDefaultFileName(userName, 0, exportToPDF);

}

// =============================================================================
QString TimeTrackingModel::getPrevDefaultFileName(const QString& userName, int index, bool exportToPDF) const {

    int month = getBaseDate().month();
    int year = getBaseDate().year();

    if(index == 0) {
        return createFileName(userName,  year, month, exportToPDF);
    }

    index = abs(index);
    while(index > 0) {
        if(month > index) {
            month -= index;
            index = 0;
        } else {
            index -= month;
            month = 12;
            year--;
        }
    }

    return createFileName(userName,  year, month, exportToPDF);
}

// =============================================================================
QString TimeTrackingModel::getNextDefaultFileName(const QString& userName, int index, bool exportToPDF) const {

    int month = getBaseDate().month();
    int year = getBaseDate().year();

    if(index == 0) {
        return createFileName(userName,  year, month, exportToPDF);
    }

    // This should work too
    // year += index % 12
    // month += index - (index % 12) * 12;
    while(index > 0) {
        if(month + index <= 12) {
            month += index;
            index = 0;
        } else {
            index -= 12;
            year++;
        }
    }

    QDateTime now = QDateTime::currentDateTime();
    if(year < now.date().year() || (year == now.date().year() && month <= now.date().month())) {
        return createFileName(userName,  year, month, exportToPDF);
    }

    return "";
}

// =============================================================================
QString TimeTrackingModel::getFileNameRegExp(const QString& userName) {
    return userName + "-(\\d{4})-(\\d{2})\\.xml";
}

// =============================================================================
QString TimeTrackingModel::getDirectory()
{
    return QDir::homePath();
}

// =============================================================================
QString TimeTrackingModel::createFileName(const QString& userName, int year, int month, bool exportToPDF) const {
    QString tmpUser = userName;

    if(tmpUser.isEmpty()) {
        tmpUser = getUserName();
    }

    QString filePath = TimeTrackingModel::getDirectory() + QDir::separator() + tmpUser + QString("-%1-%2.%3").
            arg(year).arg(month, 2, 10, (const QChar)'0').arg(exportToPDF ? "pdf" : "xml");
    return QDir::toNativeSeparators(filePath);
}

// =============================================================================
void TimeTrackingModel::setNewBaseDate(const QDate& date)   {
    if(!date.isValid()) {
        addError(QString(QObject::tr("Wrong base date: %1")).
                                       arg(date.toString("dd-MMMM-yyyy")));
        return;

    }
    mTTTime->clear();
    initVariables(date);
}

// =============================================================================
void TimeTrackingModel::setUserName(const QString& userName)                         {
    if(mUserName.isEmpty()) {
        addError("A user name cannot be set empty !");
        return;
    }
    mUserName = userName;
}

// =============================================================================
QString TimeTrackingModel::getUserName() const {
    if(mUserName.isEmpty()) {
        addError("Trying to get a user name despite initialisation did not took place !");
    }
    return mUserName;

}

// =============================================================================
QDate TimeTrackingModel::getBaseDate() const {
    if(!mTTDate.isValid()) {
        addError(QString(QObject::tr("Current base date is not valid: %1")).
                                       arg(mTTDate.toString("dd-MMMM-yyyy")));
    }
    return mTTDate;
}

// =============================================================================
bool TimeTrackingModel::isModelOfCurMonth() const {
    return isModelOfCurMonth(mTTDate);
}

// =============================================================================
bool TimeTrackingModel::isModelOfCurMonth(const QDate& date) const {
    QDateTime now = QDateTime::currentDateTime();
    return date.month() == now.date().month() && date.year() == now.date().year();
}

// =============================================================================
void TimeTrackingModel::assertMonthYearOK(const QDate& date) {
    if(!isModelOfCurMonth(date)) {
        addError(QString(QObject::tr("Trying to start today (%1) on a "
                                                           "sheet configured for (%2)")).
                                       arg(date.toString("dd-MMMM-yyyy")).
                                       arg(mTTDate.toString("dd-MMMM-yyyy")));
        return;

    }
}

// =============================================================================
void TimeTrackingModel::setStartTime() {

    QDateTime now = QDateTime::currentDateTime();
    assertMonthYearOK(mTTDate);

    setStartTime(now.date().day(), now.time(), false);

}

// =============================================================================
int TimeTrackingModel::getCurrentDay() const {
    return QDateTime::currentDateTime().date().day();
}

// =============================================================================
int TimeTrackingModel::daysInMonth() const {
    return mTTDate.daysInMonth();
}

// =============================================================================
void TimeTrackingModel::setEndTime() {

    QDateTime now = QDateTime::currentDateTime();
    assertMonthYearOK(mTTDate);

    int index = monthDayToIndex(now.date().day());
    if(startTimeIsValid() && mTTTime->at(index).startedTime > now) {
        addError(QObject::tr("End time occurs before start time !"));
        return;

    }

    setEndTime(now.date().day(), now.time());

    if(!startTimeIsValid()) {
        addError(QObject::tr("Set an end date for a non valid start date !"));
        return;
    }

}

// =============================================================================
void TimeTrackingModel::setPause(const QTime& pause) {

    QDateTime now = QDateTime::currentDateTime();
    assertMonthYearOK(mTTDate);

    mTTTime->at(monthDayToIndex(now.date().day())).pause = pause;
    emit somethingChanged();

}

// =============================================================================
void TimeTrackingModel::setComment(const QString& comment) {

    QDateTime now = QDateTime::currentDateTime();
    assertMonthYearOK(mTTDate);

    mTTTime->at(monthDayToIndex(now.date().day())).comment = comment;

}

// =============================================================================
int TimeTrackingModel::monthDayToIndex(int monthDay) const {

    unsigned int index = monthDay;
    if(monthDay < 0) {
        index = getCurrentDay();
    }
    // Day start from 1 but index from 0!
    index--;

    if(((unsigned)index) >= mTTTime->size()) {
        addError(QString(QObject::tr("Trying to access a TT field at "
                                                      "index %1 where indexes are included in [0, %2[")).
                                  arg(index).arg(mTTTime->size()));
        return 0;
    }

    return index;
}

// =============================================================================
bool TimeTrackingModel::startTimeIsValid(int monthDay) const {
    return mTTTime->at(monthDayToIndex(monthDay)).startedTime.isValid();
}

// =============================================================================
bool TimeTrackingModel::endTimeIsValid(int monthDay) const {
    return mTTTime->at(monthDayToIndex(monthDay)).endedTimeIsValid && mTTTime->at(monthDayToIndex(monthDay)).endedTime.isValid();
}

// =============================================================================
bool TimeTrackingModel::pauseIsValid(int monthDay) const {
    return mTTTime->at(monthDayToIndex(monthDay)).pause.isValid();
}

// =============================================================================
bool TimeTrackingModel::requiredWorkTimeIsValid(int monthDay) const {
    return mTTTime->at(monthDayToIndex(monthDay)).requiredTimePerDay.isValid();

}

// =============================================================================
const QTime TimeTrackingModel::getStartTime(int monthDay) const {

    if(startTimeIsValid(monthDay)) {
        // This will activate the pause (Necessary for DialogTimeFollower which is not based on a MVC)
        getPause(monthDay);
        return mTTTime->at(monthDayToIndex(monthDay)).startedTime.time();
    }
    return QTime();
}

// =============================================================================
const QTime TimeTrackingModel::getEndTime(int monthDay) const {

    if(endTimeIsValid(monthDay)) {
        return mTTTime->at(monthDayToIndex(monthDay)).endedTime.time();
    }
    return QTime();

}

// =============================================================================
const QTime TimeTrackingModel::getWorkTime(int monthDay) const {
    return endMinusStartMinusPause(monthDay);

}

// =============================================================================
const QTime TimeTrackingModel::getPause(int monthDay) const {
    if(!startTimeIsValid(monthDay)) {
        return QTime();
    }

    int index = monthDayToIndex(monthDay);
    if(!mTTTime->at(index).pause.isValid()) {
        mTTTime->at(index).pause = TimeTrackingSettings::getInstance()->getPause(
                    QDate(mTTDate.year(), mTTDate.month(), monthDay));
    }

    return mTTTime->at(index).pause;
}

// =============================================================================
const QTime TimeTrackingModel::getRequiredWorkTime(int monthDay) const {

    if(!startTimeIsValid(monthDay)) {
        return QTime();
    }

    int index = monthDayToIndex(monthDay);

    if(!mTTTime->at(index).requiredTimePerDay.isValid()) {
        mTTTime->at(index).requiredTimePerDay = TimeTrackingSettings::getInstance()->getRequiredWorkTime(
                    QDate(mTTDate.year(), mTTDate.month(), monthDay <= 0 ? index + 1 : monthDay));
    }

    return mTTTime->at(index).requiredTimePerDay;
}

// =============================================================================
const QTime TimeTrackingModel::getDuedOverHours() const {
    return QDateTime::currentDateTime().time();
}

// =============================================================================
const QString TimeTrackingModel::getComment(int monthDay) const {
    int index = monthDayToIndex(monthDay);

    return mTTTime->at(index).comment;

}

// =============================================================================
const QString TimeTrackingModel::getDayName(int monthDay) const{

    if(monthDay > mTTDate.daysInMonth()) {
        addError(QString(QObject::tr("Trying to access a date for day %1 "
                                                      " in a month having only %2 days")).
                                  arg(monthDay).arg(mTTDate.daysInMonth()));
    }

    return QDate(mTTDate.year(), mTTDate.month(), monthDay).toString("ddd");

}


// =============================================================================
void TimeTrackingModel::setStartTime(int monthDay, const QTime& newTime, bool changeStartTimeOnly) {

    if(newTime.isValid()) {
        int index = monthDayToIndex(monthDay);
        // If we start while start time is valid use span time from stop
        // to now as pause time
        if(startTimeIsValid() && changeStartTimeOnly == false && getEndTime(monthDay).isValid()) {
            int elapsedSeconds = getEndTime(monthDay).secsTo(QTime::currentTime());
            setPause(monthDay, getPause(monthDay).addSecs(elapsedSeconds));
            // Make endTime invalid
            //TODO: Use a new method for this
            mTTTime->at(index).endedTimeIsValid = false;

        } else {

            mTTTime->at(index).startedTime =
                    QDateTime(getBaseDate(),
                              newTime);
        }
        emit somethingChanged();

    }
}

// =============================================================================
void TimeTrackingModel::setEndTime(int monthDay, const QTime& newTime) {

    if(newTime.isValid()) {

        if(newTime < getStartTime(monthDay)) {
            DEBUG(QString(
                      QObject::tr("End time is smaller than start time for day %1")).
                  arg(monthDay));
            addError(QString(QObject::tr("End time is smaller than start time for day %1")).
                                               arg(monthDay) );
            return;
        }
        int index = monthDayToIndex(monthDay);
        mTTTime->at(index).endedTime =
                QDateTime(QDate(mTTDate.year(), mTTDate.month(), monthDay),
                          newTime);
        mTTTime->at(index).endedTimeIsValid = true;

        emit somethingChanged();

    }
}

// =============================================================================
void TimeTrackingModel::setPause(int monthDay, const QTime& newTime) {
    QTime tmpTime = newTime;

    if(tmpTime.isValid()) {
        int index = monthDayToIndex(monthDay);
        mTTTime->at(index).pause = tmpTime;
        emit somethingChanged();

    }

}

// =============================================================================
void TimeTrackingModel::setComment(int monthDay, const QString& comment) {
    int index = monthDayToIndex(monthDay);
    mTTTime->at(index).comment = comment;

}

// =============================================================================
void TimeTrackingModel::setRequiredWorkTime(int monthDay, const QTime& newTime) {
    QTime tmpTime = newTime;
    if(!tmpTime.isValid()) {
        tmpTime = getDefaultRequiredWorkTime(monthDay);
    }

    if(tmpTime.isValid()) {
        int index = monthDayToIndex(monthDay);
        mTTTime->at(index).requiredTimePerDay = tmpTime;

    }
    emit somethingChanged();

}

// =============================================================================
QTime TimeTrackingModel::getTimeWorkedDiff(int monthDay) const {

    if(!requiredWorkTimeIsValid(monthDay) || !endMinusStartMinusPauseValid(monthDay)) {
        return QTime();
    }
    QTime requiredTime = getRequiredWorkTime(monthDay);
    QTime endStart = endMinusStartMinusPause(monthDay);
    if(endStart.isValid() && requiredTime.isValid()) {
        if(endStart > requiredTime) {
            return endStart. addSecs(- requiredTime.hour() * 3600 - requiredTime.minute() * 60 - requiredTime.second());
        }
        return requiredTime.addSecs(- endStart.hour() * 3600 - endStart.minute() * 60 - endStart.second());
    }
    return QTime();
}

// =============================================================================
bool TimeTrackingModel::timeWorkDiffNegativ(int monthDay) const {

    if(!requiredWorkTimeIsValid(monthDay) || !endMinusStartMinusPauseValid(monthDay)) {
        return false;
    }

    QTime requiredTime = getRequiredWorkTime(monthDay);
    QTime endStart = endMinusStartMinusPause(monthDay);
    if(endStart >= requiredTime) {
        return false;
    }
    return true;
}

// =============================================================================
QTime TimeTrackingModel::differenceFromStartToNow(bool & isNegativ) const {

    QTime requiredWorkTime = getRequiredWorkTime();
    if(!requiredWorkTimeIsValid()) {
        requiredWorkTime = QTime(0, 0, 0);
    }
    if(getStartTime().isValid()) {
        QTime started = getStartTime();
        QTime workedSeconds = QTime::currentTime().
                addSecs( - started.hour() * 3600 - started.minute() * 60 - started.second());
        // workedSeconds = currentTime - startedTime
        if(pauseIsValid()) {
            QTime pause = getPause();
            requiredWorkTime = requiredWorkTime.addSecs(pause.hour() * 3600 + pause.minute() * 60 + pause.second());
            // requiredWorkTime += pause
        }

        if(workedSeconds < requiredWorkTime) {
            requiredWorkTime = requiredWorkTime.addSecs( - workedSeconds.hour() * 3600 - workedSeconds.minute() * 60 - workedSeconds.second());
            isNegativ = true;
            return requiredWorkTime;

        } else {
            workedSeconds = workedSeconds.addSecs( - requiredWorkTime.hour() * 3600 - requiredWorkTime.minute() * 60 - requiredWorkTime.second() + 60);
            isNegativ = false;
            return workedSeconds;
        }

    }
    return QTime();

}

// =============================================================================
QTime TimeTrackingModel::getDefaultRequiredWorkTime(int monthDay) const {
    if(monthDay < 1 || monthDay > mTTDate.daysInMonth()) {
        return QTime();
    }
    return TimeTrackingSettings::getInstance()->getRequiredWorkTime(QDate(mTTDate.year(),
                                                                          mTTDate.month(),
                                                                          monthDay));
}

// =============================================================================
bool TimeTrackingModel::endMinusStartMinusPauseValid(int monthDay) const {
    return  startTimeIsValid(monthDay) &&
            endTimeIsValid(monthDay) &&
            pauseIsValid(monthDay);
}

// =============================================================================
QTime TimeTrackingModel::endMinusStartMinusPause(int monthDay) const {

    if(!endMinusStartMinusPauseValid(monthDay)) {
        return QTime();
    }

    int startedPlusPause = QTime(0, 0, 0).secsTo(getStartTime(monthDay)) +
            QTime(0, 0, 0).secsTo(getPause(monthDay));
    QTime stopped = getEndTime(monthDay);
    if(startedPlusPause < QTime(0, 0, 0).secsTo(stopped)) {
        return stopped.addSecs(- startedPlusPause);
    }
    return QTime();

}

// =============================================================================
void TimeTrackingModel::setPrevMonthlyOverDueSeconds(int prevMonthlyOverDueSeconds) {
    mPrevMonthlyOverDueSeconds = prevMonthlyOverDueSeconds;
}

// =============================================================================
int TimeTrackingModel::getPrevMonthlyOverDueSeconds(void) const {
    return mPrevMonthlyOverDueSeconds;
}

// =============================================================================
int TimeTrackingModel::getMonthlySeconds() const {
    int monthlySeconds = 0;
    for(int monthDay = 1; monthDay <= daysInMonth(); monthDay++) {
        if(startTimeIsValid(monthDay) && endTimeIsValid(monthDay) &&
                pauseIsValid(monthDay)) {

            QTime tmpTime = getWorkTime(monthDay);
            monthlySeconds += MiscDateTime::timeToSeconds(tmpTime);
//            DEBUG(QString("Month: %1, monthDay %2, monthlySeconds: %3, monthlySeconds/3600: %4").arg(mTTDate.toString("MMM / yyyy"))
//                  .arg(monthDay).arg(monthlySeconds).arg(monthlySeconds/3600));
        }
    }
    return monthlySeconds;

}

// =============================================================================
void TimeTrackingModel::setCompensatedOverDueSeconds(int compensatedOverDueSeconds) {
    mCompensatedOverDueSeconds = compensatedOverDueSeconds;
}

// =============================================================================
int TimeTrackingModel::getCompensatedOverDueSeconds() const {
    return mCompensatedOverDueSeconds;
}

// =============================================================================
int TimeTrackingModel::getMonthlyOverDueSeconds() const {
    return getMonthOnlyOverDueSeconds() + mPrevMonthlyOverDueSeconds - mCompensatedOverDueSeconds;

}

// =============================================================================
int TimeTrackingModel::getMonthOnlyOverDueSeconds() const {
    int overDueSeconds = 0;
    for(int monthDay = 1; monthDay <= daysInMonth(); monthDay++) {
        if(startTimeIsValid(monthDay) && endTimeIsValid(monthDay) &&
                pauseIsValid(monthDay) && requiredWorkTimeIsValid(monthDay)) {

            QTime tmpTime = getTimeWorkedDiff(monthDay);
            if(timeWorkDiffNegativ(monthDay)) {
                overDueSeconds -= QTime(0, 0, 0).secsTo(tmpTime);
            } else {
                overDueSeconds += QTime(0, 0, 0).secsTo(tmpTime);
            }
        }
    }

    return overDueSeconds;

}

// =============================================================================
QString TimeTrackingModel::getPrevMonthlyOverDueSecondsInHours() const {
    return MiscDateTime::secondsToHours(mPrevMonthlyOverDueSeconds);

}

// =============================================================================
QString TimeTrackingModel::getMonthlySecondsInHours() const {
    return MiscDateTime::secondsToHours(getMonthlySeconds());

}

// =============================================================================
QString TimeTrackingModel::getMonthlyOverDueSecondsInHours() const {
    return MiscDateTime::secondsToHours(getMonthlyOverDueSeconds());

}

// =============================================================================
QString TimeTrackingModel::getMonthOnlyOverDueSecondsInHours() const {
    return MiscDateTime::secondsToHours(getMonthOnlyOverDueSeconds());

}

// =============================================================================
DynamicField * TimeTrackingModel::getDynamicFieldFromName(const QString & name) const {
    for(DynamicField * dynamicField : mVectorDynamicFields) {
        if(dynamicField->getFieldName() == name) {
            return dynamicField;
        }
    }
    return nullptr;
}

// =============================================================================
void TimeTrackingModel::createDynamicComboBox(const QString & comboboxName, const QStringList & values) {
    DynamicField * dynamicField = getDynamicFieldFromName(comboboxName);
    if(dynamicField == nullptr) {
        dynamicField = new DynamicFieldInMultipleValues(comboboxName);
        static_cast<DynamicFieldInMultipleValues *>(dynamicField)->setStringList(values);
        mVectorDynamicFields.push_back(dynamicField);
    }
}

// =============================================================================
void TimeTrackingModel::createDynamicTextField(const QString & textFieldName) {
    DynamicField * dynamicField = getDynamicFieldFromName(textFieldName);
    if(dynamicField == nullptr) {
        mVectorDynamicFields.push_back(new DynamicField(textFieldName));
        return;
    }
    assert(false);
}

// =============================================================================
void TimeTrackingModel::removeDynamicField(const QString & name) {
    DynamicField * dynamicField = getDynamicFieldFromName(name);
    assert(dynamicField != nullptr);
    if(dynamicField != nullptr) {
        for(std::vector<DynamicField *>::iterator vectorDynamicFieldsIt = mVectorDynamicFields.begin();
            vectorDynamicFieldsIt != mVectorDynamicFields.end();
            ++vectorDynamicFieldsIt) {
            if((*vectorDynamicFieldsIt)->getFieldName() == name) {
                mVectorDynamicFields.erase(vectorDynamicFieldsIt);
                break;
            }
        }
        delete dynamicField;
    }
}

// =============================================================================
const QString & TimeTrackingModel::getDynamicFieldSelection(const QString & dynamicFieldName, const QDate& date) {
    DynamicField * dynamicField = getDynamicFieldFromName(dynamicFieldName);
    assert(dynamicField != nullptr);
    if(dynamicField != nullptr) {
        return dynamicField->getValueForDate(date);
    }
    assert(false);
    static QString error = "Error dynamicField does not exists !";
    return error;
}

// =============================================================================
void TimeTrackingModel::setDynamicFieldSelection(const QString & dynamicFieldName, const QDate & date, const QString & value) {
    DynamicField * dynamicField = getDynamicFieldFromName(dynamicFieldName);
    assert(dynamicField != nullptr);
    if(dynamicField != nullptr) {
        dynamicField->setValueForDate(date, value);
    }
    assert(false);

}

// =============================================================================
const QStringList & TimeTrackingModel::getComboValuesFromDynamicFieldName(const QString & name) const {

    DynamicField * dynamicField = getDynamicFieldFromName(name);
    assert(dynamicField != nullptr);
    if(dynamicField != nullptr) {
        return static_cast<DynamicFieldInMultipleValues *>(dynamicField)->getStringList();
    }

    static QStringList stringList;
    return stringList;

}

// =============================================================================
size_t TimeTrackingModel::getDynamicFieldCount() const {
    return mVectorDynamicFields.size();
}

// =============================================================================
const QString &TimeTrackingModel::getDynamicFieldNameFromIndex(size_t index) const {
    assert(index < getDynamicFieldCount());
    if(index < getDynamicFieldCount()) {
        return mVectorDynamicFields[index]->getFieldName();
    }
    static QString error = "error: Field does not exist!";
    return error;
}

