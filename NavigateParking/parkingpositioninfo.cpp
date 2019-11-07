#include "parkingpositioninfo.h"
#include "parkmapgridinfo.h"

QSharedPointer<ParkingPositions> ParkingPositions::m_Ins;

ParkingPositionInfo::ParkingPositionInfo(int index, ParkingDirection direction, int gridIndex, int height, int width)
                     :m_ParkingIndex(index)
                     ,m_Direction(direction)
                     ,m_GridIndex(gridIndex)
                     ,m_Height(height)
                     ,m_Width(width)
                     ,m_IsUsed(false)
{

}

ParkingPositionInfo::~ParkingPositionInfo()
{

}

ParkingPositions::ParkingPositions() : m_Index(0)
{
    //m_AllParkingPositions.reserve(1000);
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

void ParkingPositions::AddParkingPosition(int index, ParkingDirection direction, int gridIndex, int height, int width)
{
    m_AllParkingPositions.push_back(QSharedPointer<ParkingPositionInfo>(
                                        new ParkingPositionInfo(index, direction, gridIndex, height, width)));

    int x = gridIndex % MAP_WIDTH;
    int y = gridIndex / MAP_WIDTH;
    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
        {
            int checkIndex = (x + j) + (y + i) * MAP_WIDTH;
            if(ParkMapGridInfo::GetIns()->GetParkingPositionIndex(checkIndex) != 0)
            {
                continue;
            }
            ParkMapGridInfo::GetIns()->SetParkingPositionIndex(checkIndex, index);
        }
    }
}

const ParkingPositionInfo* ParkingPositions::GetParkingPositionInfoByIndex(int index) const
{
    foreach (const QSharedPointer<ParkingPositionInfo>& iter, m_AllParkingPositions)
    {
        if(iter.get()->GetIndex() == index)
        {
            return iter.get();
        }
    }
    return nullptr;
}
