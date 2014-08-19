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

#ifndef MISC_H
#define MISC_H

#include "debug.h"
#include "ttexceptions.h"

#include <QStringList>
#include <QTime>
#include <QMap>

#include "assert.h"

#define TT_VERSION "0.1"
template <typename T>
class TCallBack {
public:
    typedef void (T::* TClassFunction)(void);
private:
    TClassFunction mPt2FunctionFirst;
    TClassFunction mPt2FunctionSecond;
    T* mClassPtr;
    int* mReferenceVariable;
public:

    TCallBack(T* classPtr,
              TClassFunction pt2FunctionFirst,
              TClassFunction pt2FunctionSecond,
              int* referenceVariable,
              bool forceCall = true) :
        mClassPtr(classPtr),
        mPt2FunctionFirst(pt2FunctionFirst),
        mPt2FunctionSecond(pt2FunctionSecond),
        mReferenceVariable(referenceVariable){

        (*mReferenceVariable)++;

        if((*mReferenceVariable) <= 1 || forceCall) {
            (mClassPtr->*mPt2FunctionFirst)();
        }
    }

    ~TCallBack() {
        (*mReferenceVariable)--;

        if((*mReferenceVariable) > 0) {
            DEBUG("Calling TCallBack::Destructor: Not calling 2nd fct because nested call occured.");

        } else {
            if((*mReferenceVariable) < 0) {
                DEBUG(QString("Calling TCallBack::Destructor: Strange it is lower than 0: %1").arg((*mReferenceVariable)));
            }

            (mClassPtr->*mPt2FunctionSecond)();

        }

    }
};

class Misc {
public:
    static QString getUserName();

};

class MiscDateTime {
    static const QDate mInitDate;
    static const QTime mInitTime;
public:
    static QTime stringToTime(const QString& text) ;

    static QString timeToString(const QTime& time, const QString& conv = "HH:mm") {
        return time.toString(conv);
    }

    static int stringHoursToSeconds(const QString& text);

    static QDate stringToDate(const QString& text);
    static QString dateToString(const QDate& date, const QString& conv = "dd-MM-yyyy") {
        return date.toString(conv);
    }

    static QString dateTimeToString(const QDateTime& dateTime,
                                    const QString& conv = "dd-MM-yyyy hh:mm:ss") {
        return dateTime.toString(conv);
    }
    static QDateTime stringToDateTime(const QString& text);

    static int dateTimeToSeconds(const QDateTime& dateTime) {
        return QDateTime(mInitDate, mInitTime).secsTo(dateTime);
    }
    static int timeToSeconds(const QTime& time) {
        return QTime(mInitTime).secsTo(time);
    }

    static QDateTime secondsToDateTime(int seconds) {
        return QDateTime(mInitDate, mInitTime).addSecs(seconds);
    }

    static QString dateTimeToHours(QDateTime dateTime) {
        QDateTime tmpDateTime(mInitDate, mInitTime);
        int seconds = tmpDateTime.secsTo(dateTime);
        return secondsToHours(seconds);
    }
    static QString secondsToHours(int seconds);

    static QDate getInitDate() {
        return mInitDate;
    }
    static QTime getInitTime() {
        return mInitTime;
    }
};
#endif // MISC_H
