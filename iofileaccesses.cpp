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

#include "iofileaccesses.h"
#include "ttexceptions.h"
#include "timetrackingmodel.h"
#include "debug.h"

#include <QObject>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>

const int IOFileAccesses::IDX_FILENAME_SHIFT_YEAR = 11;
const int IOFileAccesses::IDX_FILENAME_SHIFT_MONTH = 6;
/*
 *  <ETT version = "0.1"
 *       userName ="userName"
 *       year = "year yyyy"
 *       month="month From 1 to 12"
 *       prevMonthlyOverDueHours="previous Monthly Hours">
 *
 *    <DAY number = "1 to 31"
 *         started = "started Time HH:mm"
 *         ended = "ended Time HH:mm"
 *         pause = "pause Time HH:mm"
 *         requiredWorkTime = "required Work Time HH:mm"
 *         comment = "comment" />
 *  </ETT>
 *
 **/
// =============================================================================
bool IOFileAccesses::openSheetInformation(TimeTrackingModel* timeTrackingModel,
                                          bool silent)
{
    bool returnValue = false;

    if(timeTrackingModel == NULL) {
        timeTrackingModel = mTimeTrackingModel;
    }

    QFile file(timeTrackingModel->getDefaultFileName());
    if(mDebug) DEBUG("Trying to open " + timeTrackingModel->getDefaultFileName());
    if(file.exists()) {

        if (file.open(QFile::ReadOnly)) {
            returnValue = openSheetInformation(&file, timeTrackingModel);
            file.close();
        }

    } else {
        if(!silent){
            QMessageBox::information(NULL, QObject::tr("Problem opening file"),
                                     QObject::tr("Could not open file ") + file.fileName() + ".");
        }
    }

    return returnValue;
}

// =============================================================================
QStringList IOFileAccesses::getFileList(const QString & userName, int fromYear, int fromMonth) {
    QDir path(TimeTrackingModel::getDirectory());
    QStringList files = path.entryList(QDir::Files);
    QStringList listOfFiles;
    //DEBUG("Searching in path: " + path.absolutePath());
    QRegularExpression re(TimeTrackingModel::getFileNameRegExp(userName));
    foreach (QString file, files) {
        QRegularExpressionMatch match = re.match(file);
        if (match.hasMatch()) {
            //DEBUG("Matched: File " + file);
            QString year = match.captured(1);
            QString month = match.captured(2);
            if((fromYear  < 0 || (fromYear  >= year.toInt())) &&
               (fromMonth < 0 || (fromMonth >= month.toInt()))) {
                listOfFiles.append(file);
            }
        } else {
            //DEBUG("FAILED: File " + file);

        }
    }

    if(listOfFiles.count() > 0) {
        listOfFiles.sort();
    }

    return listOfFiles;
}


// =============================================================================
void IOFileAccesses::updateAllBalances(const QString & userName)
{
    DEBUG_OBJ("");
    QStringList fileList = getFileList(userName);
    for(int index = 1; index < fileList.size(); index++) {
        QString fileName = fileList[index];
        int year = fileName.mid(fileName.size() - IDX_FILENAME_SHIFT_YEAR, 4).toInt();
        int month = fileName.mid(fileName.size() - IDX_FILENAME_SHIFT_MONTH, 2).toInt();
        TimeTrackingModel timeTrackingModel(month, year);
        IOFileAccesses ioFileAccesses(&timeTrackingModel);
        ioFileAccesses.openSheetInformation(NULL, true);
        DEBUG("Calling setPreviousBalance for file " + fileName);
        ioFileAccesses.setPreviousBalance(fileList);
        // Saving
        QFile file(timeTrackingModel.getDefaultFileName());
        if (file.open(QIODevice::WriteOnly)) {
            ioFileAccesses.saveSheetInformation(&file, &timeTrackingModel);
            file.close();
        }

    }
}

