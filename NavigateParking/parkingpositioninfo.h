#ifndef PARKINGPOSITIONINFO_H
#define PARKINGPOSITIONINFO_H

#include <QVector>
#include <QSharedPointer>

enum ParkingPositionDirection
{
    PPD_Horizontal,
    PPD_Vertical,
};

class ParkingPositionInfo
{
public:
    ParkingPositionInfo() = delete;
    explicit ParkingPositionInfo(int index, ParkingPositionDirection direction, int gridIndex, int length, int width);
    ~ParkingPositionInfo();

    inline int GetIndex() const { return m_ParkingIndex; }
    inline int GetGridIndex() const { return m_GridIndex; }
    inline ParkingPositionDirection GetDirection() const { return m_Direction; }
    inline bool IsUsed() const { return m_IsUsed; }
    inline void Use() { m_IsUsed = true; }
    inline void Unuse() { m_IsUsed = false; }

private:
    int m_ParkingIndex;
    ParkingPositionDirection m_Direction;
    int m_GridIndex;
    int m_Length;
    int m_Width;
    bool m_IsUsed;
};

class ParkingPositions
{
public:
    explicit ParkingPositions();
    ~ParkingPositions();

    static ParkingPositions* GetIns();
    void AddParkingPosition(int index, ParkingPositionDirection direction, int gridIndex, int length, int width);
    inline int GenerateIndex()
    {
        ++m_Index;
        return m_Index;
    }
    inline const QVector<QSharedPointer<ParkingPositionInfo>>& GetAllPositions() const
    {
        return m_AllParkingPositions;
    }

private:
    static QSharedPointer<ParkingPositions> m_Ins;
    QVector<QSharedPointer<ParkingPositionInfo>> m_AllParkingPositions;
    int m_Index;
};

#endif // PARKINGPOSITIONINFO_H
