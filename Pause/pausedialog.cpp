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

#include "pausedialog.h"
#include "ui_pausedialog.h"
#include "debug.h"

#include <QMessageBox>

PauseDialog::PauseDialog(QWidget *parent, const QTime & currentPause) :
    QDialog(parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint),
    mTimer(new QTimer(this)),
    mCurrentPause(currentPause),
    mValidPause(true),
    ui(new Ui::PauseDialog)
{
    ui->setupUi(this);
    connect(mTimer.data(), SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(ui->checkBoxIncludeExistingOrDefaultPause, SIGNAL(stateChanged(int)),
            this, SLOT(onCheckBoxIncludeExistingOrDefaultPause(int)));
    //setAttribute(Qt::WA_TranslucentBackground);

    mTimer->setInterval(1000);
    mTimer->start();

    ui->checkBoxIncludeExistingOrDefaultPause->setText(
                ui->checkBoxIncludeExistingOrDefaultPause->text() + mCurrentPause.toString("hh:mm:ss"));

    mStartedDate = QDate::currentDate();
    mStartedTime = QTime::currentTime();
    ui->labelPauseStartedOn->setText(ui->labelPauseStartedOn->text() +
                                     mStartedDate.toString("dd MMM") +
                                     tr(" at ") +
                                     QTime::currentTime().toString("hh:mm"));
    setNewTime();

}

// =============================================================================
PauseDialog::~PauseDialog()
{
    delete ui;
}

// =============================================================================
QTime PauseDialog::getPause() const
{
    return ui->timeEditFullPauseTime->time();
}

// =============================================================================
void PauseDialog::onTimer()
{
    mTimer->stop();
    QDate date = QDate::currentDate();
    if(date != mStartedDate) {
        QMessageBox::warning(this, tr("Exceeded pause !"),
                             tr("Your pause excedeed the day when it was started. \n"
                                "Please adjust yourself the pause in the table when you are back."));
        mValidPause = false;
        close();
        return;
    }

    setNewTime();
    mTimer->start();

}

// =============================================================================
void PauseDialog::setNewTime() {
    int elapsedSeconds = mStartedTime.secsTo(QTime::currentTime());
    if(ui->checkBoxIncludeExistingOrDefaultPause->isChecked()) {
        elapsedSeconds += mCurrentPause.hour() * 3600 +
                                      mCurrentPause.minute() * 60 +
                                      mCurrentPause.second();
    }
    ui->timeEditFullPauseTime->setTime(QTime(elapsedSeconds / 3600, (elapsedSeconds / 60) % 60, elapsedSeconds % 60));


}

// =============================================================================
void PauseDialog::onCheckBoxIncludeExistingOrDefaultPause(int newState)
{
    Q_UNUSED(newState);
    setNewTime();
}

