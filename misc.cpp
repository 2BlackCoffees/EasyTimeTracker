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

#include "misc.h"

const QDate MiscDateTime::mInitDate = QDate(1, 1, 1);
const QTime MiscDateTime::mInitTime = QTime(0, 0, 0);

QString Misc::getUserName() {

#ifndef _WIN32
   return getenv("USER"); ///for MAC or Linux

#else
    return std::getenv("USERNAME");

#endif
}


// =============================================================================
QTime MiscDateTime::stringToTime(const QString& text) {
    QStringList listText = text.split(":"/*QRegExp("[:-\\.]")*/);
    int hour = 0;
    int minute = 0;
    int second = 0;
    if(listText.size() > 0)  hour   = listText.at(0).toInt();
    if(listText.size() > 1)  minute = listText.at(1).toInt();
    if(listText.size() > 2)  second = listText.at(2).toInt();

    QTime tmpTime = QTime(hour, minute, second);
    if(!tmpTime.isValid()) {
        throw (TTExceptionWrongBaseTime(QString(
                                            QObject::tr("Time defined by hour %1, minute %2, second %3 is invalid.").
                                            arg(hour, minute, second))));
    }

    return tmpTime;
}

// =============================================================================
int MiscDateTime::stringHoursToSeconds(const QString& text) {
    int allSecs = 0;
    QString tmpText = text;
    bool negativ = tmpText.length() > 0 && tmpText.at(0) == '-';
    if(negativ) {
        tmpText = tmpText.mid(1);
    }

    QStringList listText = tmpText.split(":"/*QRegExp("[:-\\.]")*/);

    if(listText.size() > 2) {
        allSecs = listText.at(0).toInt() * 3600 +
                listText.at(1).toInt() * 60 +
                listText.at(2).toInt();

    } else if(listText.size() > 1) {
        allSecs = listText.at(0).toInt() * 3600 +
                listText.at(1).toInt() * 60;

    } else if(listText.size() > 0) {
        allSecs = listText.at(0).toInt() * 3600;
    }

    if(negativ) allSecs = -allSecs;

    return allSecs;

}

// =============================================================================
QDate MiscDateTime::stringToDate(const QString& text) {
    QStringList listText = text.split("-"/*QRegExp("[:-\\.]")*/);
    int day = 1;
    int month = 1;
    int year = 1;
    if(listText.size() > 0)  day   = listText.at(0).toInt();
    if(listText.size() > 1)  month = listText.at(1).toInt();
    if(listText.size() > 2)  year = listText.at(2).toInt();

    DEBUG(QString("stringToDate: Day: %1, Month: %2, Year: %3").arg(day).arg(month).arg(year));
    QDate tmpDate = QDate(year, month, day);
    if(!tmpDate.isValid()) {
        throw (TTExceptionWrongBaseDate(QString(
                                            QObject::tr("Date defined by day %1, month %2, year %3 is invalid.").
                                            arg(day, month, year))));
    }
    return tmpDate;
}

// =============================================================================
QDateTime MiscDateTime::stringToDateTime(const QString& text) {
    QStringList listText = text.split(" ");

    if(listText.size() > 1) {
        DEBUG(QString("String1: %1, String2: %2").arg(listText.at(1)).arg(listText.at(0)));
        return QDateTime(stringToDate(listText.at(0)), stringToTime(listText.at(1)));
    }

    if(listText.size() > 0) {
        return QDateTime(mInitDate, stringToTime(listText.at(0)));
    }

    return QDateTime(mInitDate, mInitTime);

}

// =============================================================================
QString MiscDateTime::secondsToHours(int seconds) {
    bool negativ = seconds < 0;
    if(negativ) {
        seconds = -seconds;
    }
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    QString out = QString("%1:%2").arg(hours).arg(minutes, 2, 10, (const QChar)'0');
    if(negativ) {
        out = "-" + out;
    }
    return out;
}
