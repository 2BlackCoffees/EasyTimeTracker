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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "iiosettings.h"
#include "misc.h"

#include "QString"
#include "QDate"

class IOSettings : public IIOSettings
{
public:
    IOSettings() : mUserName(Misc::getUserName()),
        mSlashReplacement("<**||**>"),
        mMainKey("EasyTimeTracker - 2BlackCoffees - " + mUserName)
    {
        mDate = QDate::currentDate().addDays(-14);
        while(mDate.dayOfWeek() != 1){
            mDate = mDate.addDays(1);
        }

    }

    virtual bool saveSettings(TimeTrackingSettings * timeTrackingSettings) const;
    virtual bool readSettings(TimeTrackingSettings * timeTrackingSettings);

    ~IOSettings() {

    }
private:
    QString mUserName;
    QString mSlashReplacement;
    QString mMainKey;
    QDate mDate;
};

#endif // SETTINGS_H
