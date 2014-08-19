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

#ifndef IIOFILEACCESSES_H
#define IIOFILEACCESSES_H

#include <QString>
#include <QDate>
#include <QString>

class TimeTrackingModel;
class IIOFileAccesses {
public:

    virtual bool openSheetInformation(TimeTrackingModel* timeTrackingModel = NULL,
                                      bool silent = false)                                 = 0;
    virtual bool saveSheetInformation()                                                    = 0;
    virtual bool setPreviousBalance()                                                      = 0;

    virtual ~IIOFileAccesses() {
    }
};
#endif // IIOFILEACCESSES_H
