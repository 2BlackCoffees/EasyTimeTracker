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

#include "observersubject.h"
#include "debug.h"

#include <algorithm>

// =============================================================================
void ObserverSubject::attach(IObserver* observer) {
    DEBUG_OBJ("");
    if(std::find(mObserverVector.begin(), mObserverVector.end(), observer) == mObserverVector.end()) {
        mObserverVector.push_back(observer);
    }
}

// =============================================================================
void ObserverSubject::detach(IObserver* observer) {
    DEBUG_OBJ("");
    mObserverVector.erase(std::remove(mObserverVector.begin(), mObserverVector.end(), observer),
                          mObserverVector.end());

}

// =============================================================================
void ObserverSubject::notifyAll(IObserver *observer) {
    DEBUG_OBJ("");
    mNotifying = true;
    for(std::vector<IObserver*>::const_iterator it = mObserverVector.begin();
        it != mObserverVector.end(); it++) {
        if(*it != observer) {
            (*it)->update();
        }
    }
    mNotifying = false;
}
