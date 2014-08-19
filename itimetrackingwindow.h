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

#ifndef ITIMETRACKINGWINDOW_H
#define ITIMETRACKINGWINDOW_H

#include <QMainWindow>
class TimeTrackingModel;
//class QMainWindow;

class ITimeTrackingWindow : public QMainWindow
{
public:

    virtual ~ITimeTrackingWindow()                         = 0;

    virtual void copy()                             const  = 0;
    virtual void cut()                                     = 0;
    virtual void paste()                                   = 0;

    virtual void startTT()                                 = 0;
    virtual void stopTT()                                  = 0;
    virtual void pauseTT()                                 = 0;

    virtual bool isStarted()                        const  = 0;
    virtual bool isStopped()                        const  = 0;

    virtual void lockUnlock(bool unlock)                   = 0;
    virtual bool isSheetOfCurMonth()                const  = 0;
    virtual bool openSpreadsheet(bool silent)              = 0;

    virtual void exportSheet(const QString & path)   const = 0;
    virtual void printSheet()                              = 0;
    virtual void printPreview()                            = 0;

    virtual TimeTrackingModel * getTrackingModel()  const  = 0;
};

#endif // ITIMETRACKINGWINDOW_H
