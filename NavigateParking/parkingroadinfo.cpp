#include "parkingroadinfo.h"
#include "parkmapgridinfo.h"

QSharedPointer<ParkingRoads> ParkingRoads::m_Ins;

ParkingRoads* ParkingRoads::GetIns()
{
    if (nullptr == m_Ins.get())
    {
        m_Ins.reset(new ParkingRoads());
    }
    return m_Ins.get();
}

ParkingRoadInfo::ParkingRoadInfo(int index) : m_Index(index)
{

}

ParkingRoadInfo::~ParkingRoadInfo()
{

}

ParkingRoads::ParkingRoads() : m_IndexGenerater(0)
{
    buildLeftRoad();
    buildRoadIndex(6, 1);
}

ParkingRoads::~ParkingRoads()
{
    m_kAllRoads.clear();
}

void ParkingRoads::buildRoadIndex(int x, int y)
{
    bool IsHorizontal = true;
    if(ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x - 1) == MapGrid::MG_ParkPosition)
    {
        IsHorizontal = false;
    }
    if(IsHorizontal)
    {

    }
    else
    {
        //int roadWidth = 0;
        //while(ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_ParkPosition)
    }
}

void ParkingRoads::buildLeftRoad()
{
    {
        ParkingRoadInfo* road = new ParkingRoadInfo(generateIndex());
        for(int i = 1; i < 4; ++i)
        {
            road->AddParkingPosition(i);
        }
        for(int i = 6; i < 12; ++i)
        {
            for(int j = 1; j < 12; ++j)
            {
                int index = j * MAP_WIDTH + i;
                road->AddGrid(index);
            }
        }
        m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
    }
}
