#ifndef PARKINGPOSITIONINFO_H
#define PARKINGPOSITIONINFO_H

#include <QVector>
#include <QSharedPointer>
#include "direction.h"

class ParkingPositionInfo
{
public:
    ParkingPositionInfo() = delete;
    explicit ParkingPositionInfo(int index, ParkingDirection direction, int gridIndex, int height, int width);
    ~ParkingPositionInfo();

    inline int GetIndex() const { return m_ParkingIndex; }
    inline int GetGridIndex() const { return m_GridIndex; }
    inline ParkingDirection GetDirection() const { return m_Direction; }
    inline bool IsUsed() const { return m_IsUsed; }
    inline void Use() { m_IsUsed = true; }
    inline void Unuse() { m_IsUsed = false; }
    inline int GetHeight() const { return m_Height; }
    inline int GetWidth() const { return m_Width; }

private:
    int m_ParkingIndex;
    ParkingDirection m_Direction;
    int m_GridIndex;
    int m_Height;
    int m_Width;
    bool m_IsUsed;
};

class ParkingPositions
{
public:
    explicit ParkingPositions();
    ~ParkingPositions();

    static ParkingPositions* GetIns();
    void AddParkingPosition(int index, ParkingDirection direction, int gridIndex, int height, int width);
    inline int GenerateIndex()
    {
        ++m_Index;
        return m_Index;
    }
    inline const QVector<QSharedPointer<ParkingPositionInfo>>& GetAllPositions() const
    {
        return m_AllParkingPositions;
    }
    const ParkingPositionInfo* GetParkingPositionInfoByIndex(int index) const;

private:
    static QSharedPointer<ParkingPositions> m_Ins;
    QVector<QSharedPointer<ParkingPositionInfo>> m_AllParkingPositions;
    int m_Index;
};

#endif // PARKINGPOSITIONINFO_H
