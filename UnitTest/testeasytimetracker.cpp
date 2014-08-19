#include "testeasytimetracker.h"
#include <iostream>

void TestEasyTimeTracker::test_stringToTime_data() {
    QTest::addColumn<QTime>("time");
    QTest::addColumn<QString>("string");

    QTest::newRow("hour1") << QTime(2,  0) << "2";
    QTest::newRow("hour2") << QTime(10, 0) << "10";
    QTest::newRow("hour3") << QTime(14, 0) << "14";
    QTest::newRow("hour4") << QTime(18, 0) << "18";
    QTest::newRow("hourMin1") << QTime(2,   0) << "2:0";
    QTest::newRow("hourMin2") << QTime(8,   3) << "8:3";
    QTest::newRow("hourMin3") << QTime(14,  3) << "14:03";
    QTest::newRow("hourMin4") << QTime(18, 41) << "18:41";
    QTest::newRow("hourMinSec1") << QTime(2,   0,  1) << "2:0:1";
    QTest::newRow("hourMinSec2") << QTime(8,   3, 10) << "8:3:10";
    QTest::newRow("hourMinSec3") << QTime(14,  3, 45) << "14:03:45";
    QTest::newRow("hourMinSec4") << QTime(18, 41,  6) << "18:41:06";
}

void TestEasyTimeTracker::test_stringToTime() {

    QFETCH(QTime, time);
    QFETCH(QString, string);

    QCOMPARE(time, MiscDateTime::stringToTime(string));
}

void TestEasyTimeTracker::test_stringHoursToSeconds_data() {
    QTest::addColumn<int>("seconds");
    QTest::addColumn<QString>("hours");

    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    for(int index = 0; index < qrand() % 10 + 5; index++) {
        bool sign = qrand() % 10 > 3;
        int hour = qrand() % 24;
        int minute = qrand() % 60;
        int second = qrand() % 60;
        int seconds = hour * 3600 + minute * 60 + second;
        QString strTime = "";
        if(!sign) {
            seconds = -seconds;
            strTime += "-";
        }
        strTime += QString("%1:%2:%3").
                arg(hour,   qrand() % 2 + 1, 10, (QChar)'0').
                arg(minute, qrand() % 2 + 1, 10, (QChar)'0').
                arg(second, qrand() % 2 + 1, 10, (QChar)'0');

        QTest::newRow(QString("hourToSecs%1").arg(index).toStdString().c_str())
                << seconds << strTime;
    }
}

void TestEasyTimeTracker::test_stringHoursToSeconds() {
    QFETCH(int, seconds);
    QFETCH(QString, hours);

    QCOMPARE(seconds, MiscDateTime::stringHoursToSeconds(hours));

}
