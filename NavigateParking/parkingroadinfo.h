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

    inline void AddGrid(int grid);
    inline void AddParkingPosition(int pos) { m_ParkingPositions.push_back(pos); }
    inline void AddLinkRoad(int index) { m_LinkRoads.push_back(index); }
    inline int GetIndex() { return m_Index; }
    inline const QVector<int>& GetGrids() const { return m_Grids; }
    inline const QVector<int>& GetLinkRoads() const { return m_LinkRoads; }
    inline const QVector<int>& GetParkingPositions() const { return m_ParkingPositions; }
    inline ParkingDirection GetDirection() const { return m_Direction; }

    int GetStartGrid() const;
    int GetLastGrid() const;
    void Write(QJsonObject &json) const;
    void Read(const QJsonObject &json);

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
    void Initialize();
    inline const QVector<QSharedPointer<ParkingRoadInfo>>& GetRoads() const
    {
        return m_kAllRoads;
    }
    const ParkingRoadInfo* GetRoadByIndex(int index) const;
    void Write(QJsonObject &json) const;
    void Read(const QJsonObject &json);

private:
    void buildVerticalRoad();
    void buildHorizontalRoad();
    void buildRoadIndex(int x, int y, int width);
    void buildLiknRoads();
    void buildParkingPositions();
    void addUniqueLinkRoad(const QSharedPointer<ParkingRoadInfo>& road, int x, int y);
    void addUniqueParkingPosition(const QSharedPointer<ParkingRoadInfo>& road, int x, int y);
    bool isRoad(int x, int y);
    bool isParkingPosition(int x, int y);
    inline int generateIndex() { return ++m_IndexGenerater; }

private:
    static QSharedPointer<ParkingRoads> m_Ins;
    QVector<QSharedPointer<ParkingRoadInfo>> m_kAllRoads;
    int m_IndexGenerater;
};

#endif // PARKINGROADINFO_H
