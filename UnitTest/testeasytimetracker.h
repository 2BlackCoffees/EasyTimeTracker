#ifndef TESTEASYTIMETRACKER_H
#define TESTEASYTIMETRACKER_H

#include <QtTest/QtTest>
#include "mainwindow.h"

class TestEasyTimeTracker: public QObject
{
    Q_OBJECT
private slots:
    /* ================================ */
    /*            TestMisc              */
    void test_stringToTime_data();
    void test_stringToTime();

    void test_stringHoursToSeconds_data();
    void test_stringHoursToSeconds();
    /*
    void test_stringToDate();
    void test_dateToString();
    void test_dateTimeToString();
    void test_stringToDateTime();
    void test_dateTimeToSeconds();
    void test_timeToSeconds();
    void test_secondsToDateTime();
    void test_dateTimeToHours();
    void test_secondsToHours();
    void test_getInitDate();
    void test_getInitTime();
    */

};

#endif // TESTEASYTIMETRACKER_H
