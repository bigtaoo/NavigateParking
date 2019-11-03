#include "parkingpositioninfo.h"

QSharedPointer<ParkingPositions> ParkingPositions::m_Ins;

ParkingPositionInfo::ParkingPositionInfo(int index, ParkingPositionDirection direction, int gridIndex, int length, int width)
                     :m_ParkingIndex(index)
                     ,m_Direction(direction)
                     ,m_GridIndex(gridIndex)
                     ,m_Length(length)
                     ,m_Width(width)
                     ,m_IsUsed(false)
{

}

ParkingPositionInfo::~ParkingPositionInfo()
{

}

ParkingPositions::ParkingPositions() : m_Index(0)
{
    m_AllParkingPositions.reserve(1000);
}

ParkingPositions::~ParkingPositions()
{
    m_AllParkingPositions.clear();
}

ParkingPositions* ParkingPositions::GetIns()
{
    if (nullptr == m_Ins.get())
    {
        m_Ins.reset(new ParkingPositions());
    }
    return m_Ins.get();
}

void ParkingPositions::AddParkingPosition(int index, ParkingPositionDirection direction, int gridIndex, int length, int width)
{
    m_AllParkingPositions.push_back(QSharedPointer<ParkingPositionInfo>(
                                        new ParkingPositionInfo(index, direction, gridIndex, length, width)));
}
