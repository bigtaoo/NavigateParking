#ifndef PARKINGDATA_H
#define PARKINGDATA_H

#include <QSharedPointer>

class ParkingData
{
public:
    ParkingData();
    ~ParkingData();

    static ParkingData* GetIns();
    void InitializeData();

private:
    void saveData();
    void loadData();

private:
    static QSharedPointer<ParkingData> m_Ins;
};

#endif // PARKINGDATA_H
