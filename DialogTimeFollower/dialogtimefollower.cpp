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

#include "dialogtimefollower.h"
#include "ui_dialogtimefollower.h"
#include "debug.h"
#include "misc.h"
#include "timetrackingmodel.h"

#include <QMouseEvent>
#include <QPoint>
#include <QDesktopWidget>
#include <QApplication>

DialogTimeFollower::DialogTimeFollower(QWidget *parent, TimeTrackingModel * timeTrackingModel) :
    QDialog(parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint),
    ui(new Ui::DialogTimeFollower), mWindowIsClicked(false), mTimeTrackingModel(timeTrackingModel)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(1.0);
    connect(ui->horizontalSlider,   SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    connect(ui->pushButtonStart,    SIGNAL(clicked()), SIGNAL(start()));
    connect(ui->pushButtonStop,     SIGNAL(clicked()), SIGNAL(stop()));
    connect(ui->pushButtonPause,    SIGNAL(clicked()), SIGNAL(pause()));

    connect(ui->pushButtonStart,    SIGNAL(clicked()), this, SLOT(onTimeout()));
    connect(ui->pushButtonStop,     SIGNAL(clicked()), this, SLOT(onTimeout()));
    connect(ui->pushButtonPause,    SIGNAL(clicked()), this, SLOT(onTimeout()));

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    mTimer.setInterval(2000);
    mTimer.start();
    onTimeout();
    ui->horizontalSlider->setMinimum(10);
    ui->horizontalSlider->setMaximum(100);

/*    QDesktopWidget * desktopWidget = QApplication::desktop();
    move(desktopWidget->width()  - size().width(),
         desktopWidget->height() - size().height());*/

}

// =============================================================================
DialogTimeFollower::~DialogTimeFollower()
{
    delete ui;
}

// =============================================================================
void DialogTimeFollower::onSliderValueChanged(int value)
{
    setWindowOpacity(1.0 * value / ui->horizontalSlider->maximum());
}

// =============================================================================
void DialogTimeFollower::onTimeout()
{
    // User clicked on stop
    QString color = "black";
    bool startTimeIsValid = mTimeTrackingModel->startTimeIsValid();
    bool endTimeIsValid = mTimeTrackingModel->endTimeIsValid();
    ui->pushButtonStart->setVisible(startTimeIsValid == false || endTimeIsValid == true);
    ui->pushButtonStop->setVisible(endTimeIsValid == false && startTimeIsValid == true);
    ui->pushButtonPause->setVisible(endTimeIsValid == false && startTimeIsValid == true);
    if(endTimeIsValid) {
        color = "blue";
    }

    bool isNegativ;
    QTime remainingTime = mTimeTrackingModel->differenceFromStartToNow(isNegativ);
    if(remainingTime.isValid()) {
        QString label = remainingTime.toString("hh:mm");
        if(isNegativ) {
            label = "-" + label;
        }
        ui->labelDayRemaining->setText("<font color='" + color + "'>" + label + "</font>");
    } else {
        ui->labelDayRemaining->setText("Zzz..");
    }

    ui->labelMonthWorked->setText("Month: " + MiscDateTime::secondsToHours(mTimeTrackingModel->getMonthlySeconds()));
    ui->labelBalance->setText("Bal: " + MiscDateTime::secondsToHours(mTimeTrackingModel->getMonthlyOverDueSeconds()));
}

// =============================================================================
void DialogTimeFollower::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mWindowIsClicked = false;
}

// =============================================================================
void DialogTimeFollower::mouseMoveEvent(QMouseEvent *event)
{
    if(mWindowIsClicked) {
        move(mWindowPos + event->globalPos() - mClickedPoint);
    }
}

// =============================================================================
void DialogTimeFollower::mousePressEvent(QMouseEvent *event) {
    mClickedPoint = event->globalPos();
    mWindowPos = pos();
    mWindowIsClicked = true;
}


// =============================================================================
MoveableLabel::MoveableLabel(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent, f), mParent(parent)
{

}

// =============================================================================
MoveableLabel::MoveableLabel(const QString &text, QWidget *parent, Qt::WindowFlags f) :
    QLabel(text, parent, f), mParent(parent)
{

}

// =============================================================================
void MoveableLabel::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

// =============================================================================
void MoveableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
}

// =============================================================================
void MoveableLabel::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}
