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

#ifndef GENERAL_H
#define GENERAL_H

#include <iostream>
#include <vector>
#include "QString"

#define DEBUG(debugInfo) Debug::printDebug(debugInfo, __FILE__, __LINE__)
// This is for MSVC
//#define DEBUG_OBJ(debugInfo)  Debug varDebug ## __FUNCDNAME__(debugInfo + QString(__FUNCDNAME__), __FILE__, __LINE__)

#define DEBUG_OBJ(debugInfo)  Debug varDebug ## __PRETTY_FUNCTION__(debugInfo + QString(__PRETTY_FUNCTION__ ), __FILE__, __LINE__)

#define DEBUG_GET_STACK()  Debug::printStack()
class TTQString : public QString
{
public:
    TTQString(const QString& string) : QString(string) {}
    TTQString(const char* string) : QString(string) {}

    friend std::ostream& operator<<(std::ostream& os, const TTQString& string) {
        os << qPrintable(string);
        return os;
    }

};

class Debug {
    TTQString mDebugInfo;
    TTQString mFromFile;
    int mFromLine;
    static int id;
    static std::vector<TTQString> stackInfo;

public:
    Debug(const TTQString& debugInfo, const TTQString& fromFile, const int fromLine):
        mDebugInfo(debugInfo), mFromFile(fromFile), mFromLine(fromLine) {
        id++;
        printDebug("Entering " + debugInfo, fromFile, fromLine);
        stackInfo.push_back(debugInfo);
    }

    ~Debug() {
        id--;
        printDebug("Leaving " + mDebugInfo, mFromFile, mFromLine);
        stackInfo.pop_back();

    }

    static int getId() {
        return id;
    }

    static void printStack() {

#ifdef PRINT_DEBUG

        for(int index = stackInfo.size() - 1; index >= 0; index--) {
            std::cout <<  "FrameStack: " << index << " - " << stackInfo.at(index) << std::endl;
        }

#endif

    }

    static void printDebug(const TTQString& debugInfo, const TTQString& fromFile, const int fromLine) {

#ifdef PRINT_DEBUG

        std::cout << "File: " << fromFile <<
                     ":" << fromLine <<
                     " >>" << debugInfo << "<<" << std::endl;

#endif

    }

};

#endif // GENERAL_H
