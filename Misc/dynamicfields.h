#ifndef DYNAMICFIELDS_H
#define DYNAMICFIELDS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDate>

class DynamicField {
private:
    QString                 mFieldName;
    QMap<QDate, QString>    mMapDateSelection;

public:
    DynamicField(const QString & fieldName) : mFieldName(fieldName) {
    }
    virtual ~DynamicField() {}

    virtual bool isAMultipleValue   ()  const { return false;   }
    virtual bool isASingleValue     ()  const { return false;   }

    void setValueForDate(const QDate & date, const QString & value) {
       mMapDateSelection[date] = value;
    }
    const QString & getValueForDate(const QDate & date) {
       return mMapDateSelection[date];
    }
    void deleteValueForDate(const QDate & date) {
        mMapDateSelection.remove(date);
    }

    const QString & getFieldName() const {
        return mFieldName;
    }

};

class DynamicFieldInMultipleValues : public DynamicField {
    QStringList mStringListValues;
public:
    DynamicFieldInMultipleValues(const QString & fieldName)   : DynamicField(fieldName) {}
    virtual bool isAMultipleValue()   const {
        return true;
    }
    void setStringList(const QStringList & stringList) {
        assert(isAMultipleValue());
        mStringListValues = stringList;

    }
    const QStringList & getStringList() const {
        assert(isAMultipleValue());
        return mStringListValues;
    }


};
class DynamicFieldInSingleValue : public DynamicField {
public:
    DynamicFieldInSingleValue(const QString & fieldName)   : DynamicField(fieldName) {}
    virtual bool isASingleValue() const {
        return true;
    }

};
#endif // DYNAMICFIELDS_H
