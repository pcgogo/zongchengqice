#ifndef PUMPDATA_H
#define PUMPDATA_H

#endif // PUMPDATA_H

#include <QCoreApplication>

class PumpData
{

public:
    void setID(QString ID);
    void setdate(QString date);
    QString other;

    PumpData();

    void SetID();
    void SetDate();
    void SetData1(float current1, float entrance1, float exit1, float flow1);
    void SetData2(float current2, float entrance2, float exit2, float flow2);
    void SetData3(float current3, float entrance3, float exit3, float flow3);
    void GetAllData();

private:
    QString ID;
    QString date;
    float current1;
    float entrance1;
    float exit1;
    float flow1;
    float current2;
    float entrance2;
    float exit2;
    float flow2;
    float current3;
    float entrance3;
    float exit3;
    float flow3;


};
