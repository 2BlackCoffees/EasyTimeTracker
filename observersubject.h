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

#ifndef OBSERVERSUBJECT_H
#define OBSERVERSUBJECT_H

#include <vector>
#include "iobserver.h"

class ObserverSubject
{
private:
    std::vector<IObserver*> mObserverVector;
    ObserverSubject(ObserverSubject&);
    ObserverSubject& operator=(ObserverSubject&);
    bool mNotifying;
public:
    ObserverSubject() : mNotifying(false) {

    }

    void attach(IObserver* observer);
    void detach(IObserver* observer);
    void notifyAll(IObserver* observer);
    bool notifying() const {
        return mNotifying;
    }

    virtual ~ObserverSubject() {

    }
};

#endif // OBSERVERSUBJECT_H