// =============================================================================
bool IOFileAccesses::openSheetInformation(QIODevice* file,
                                          TimeTrackingModel* timeTrackingModel)
{
    QString errorStr;
    int errorLine = -1;
    int errorColumn = -1;

    if(timeTrackingModel == NULL) {
        timeTrackingModel = mTimeTrackingModel;
    }

    if (!mDomDocument.setContent(file, true, &errorStr, &errorLine,
                                 &errorColumn)) {
        QMessageBox::information(NULL, QObject::tr("Error reading XML file ") +
                                 timeTrackingModel->getDefaultFileName(),
                                 QObject::tr("Filename ") + timeTrackingModel->getDefaultFileName() + "\n" +
                                 QString(QObject::tr("Parse error at line %1, column %2:\n%3"))
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return false;
    }


    QDomElement root = mDomDocument.documentElement();
    if(!readHeader(root, timeTrackingModel)) {
        return false;
    }

    QDomElement child = root.firstChildElement(mTagDay);
    while (!child.isNull()) {
        if(!readChild(child, timeTrackingModel)) {
            DEBUG("One XML child could not be read");
        }
        child = child.nextSiblingElement(mTagDay);
    }

    return true;
}

// =============================================================================
bool IOFileAccesses::readHeader(const QDomElement& root,
                                TimeTrackingModel* timeTrackingModel) {

    /* Read header:
     * <ETT
     *       version = "0.1"
     *       userName ="userName"
     *       year = "year yyyy"
     *       month="month From 1 to 12"
     *       prevMonthlyOverDueHours="previous Monthly Hours">
     **/

    if (root.tagName() != mTagMain) {
        DEBUG("Expected tag: " + mTagMain + ", read:" + root.tagName());
        QMessageBox::information(NULL, QObject::tr("Easy Time Tracker"),
                                 QObject::tr("The file is not a TimeTracker file."));
        return false;
    }

    if (root.hasAttribute(mTagVersion)
            && root.attribute(mTagVersion) != TT_VERSION) {
        QMessageBox::information(NULL, QObject::tr("Version problem"),
                                 QObject::tr("The file is not a Time Tracker version ") + TT_VERSION +
                                 QObject::tr(" file."));
        return false;
    }

    if (root.hasAttribute(mTagUserName)) {
        timeTrackingModel->setUserName(root.attribute(mTagUserName));
    }

    if (root.hasAttribute(mTagYear) && root.hasAttribute(mTagMonth)) {
        try {
            timeTrackingModel->setNewBaseDate(QDate(root.attribute(mTagYear).toInt(),
                                                    root.attribute(mTagMonth).toInt(),
                                                    1));

            if(mDebug) {
                DEBUG("Basedate: " + timeTrackingModel->getDefaultFileName() +
                      " mTagYear: " + root.attribute(mTagYear) + ", mTagMonth:" +
                      root.attribute(mTagMonth));
            }

        } catch (TTExceptionWrongBaseDate& e) {
            QMessageBox::information(NULL, QObject::tr("Date problem in file"),
                                     e.what());
            return false;

        }
    } else {
        QMessageBox::information(NULL, QObject::tr("Missing date"),
                                 QObject::tr("The file does not specify month and year."));
        return false;

    }

    // Do not update over due hours if not required
    if(timeTrackingModel != mTimeTrackingModel ||
       timeTrackingModel->getPrevMonthlyOverDueSeconds() <= 0) {
        if (root.hasAttribute(mTagPrevMonthlyOverDueSeconds)) {
            timeTrackingModel->setPrevMonthlyOverDueSeconds(root.attribute(mTagPrevMonthlyOverDueSeconds).toInt());
            DEBUG(QString("readHeader: %1 Set PrevMonthlyOverDueSeconds to %2, read = %3 = %4 seconds").arg(timeTrackingModel->getDefaultFileName()).arg(timeTrackingModel->getPrevMonthlyOverDueSecondsInHours()).arg(MiscDateTime::secondsToHours(root.attribute(mTagPrevMonthlyOverDueSeconds).toInt())).arg(root.attribute(mTagPrevMonthlyOverDueSeconds).toInt()));
        } else {
            timeTrackingModel->setPrevMonthlyOverDueSeconds(0);
        }
    }

    if (root.hasAttribute(mTagCompensatedSeconds)) {
        timeTrackingModel->setCompensatedOverDueSeconds(root.attribute(mTagCompensatedSeconds).toInt());
    }

    return true;

}

// =============================================================================
bool IOFileAccesses::readChild(const QDomElement& child,
                               TimeTrackingModel *timeTrackingModel) {

    int dayNumber = -1;

    if(child.hasAttribute(mTagDayNumber)) {
        dayNumber = child.attribute(mTagDayNumber).toInt();
        if(mDebug > DETAILED) {DEBUG(QString("Reading day: %1").arg(dayNumber));}

    } else {
        QMessageBox::information(NULL, QObject::tr("XML format error"),
                                 QObject::tr("Missing day number."));
        return false;
    }

    if(child.hasAttribute(mTagDayStarted)) {
        timeTrackingModel->setStartTime(dayNumber,
                                        MiscDateTime::stringToTime(child.attribute(mTagDayStarted)),
                                        true /* change start time only */);
        if(mDebug > DETAILED) {
            DEBUG(QString("Start time: %1, Extracted: %2").
                  arg(child.attribute(mTagDayStarted)).
                  arg((timeTrackingModel->getStartTime(dayNumber)).toString(("HH:mm"))));
        }
    }

    if (child.hasAttribute(mTagDayEnded)) {
        timeTrackingModel->setEndTime(dayNumber,
                                      MiscDateTime::stringToTime(child.attribute(mTagDayEnded)));
        if(mDebug > DETAILED) {
            DEBUG(QString("End time: %1, Extracted: %2").
                  arg(child.attribute(mTagDayEnded)).
                  arg((timeTrackingModel->getEndTime(dayNumber)).toString(("HH:mm"))));
        }
    }

    if (child.hasAttribute(mTagDayPause)) {
        timeTrackingModel->setPause(dayNumber,
                                    MiscDateTime::stringToTime(child.attribute(mTagDayPause)));

        if(mDebug > DETAILED) {
            DEBUG(QString("Pause time: %1, Extracted: %2").
                  arg(child.attribute(mTagDayPause)).
                  arg((timeTrackingModel->getPause(dayNumber)).toString(("HH:mm"))));
        }
    }

    if (child.hasAttribute(mTagDayRequiredWorkTime)) {
        timeTrackingModel->setRequiredWorkTime(dayNumber,
                                               MiscDateTime::stringToTime(child.attribute(mTagDayRequiredWorkTime)));
    }

    if (child.hasAttribute(mTagDayComment)) {
        timeTrackingModel->setComment(dayNumber,
                                      child.attribute(mTagDayComment));
    }

    return true;
}

// =============================================================================
// TODO: Refactor using this function
/*
boost::shared_ptr<TimeTrackingModel> IOFileAccesses::getTimeTrackingModelFromFilename(const QString& fileName) {
    QString curFileName = fileList[index];
    int curYear = curFileName.mid(curFileName.size() - IDX_FILENAME_SHIFT_YEAR, 4).toInt();
    int curMonth = curFileName.mid(curFileName.size() - IDX_FILENAME_SHIFT_MONTH, 2).toInt();
    QFile curFile(mTimeTrackingModel->createFileName(
                   mTimeTrackingModel->getUserName(), curYear, curMonth));
    boost::shared_ptr<TimeTrackingModel> curTimeTrackingModel(new TimeTrackingModel());
    if(!curFile.exists() || !openSheetInformation(&curFile, curTimeTrackingModel.get())) {
        return NULL;
    }
    return curTimeTrackingModel.get();

}
*/
// =============================================================================
// Saving a sheet means
//  1. saving all information from the first existing sheet to make sure
//any hand hacking of over hours does not break the algorithm
// =============================================================================
bool IOFileAccesses::saveSheetInformation() {
    QStringList fileList = getFileList(mTimeTrackingModel->getUserName());
    return saveSheetInformation(fileList);
}

// =============================================================================
bool IOFileAccesses::saveSheetInformation(const QStringList & fileList)
{

    // First save current sheet
    QString fileName = mTimeTrackingModel->getDefaultFileName();
    QFile file(fileName);

    if(mDebug) DEBUG("Trying to save the file: " + fileName);
    if (file.open(QIODevice::WriteOnly)) {
        saveSheetInformation(&file, mTimeTrackingModel);
        file.close();

    } else  {
        QMessageBox::warning(NULL, QObject::tr("Could not write to file"),
                             QObject::tr("File ") + fileName + QObject::tr(" could not be created."),
                             QMessageBox::Ok);
        return false;

    }

    // Now iterate over all sheets and reset getMonthlyOverDueSeconds
    for(int index = 1; index < fileList.size(); ++index) {

        QString curFileName = fileList[index];
        int curYear = curFileName.mid(curFileName.size() - IDX_FILENAME_SHIFT_YEAR, 4).toInt();
        int curMonth = curFileName.mid(curFileName.size() - IDX_FILENAME_SHIFT_MONTH, 2).toInt();
        QFile curFile(mTimeTrackingModel->createFileName(
                       mTimeTrackingModel->getUserName(), curYear, curMonth));
        boost::scoped_ptr<TimeTrackingModel> curTimeTrackingModel(new TimeTrackingModel());
        if(!curFile.exists() || !openSheetInformation(&curFile, curTimeTrackingModel.get())) {
            continue;
        }

        QString prevFileName = fileList[index - 1];
        int prevYear = prevFileName.mid(prevFileName.size() - IDX_FILENAME_SHIFT_YEAR, 4).toInt();
        int prevMonth = prevFileName.mid(prevFileName.size() - IDX_FILENAME_SHIFT_MONTH, 2).toInt();
        QFile prevFile(mTimeTrackingModel->createFileName(
                       mTimeTrackingModel->getUserName(), prevYear, prevMonth));
        boost::scoped_ptr<TimeTrackingModel> prevTimeTrackingModel(new TimeTrackingModel());
        if(!prevFile.exists() || !openSheetInformation(&prevFile, prevTimeTrackingModel.get())) {
            continue;
        }

        if(index == 1) {
            prevTimeTrackingModel->setPrevMonthlyOverDueSeconds(0);
        }
        int prevMonthlyOverDueSeconds = prevTimeTrackingModel->getMonthlyOverDueSeconds();
        curTimeTrackingModel->setPrevMonthlyOverDueSeconds(prevMonthlyOverDueSeconds);
        if(mDebug) DEBUG("Month " + curFileName + " over due hours:" + curTimeTrackingModel->getMonthlyOverDueSecondsInHours());
    }

    if(mDebug) DEBUG("File " + fileName + " created");
    return true;
}

// =============================================================================
bool IOFileAccesses::saveSheetInformation(QIODevice* file, TimeTrackingModel *timeTrackingModel)
{
    const int IndentSize = 4;

    if(mDebug) DEBUG("saveSheetInformation " + timeTrackingModel->getDefaultFileName());
    QTextStream out(file);
    QDomDocument outDom(mTagMain);
    QDomElement root = outDom.createElement(mTagMain);

    root.setAttribute(mTagVersion,                  TT_VERSION);
    root.setAttribute(mTagUserName,                 timeTrackingModel->getUserName());
    root.setAttribute(mTagYear,                     timeTrackingModel->getBaseDate().toString("yyyy"));
    root.setAttribute(mTagMonth,                    timeTrackingModel->getBaseDate().toString("MM"));
    root.setAttribute(mTagCompensatedSeconds,       QString::number(timeTrackingModel->getCompensatedOverDueSeconds()));
    root.setAttribute(mTagPrevMonthlyOverDueSeconds,  QString::number(timeTrackingModel->getPrevMonthlyOverDueSeconds()));
    DEBUG(QString("saveSheetInformation: Saved PrevMonthlyOverDueSeconds: %1 = %2 seconds").arg(timeTrackingModel->getPrevMonthlyOverDueSecondsInHours()).arg(timeTrackingModel->getPrevMonthlyOverDueSeconds()));
    outDom.appendChild( root );

    for(int day = 1; day <= timeTrackingModel->daysInMonth(); day++) {

        QDomElement element = outDom.createElement( mTagDay );
        bool oneElementCreated = false;

        element.setAttribute(mTagDayNumber, QString("%1").arg(day));

        if(timeTrackingModel->startTimeIsValid(day)) {
            element.setAttribute(mTagDayStarted, MiscDateTime::timeToString(timeTrackingModel->getStartTime(day)));
            oneElementCreated = true;
        }

        if(timeTrackingModel->endTimeIsValid(day)) {
            element.setAttribute(mTagDayEnded, MiscDateTime::timeToString(timeTrackingModel->getEndTime(day)));
            oneElementCreated = true;
        }

        if(timeTrackingModel->pauseIsValid(day)) {
            element.setAttribute(mTagDayPause, MiscDateTime::timeToString(timeTrackingModel->getPause(day)));
            oneElementCreated = true;
        }

        if(timeTrackingModel->requiredWorkTimeIsValid(day)) {
            element.setAttribute(mTagDayRequiredWorkTime, MiscDateTime::timeToString(timeTrackingModel->getRequiredWorkTime(day)));
            oneElementCreated = true;
        }

        if(!timeTrackingModel->getComment(day).isEmpty() || oneElementCreated) {
            element.setAttribute(mTagDayComment, timeTrackingModel->getComment(day));
            oneElementCreated = true;
        }

        if(oneElementCreated) {
            root.appendChild( element );
        }

    }

    outDom.save(out, IndentSize);
    return true;
}

// =============================================================================
bool IOFileAccesses::setPreviousBalance() {
    QStringList fileList = getFileList(mTimeTrackingModel->getUserName());
    return setPreviousBalance(fileList);
}

// =============================================================================
bool IOFileAccesses::setPreviousBalance(const QStringList & fileList) {

    boost::scoped_ptr<TimeTrackingModel> prevTimeTrackingModel(new TimeTrackingModel(mTimeTrackingModel->getBaseDate()));

    for(int index = 0; index < fileList.size(); ++index) {
        QString file = fileList[index];
        int year = file.mid(file.size() - IDX_FILENAME_SHIFT_YEAR, 4).toInt();
        int month = file.mid(file.size() - IDX_FILENAME_SHIFT_MONTH, 2).toInt();
        if(mTimeTrackingModel->getBaseDate().year() == year  &&
                mTimeTrackingModel->getBaseDate().month() == month &&
                index > 0) {
            QString prevFile = fileList[index - 1];
            year = prevFile.mid(prevFile.size() - IDX_FILENAME_SHIFT_YEAR, 4).toInt();
            month = prevFile.mid(prevFile.size() - IDX_FILENAME_SHIFT_MONTH, 2).toInt();

            DEBUG(QString("Comparing prevDate %1/%2 with %3/%4").arg(month).arg(year).arg(file.mid(file.size() - IDX_FILENAME_SHIFT_MONTH, 2)).arg(file.mid(file.size() - IDX_FILENAME_SHIFT_YEAR, 4)));

            QFile file(mTimeTrackingModel->createFileName(
                           mTimeTrackingModel->getUserName(), year, month));

            if(file.exists() && openSheetInformation(&file, prevTimeTrackingModel.get())) {
                mTimeTrackingModel->setPrevMonthlyOverDueSeconds(
                            prevTimeTrackingModel->getMonthlyOverDueSeconds());

                if(mDebug) DEBUG(QString("File %1 was opened,\n"
                                         " balance prev prev month from prev month : %2\n"
                                         "over due hours prev month: %3\n"
                                         "balance prev month: %4\n"
                                         "balance prev month from cur month : %5\n"
                                         "overdue hours cur month: %6\n"
                                         "balance cur month %7").arg(file.fileName()).
                                 arg(prevTimeTrackingModel->getPrevMonthlyOverDueSecondsInHours()).
                                 arg(prevTimeTrackingModel->getMonthOnlyOverDueSecondsInHours()).
                                 arg(prevTimeTrackingModel->getMonthlyOverDueSecondsInHours()).
                                 arg(mTimeTrackingModel->getPrevMonthlyOverDueSecondsInHours()).
                                 arg(mTimeTrackingModel->getMonthOnlyOverDueSecondsInHours()).
                                 arg(mTimeTrackingModel->getMonthlyOverDueSecondsInHours()));
                return true;
            }
        }
    }
    if(mDebug) DEBUG("No previous file found setting 0 for file " + mTimeTrackingModel->getDefaultFileName(Misc::getUserName()));

    mTimeTrackingModel->setPrevMonthlyOverDueSeconds(0);
    return false;
}
