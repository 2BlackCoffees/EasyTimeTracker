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

#ifndef DIALOGTIMEFOLLOWER_H
#define DIALOGTIMEFOLLOWER_H

#include <QDialog>
#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QTimer>

class QPoint;
class QMouseEvent;
class TimeTrackingModel;

namespace Ui {
class DialogTimeFollower;
}

class DialogTimeFollower : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTimeFollower(QWidget *parent, TimeTrackingModel * timeTrackingModel);
    ~DialogTimeFollower();

    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

signals:
    void start();
    void stop();
    void pause();

private slots:
    void onSliderValueChanged(int value);
    void onTimeout();

private:
    Ui::DialogTimeFollower *ui;
    QPoint mClickedPoint;
    QPoint mWindowPos;
    bool mWindowIsClicked;
    TimeTrackingModel * mTimeTrackingModel;
    QTimer mTimer;
};

class MoveableLabel : public QLabel {
public:
    explicit MoveableLabel(QWidget *parent=0, Qt::WindowFlags f=0);
    explicit MoveableLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);

    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

private:
    QWidget * mParent;
};

#endif // DIALOGTIMEFOLLOWER_H
