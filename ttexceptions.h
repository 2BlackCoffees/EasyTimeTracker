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

#ifndef TTEXCEPTIONS_H
#define TTEXCEPTIONS_H

#include <stdexcept>
#include <QString>
//#pragma warning( disable : 4290 )

class TTExceptionWrongBaseDate  : public std::runtime_error
{
public:
    TTExceptionWrongBaseDate(const QString& msg) : std::runtime_error(msg.toStdString()) {

    }
};

class TTExceptionWrongBaseTime : public std::runtime_error
{
public:
    TTExceptionWrongBaseTime(const QString& msg) : std::runtime_error(msg.toStdString()) {

    }
};

class TTExceptionDayChangedSinceTTStarted : public std::runtime_error
{
public:
    TTExceptionDayChangedSinceTTStarted(const QString& msg) : std::runtime_error(msg.toStdString()) {

    }
};

class TTExceptionOverflow : public std::runtime_error
{
public:
    TTExceptionOverflow(const QString& msg) : std::runtime_error(msg.toStdString()) {

    }
};

class TTExceptionTimeConfiguration : public std::runtime_error
{
public:
    TTExceptionTimeConfiguration(const QString& msg) : std::runtime_error(msg.toStdString()) {

    }
};

class TTExceptionWrongInitialisation : public std::runtime_error
{
public:
    TTExceptionWrongInitialisation(const QString& msg) : std::runtime_error(msg.toStdString()) {

    }
};



#endif // TTEXCEPTIONS_H
