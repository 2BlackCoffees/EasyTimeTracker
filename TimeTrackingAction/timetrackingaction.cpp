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

#include "timetrackingaction.h"
#include "timetrackingsettings.h"

#include "debug.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QIcon>

TimeTrackingAction::TimeTrackingAction(void) :
    mIndex(0),
    mUserCurrentlyWorking(false)
{
#ifdef NEVER_EVER_UNCOMMENT_THIS_IT_IS_DANGEROUS_FOR_YOUR_HEALTH_THIS_IS_ONLY_FOR_OWN_PURPOSES
    (QString("http://youtu.be/xgJZppUtz8o"));
    (QString("http://youtu.be/cdrR9YdTHrI"));
    (QString("http://youtu.be/Tw9cDhK27rg"));
    (QString("http://youtu.be/fm-dU5zNtfw"));
    (QString("http://youtu.be/Y2Nb5LbkrGE"));
    (QString("http://youtu.be/EaCe1JuF46E"));
    (QString("http://youtu.be/mARo2dCiMTI"));
#endif
    mTimerDefaultAction.reset(new QTimer(this));
    mTimerPause.reset(new QTimer(this));

    connect(mTimerDefaultAction.get(),    SIGNAL(timeout()) ,
            this, SLOT(updateTimerDefaultAction()));
    connect(mTimerPause.get(),            SIGNAL(timeout()) ,
            this, SLOT(updateTimerPause()));

}

// =============================================================================
void TimeTrackingAction::resetActions()
{
    mActions = TimeTrackingSettings::getInstance()->getActions();
    if(mIndex > mActions.size()) {
        mIndex = 0;
    }
}

// =============================================================================
void TimeTrackingAction::setUserCurrentlyWorking(bool userCurrentlyWorking) {
    mUserCurrentlyWorking = userCurrentlyWorking;
    if(mUserCurrentlyWorking) {
        resetTimerDefaultAction();
        resetActions();
        resetTimerPause();
    } else {

        mTimerDefaultAction->stop();
        mTimerPause->stop();

    }
}

// =============================================================================
void TimeTrackingAction::updateTimerDefaultAction() {

    mTimerDefaultAction->stop();

    if(userCurrentlyWorking() &&
            TimeTrackingSettings::getInstance()->getEnablePeriodDefaultAction()) {
        action();
        resetTimerDefaultAction();
    }
}

// =============================================================================
void TimeTrackingAction::resetTimerDefaultAction() {
    QTime tmpTime = TimeTrackingSettings::getInstance()->getPeriodDefaultAction();
    mTimerDefaultAction->setInterval((tmpTime.hour() * 3600 +
                         tmpTime.minute() * 60 +
                         tmpTime.second()) * 1000);
    mTimerDefaultAction->start();


}

// =============================================================================
void TimeTrackingAction::updateTimerPause() {

    mTimerPause->stop();

    if(userCurrentlyWorking() &&
        TimeTrackingSettings::getInstance()->getEnablePeriodPauseTime()) {
        QMessageBox msgBox(NULL);
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setWindowTitle(QObject::tr("Pause time !"));
        msgBox.setText( TimeTrackingSettings::getInstance()->getPauseString());
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::ApplicationModal);
        msgBox.setVisible(true);
        msgBox.setWindowIcon(QIcon(":/images/tt_icon.png"));

        msgBox.exec();
        resetTimerPause();
    }

}

// =============================================================================
void TimeTrackingAction::resetTimerPause() {
    QTime tmpTime = TimeTrackingSettings::getInstance()->getPeriodPauseTime();
    mTimerPause->setInterval((tmpTime.hour() * 3600 +
                         tmpTime.minute() * 60 +
                         tmpTime.second()) * 1000);
    mTimerPause->start();

}


// =============================================================================
bool TimeTrackingAction::userCurrentlyWorking() {
    return mUserCurrentlyWorking;
}

// =============================================================================
void TimeTrackingAction::action() {

    if(mIndex > mActions.size()) {
        mIndex = 0;
    }
    int curIndex = 0;
    QMultiHash<QString, QString>::ConstIterator it =
            mActions.constBegin();
    while(curIndex < mIndex) {
        ++it;
        ++curIndex;
    }
    if(it != mActions.constEnd()) {
        QString command = it.key();
        QString parameter = it.value();

        DEBUG("Starting " + command + " " + parameter);
        if(TimeTrackingSettings::getInstance()->isCommandDefaultBrowser(command)) {
            QDesktopServices::openUrl(parameter);
        } else {
            Q_ASSERT(false);
        }

        ++mIndex;
    } else {
        mIndex = 0;
    }
}


