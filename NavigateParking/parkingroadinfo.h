#ifndef PARKINGROADINFO_H
#define PARKINGROADINFO_H

#include <QSharedPointer>
#include <QVector>
#include "direction.h"

class ParkingRoadInfo
{
public:
    ParkingRoadInfo() = delete;
    ParkingRoadInfo(int index, ParkingDirection direction);
    ~ParkingRoadInfo();

    inline void AddGrid(int grid) { m_Grids.push_back(grid); }
    inline void AddParkingPosition(int pos) { m_ParkingPositions.push_back(pos); }
    inline void AddLinkRoad(int index) { m_ParkingPositions.push_back(index); }
    inline int GetIndex() { return m_Index; }

    int GetStartGrid();

private:
    int m_Index;
    ParkingDirection m_Direction;
    QVector<int> m_Grids;
    QVector<int> m_ParkingPositions;
    QVector<int> m_LinkRoads;
};

class ParkingRoads
{
public:
    explicit ParkingRoads();
    ~ParkingRoads();

    static ParkingRoads* GetIns();
    inline const QVector<QSharedPointer<ParkingRoadInfo>>& GetRoads() const
    {
        return m_kAllRoads;
    }

private:
    void buildVerticalRoad();
    void buildHorizontalRoad();
    void buildRoadIndex(int x, int y, int width);
    bool isRoad(int x, int y);
    bool isParkingPosition(int x, int y);
    inline int generateIndex() { return ++m_IndexGenerater; }

private:
    static QSharedPointer<ParkingRoads> m_Ins;
    QVector<QSharedPointer<ParkingRoadInfo>> m_kAllRoads;
    int m_IndexGenerater;
};

#endif // PARKINGROADINFO_H
