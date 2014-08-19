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

#include "QSettings"
#include "iosettings.h"

// =============================================================================
bool IOSettings::saveSettings(TimeTrackingSettings * timeTrackingSettings) const {
    QSettings settings(mMainKey, "Settings");
    settings.setValue("alreadySaved", true);
    settings.setValue("actionPeriod",
                      timeTrackingSettings->getPeriodDefaultAction());
    settings.setValue("actionEnable",
                      timeTrackingSettings->getEnablePeriodDefaultAction());
    settings.setValue("pauseTimePeriod",
                      timeTrackingSettings->getPeriodPauseTime());
    settings.setValue("pauseEnable",
                      timeTrackingSettings->getEnablePeriodPauseTime());
    
    settings.setValue("printingRealName",
                      timeTrackingSettings->getPrintingRealName());
    settings.setValue("printingFooter",
                      timeTrackingSettings->getPrintingFooter());
    settings.setValue("printingShowPicture",
                      timeTrackingSettings->getPrintingShowPicture());

    for(int day = 0; day < 7; day++) {
        settings.setValue(QString("isWorkingDay-%1").arg(day),
                          timeTrackingSettings->isWorkingDay(mDate.addDays(day)));
        settings.setValue(QString("requiredWorkDay-%1").arg(day),
                          timeTrackingSettings->getRequiredWorkTime(mDate.addDays(day)));
        settings.setValue(QString("pause-%1").arg(day),
                          timeTrackingSettings->getPause(mDate.addDays(day)));

    }


    settings.setValue("Printing/realName", timeTrackingSettings->getPrintingRealName());
    settings.setValue("Printing/footer", timeTrackingSettings->getPrintingFooter());
    settings.setValue("Printing/showPicture", timeTrackingSettings->getPrintingShowPicture());

    // First clear all existing actions
    settings.remove("action");

#define DEBUG_REMOVE_ALL_ACTIONS
#ifdef DEBUG_REMOVE_ALL_ACTIONS
    // Now save all actions existing in TimeTrackingSettings
    QMap<QString, QString> actions = timeTrackingSettings->getActionsWithKeys();
    for(auto pair: actions.toStdMap()) {
        QString key = pair.first;
        while(key.indexOf('/') != -1) {
            key.replace("/", mSlashReplacement);
        }
        settings.setValue("action/" + key, pair.second);
        DEBUG(QString("Saving action %1 changed to %2 %3").arg(pair.first).arg(key).arg(pair.second));

    }
#endif
    return true;
}

// =============================================================================
bool IOSettings::readSettings(TimeTrackingSettings * timeTrackingSettings) {

    QSettings settings(mMainKey, "Settings");

    if(settings.value("alreadySaved").toBool()) {
        QTime tmpTime = settings.value("actionPeriod").toTime();
        if(tmpTime.isValid()) {
            timeTrackingSettings->setPeriodDefaultAction(tmpTime);
        } else {
            DEBUG(QObject::tr("Invalid time saved in properties"));
        }
        timeTrackingSettings->setEnablePeriodDefaultAction(settings.value("actionEnable").toBool());

        tmpTime = settings.value("pauseTimePeriod").toTime();
        if(tmpTime.isValid()) {
            timeTrackingSettings->setPeriodPauseTime(tmpTime);
        } else {
            DEBUG(QObject::tr("Invalid time saved in properties"));
        }
        timeTrackingSettings->setEnablePeriodPauseTime(settings.value("pauseEnable").toBool());

        for(int day = 0; day < 7; day++) {
            timeTrackingSettings->setWorkingDay(mDate.addDays(day),
                                                settings.value(QString("isWorkingDay-%1").arg(day)).toBool());
            timeTrackingSettings->setRequiredWorkTime(mDate.addDays(day),
                                                      settings.value(QString("requiredWorkDay-%1").arg(day)).toTime());
            timeTrackingSettings->setPause(mDate.addDays(day),
                                           settings.value(QString("pause-%1").arg(day)).toTime());

        }

        if(settings.contains("Printing/realName")) {
            timeTrackingSettings->setPrintingRealName(settings.value("Printing/realName").toString());
        } else {
            timeTrackingSettings->setPrintingRealName("Two Black Coffees");
        }

        if(settings.contains("Printing/footer")) {
            timeTrackingSettings->setPrintingFooter(settings.value("Printing/footer").toString());
        } else {
            timeTrackingSettings->setPrintingFooter("Signature TwoBlackCoffees _________  Signature manager Three Black Coffees: _______");
        }
        if(settings.contains("Printing/showPicture")) {
            timeTrackingSettings->setPrintingShowPicture(settings.value("Printing/showPicture").toBool());
        } else {
            timeTrackingSettings->setPrintingShowPicture(true);
        }


        QStringList allKeys = settings.allKeys();
        for(QString key: allKeys.toStdList()) {
            if(key.startsWith("action/")) {
                QString tmpKey = key.mid(key.indexOf('/') + 1);
                tmpKey.replace(mSlashReplacement, "/");

                DEBUG(QString("Reading action %1: %2, %3").arg(key).arg(tmpKey).arg(settings.value(key).toString()));
                timeTrackingSettings->addActionFromKey(tmpKey,
                                                       settings.value(key).toString());
            }
        }

        return true;
    }
    return false;
}
