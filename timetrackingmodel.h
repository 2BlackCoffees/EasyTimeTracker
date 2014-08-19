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

#ifndef TIMETRACKINGMODEL_H
#define TIMETRACKINGMODEL_H

#include "ttexceptions.h"
#include "timetrackingsettings.h"
#include "misc.h"
#include "Misc/dynamicfields.h"

#include <QDate>
#include <QObject>
#include <QVariant>

#include <vector>


class TimeTrackingModel : public QObject
{
    Q_OBJECT
    TimeTrackingSettings* mTimeTrackingSettings;
public:

    TimeTrackingModel(const QDate& date) :
        mPrevMonthlyOverDueSeconds(0),
        mCompensatedOverDueSeconds(0),
        mTTTime(NULL) {
        initVariables(date);
    }

    TimeTrackingModel(int month, int year) :
        mPrevMonthlyOverDueSeconds(0),
        mCompensatedOverDueSeconds(0),
        mTTTime(NULL) {
        QDate date(year, month, 1);
       if(!date.isValid()) DEBUG("Invalid date !");
       else DEBUG("Date: " + date.toString("yyyy"));
       initVariables(date);
    }

    TimeTrackingModel() :
        mPrevMonthlyOverDueSeconds(0),
        mCompensatedOverDueSeconds(0),
        mTTTime(NULL) {
        initVariables(QDateTime::currentDateTime().date());
    }

    virtual ~TimeTrackingModel() {
        delete mTTTime;
    }


    virtual QString getDefaultFileName(const QString& userName = "", bool exportToPDF = false)  const;
    virtual QString getPrevDefaultFileName(const QString& userName = "", int index = 0, bool exportToPDF = false) const;
    virtual QString getNextDefaultFileName(const QString& = "", int index = 0, bool exportToPDF = false) const;
    QString createFileName(const QString& userName, int year, int month, bool exportToPDF = false) const;
    static QString getFileNameRegExp(const QString &userName);
    static QString getDirectory();


    virtual void setNewBaseDate(const QDate& date)                           ;
    virtual void setUserName(const QString& userName)                        ;

    virtual QString getUserName() const;

    virtual QDate getBaseDate() const;

    virtual bool isModelOfCurMonth()                                      const;

    virtual void setStartTime();
    virtual void setEndTime()                                                ;
    virtual void setPause(const QTime& pause);
    virtual void setComment(const QString& comment);

    virtual int getCurrentDay()                                         const;
    virtual int daysInMonth()                                           const;

    virtual const QTime getStartTime(int monthDay = -1)                 const;
    virtual const QTime getEndTime(int monthDay = -1)                   const;
    virtual const QTime getWorkTime(int monthDay = -1)                  const;

    virtual const QTime getPause(int monthDay = -1)                     const;
    virtual const QTime getRequiredWorkTime(int monthDay = -1)          const;
    virtual const QTime getDuedOverHours()                              const;
    virtual const QString getComment(int monthDay = -1)                 const;
    virtual const QString getDayName(int monthDay)                      const;

    virtual void setStartTime(int monthDay, const QTime& newTime, bool changeStartTimeOnly)            ;
    virtual void setEndTime(int monthDay, const QTime& newTime)              ;
    virtual void setPause(int monthDay, const QTime& newTime = mINVALID_DATE.time());
    virtual void setComment(int monthDay, const QString& comment);
    virtual void setRequiredWorkTime(int monthDay, const QTime& newTime = mINVALID_DATE.time());

    virtual bool isWorkingDay(const QDate& date)                        const {
        return TimeTrackingSettings::getInstance()->isWorkingDay(date);
    }

    virtual bool isWorkingDay(int monthDay)                             const {
        return TimeTrackingSettings::getInstance()->isWorkingDay(
                    QDate(mTTDate.year(), mTTDate.month(), monthDay));
    }

    virtual QTime getTimeWorkedDiff(int monthDay = -1)                  const;
    virtual bool timeWorkDiffNegativ(int monthDay = -1)                 const;
    virtual QTime differenceFromStartToNow(bool & isNegativ)            const;

    virtual bool startTimeIsValid(int monthDay = -1)                    const;
    virtual bool endTimeIsValid(int monthDay = -1)                      const;
    virtual bool pauseIsValid(int monthDay = -1)                        const;
    virtual bool requiredWorkTimeIsValid(int monthDay = -1)                 const;

    virtual QTime getDefaultRequiredWorkTime(int monthDay)              const;

    virtual void setPrevMonthlyOverDueSeconds(int prevMonthlyOverDueSeconds);
    virtual int getPrevMonthlyOverDueSeconds(void)                      const;
    virtual int getMonthlySeconds()                                     const;
    virtual void setCompensatedOverDueSeconds(int compensatedOverDueSeconds);
    virtual int getCompensatedOverDueSeconds()                          const;
    // TODO: Rename me getMonthBalance
    virtual int getMonthlyOverDueSeconds()                              const;
    virtual int getMonthOnlyOverDueSeconds()                            const;

    virtual QString getPrevMonthlyOverDueSecondsInHours()               const;
    virtual QString getMonthlySecondsInHours()                          const;
    virtual QString getMonthlyOverDueSecondsInHours()                   const;
    QString getMonthOnlyOverDueSecondsInHours()                          const;
    bool hasError() const {
        return mErrorList.empty() == false;
    }
    QString getError() {
        QString error = mErrorList.join("\n");
        mErrorList.clear();
        return error;

    }

    DynamicField * getDynamicFieldFromName(const QString & name) const;
    void createDynamicComboBox(const QString & comboboxName, const QStringList & values);
    void createDynamicTextField(const QString & textFieldName);
    void removeDynamicField(const QString & name);
    const QString & getDynamicFieldSelection(const QString & dynamicFieldName, const QDate& date);
    void setDynamicFieldSelection(const QString & dynamicFieldName, const QDate & date, const QString & value);


    const QStringList & getComboValuesFromDynamicFieldName(const QString & name) const;
    size_t getDynamicFieldCount() const;
    const QString & getDynamicFieldNameFromIndex(size_t index) const;

signals:
    void somethingChanged();


private:
    void initVariables(const QDate & date) {

        mTTDate = date;
        mTTTime = new std::vector<TTTime>(mTTDate.daysInMonth());
        mUserName = Misc::getUserName();

    }

    QDate mTTDate;
    int mPrevMonthlyOverDueSeconds;
    int mCompensatedOverDueSeconds;
    static const QDateTime mINVALID_DATE;
    QString mUserName;
    mutable QStringList mErrorList;

    struct TTTime {
        QDateTime   startedTime;
        QDateTime   endedTime;
        QTime       pause;
        QTime       requiredTimePerDay;
        QString     comment;
        int         previousBalance;
        bool        empty;
        bool        endedTimeIsValid;
        TTTime() : empty(true), endedTimeIsValid(false) {
        }
    };

    std::vector<TTTime> * mTTTime;
    std::vector<DynamicField *> mVectorDynamicFields;

    bool isModelOfCurMonth(const QDate& date)                            const;
    void assertMonthYearOK(const QDate& date)                                 ;
    int monthDayToIndex(int monthDay)                                    const;
    bool endMinusStartMinusPauseValid(int monthDay)                      const;
    QTime endMinusStartMinusPause(int monthDay = -1)                     const;
    void addError(const QString & error) const {
        mErrorList.append(error);
    }


};

#endif // TIMETRACKINGMODEL_H
