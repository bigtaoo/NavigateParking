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

ParkingRoadInfo::ParkingRoadInfo(int index, ParkingDirection direction) :
    m_Index(index)
  , m_Direction(direction)
{

}

ParkingRoadInfo::~ParkingRoadInfo()
{

}

int ParkingRoadInfo::GetStartGrid()
{
    if(m_Grids.empty())
    {
        return 0;
    }
    return m_Grids.first();
}

int ParkingRoadInfo::GetLastGrid()
{
    if(m_Grids.empty())
    {
        return 0;
    }
    return m_Grids.last();
}

void ParkingRoadInfo::AddGrid(int grid)
{
    m_Grids.push_back(grid);

    ParkMapGridInfo::GetIns()->SetRoadIndex(grid, m_Index);
}

ParkingRoads::ParkingRoads() : m_IndexGenerater(0)
{
    buildVerticalRoad();
    buildHorizontalRoad();
    //buildRoadIndex(6, 1, 6);

    buildLikns();
}

ParkingRoads::~ParkingRoads()
{
    m_kAllRoads.clear();
}

bool ParkingRoads::isRoad(int x, int y)
{
    if(ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_Road_Left
            || ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_Road_Right)
    {
        return true;
    }
    return false;
}

bool ParkingRoads::isParkingPosition(int x, int y)
{
    if(ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_ParkPosition)
    {
        return true;
    }
    return false;
}

void ParkingRoads::buildRoadIndex(int x, int y, int width)
{
    if(y > MAP_HEIGHT || x > MAP_WIDTH)
    {
        return;
    }
    ParkingRoadInfo* road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Horizontal);
    m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));

    bool IsHorizontal = true;
    if(ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x - 1) == MapGrid::MG_ParkPosition)
    {
        IsHorizontal = false;
    }
    if(IsHorizontal)
    {
        //MapGrid checkGrid = ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * (y + width + 1) + x);
        bool checkGrid = isRoad(x, y + width + 1);
        int checkX = x;
        bool tempGrid = isRoad(x + 1, y + 1 + width);
        while(x < MAP_WIDTH && tempGrid == checkGrid)
        {
            for(int i = 0; i < width; ++i)
            {
                road->AddGrid((y + i) * MAP_WIDTH + x);
            }
            ++x;
            tempGrid = isRoad(x + width + 1, y + 1);
        }
        if(!checkGrid)
        {
            // right direction
            buildRoadIndex(x + 1, y, width);
        }
        if(isRoad(x, y + width + 1))
        {
            // bottom direction
            buildRoadIndex(x, y + width + 1, x - checkX + 1);
        }
    }
    else
    {
        //MapGrid checkGrid = ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x + width + 1);
        bool checkGrid = isRoad(x + width + 1, y);
        int checkY = y;
        //MapGrid tempGrid = ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * (y + 1) + x + width + 1);
        bool tempGrid = isRoad(x + width + 1, y + 1);
        while(y < MAP_HEIGHT && tempGrid == checkGrid)
        {
            for(int i = 0; i < width; ++i)
            {
                road->AddGrid(y * MAP_WIDTH + x + i);
            }
            ++y;
            //tempGrid = ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * (y + 1) + x + width + 1);
            tempGrid = isRoad(x + width + 1, y + 1);
        }
        if(!checkGrid)
        {
            // bottom direction
            buildRoadIndex(x, y + 1, width);
        }
        if(isRoad(x, y + 1))
        {
            // right direction
            buildRoadIndex(x + width + 1, checkY, y - checkY + 1);
        }
    }
}

void ParkingRoads::buildVerticalRoad()
{
    bool findRoad = false;
    ParkingRoadInfo* road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Vertical);
    m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
    // left
    for(int k = 1; k < MAP_HEIGHT - 1; ++k)
    {
        if(isRoad(13, k) == findRoad)
        {
            for(int i = 6; i < 12; ++i)
            {
                int index = k * MAP_WIDTH + i;
                road->AddGrid(index);
            }
        }
        else
        {
            findRoad = !findRoad;
            road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Vertical);
            m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
            --k;
        }
    }
    // middle
    for(int m = 13; m < MAP_WIDTH - 13; ++m)
    {
        // check road width
        int width = 1;
        if(isRoad(m, 13))
        {
            while (isRoad(m + width, 13))
            {
                ++width;
            }

            if(width > 0)
            {
                road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Vertical);
                m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));

                bool newRoad = false;
                for(int k = 8; k < MAP_HEIGHT - 11; ++k)
                {
                    if(!isRoad(m - 1, k))
                    {
                        newRoad = true;
                        for(int i = 0; i < width; ++i)
                        {
                            int index = k * MAP_WIDTH + m + i;
                            road->AddGrid(index);
                        }
                    }
                    else
                    {
                        if(newRoad)
                        {
                            newRoad = false;
                            road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Vertical);
                            m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
                        }
                    }
                }
            }
            m += width;
        }
    }

    // right
    findRoad = false;
    for(int k = 1; k < MAP_HEIGHT - 1; ++k)
    {
        if(isRoad(MAP_WIDTH - 19, k) == findRoad)
        {
            for(int i = 6; i < 12; ++i)
            {
                int index = k * MAP_WIDTH + i + MAP_WIDTH - 17;
                road->AddGrid(index);
            }
        }
        else
        {
            findRoad = !findRoad;
            road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Vertical);
            m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
            --k;
        }
    }
}

void ParkingRoads::buildHorizontalRoad()
{
    ParkingRoadInfo* road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Horizontal);
    m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
    bool findRoad = false;

    // top
    for(int k = 12; k < MAP_WIDTH - 12; ++k)
    {
        if(isRoad(k, 13) == findRoad)
        {
            for(int i = 6; i < 12; ++i)
            {
                int index = i * MAP_WIDTH + k;
                road->AddGrid(index);
            }
        }
        else
        {
            findRoad = !findRoad;
            road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Horizontal);
            m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
            --k;
        }
    }
    // middle
    for(int i = 20; i < MAP_HEIGHT - 20; ++i)
    {
        if(isRoad(13, i))
        {
            // road width
            int width = 1;
            while (isRoad(13, i + width))
            {
                ++width;
            }

            findRoad = false;
            for(int j = 12; j < MAP_WIDTH - 13; ++j)
            {
                if(isRoad(j, i - 1) == findRoad)
                {
                    for(int m = 0; m < width; ++m)
                    {
                        int index = (i + m) * MAP_WIDTH + j;
                        road->AddGrid(index);
                    }
                }
                else
                {
                    findRoad = !findRoad;
                    road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Horizontal);
                    m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
                    --j;
                }
            }
            i += width;
        }
    }
    // bottom
    findRoad = false;
    for(int k = 12; k < MAP_WIDTH - 12; ++k)
    {
        if(isRoad(k, MAP_HEIGHT - 13) == findRoad)
        {
            for(int i = 10; i > 4; --i)
            {
                int index = (MAP_HEIGHT - i) * MAP_WIDTH + k;
                road->AddGrid(index);
            }
        }
        else
        {
            findRoad = !findRoad;
            road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Horizontal);
            m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
            --k;
        }
    }

    //ParkMapGridInfo().GetIns()->SetOffsetX(-900 * GRID_SIZE);
    //ParkMapGridInfo().GetIns()->SetOffsetY(-900 * GRID_SIZE);
}

void ParkingRoads::buildLikns()
{
    foreach (const QSharedPointer<ParkingRoadInfo>& road, m_kAllRoads)
    {
        // find the road around this road
        const QVector<int>& grids = road.get()->GetGrids();
        foreach(const int& grid, grids)
        {
            int x = grid % MAP_WIDTH;
            int y = grid / MAP_WIDTH;
            if(isRoad(x - 1, y))
            {
                addUniqueLink(road, x - 1, y);
            }
            if(isRoad(x + 1, y))
            {
                addUniqueLink(road, x + 1, y);
            }
            if(isRoad(x, y - 1))
            {
                addUniqueLink(road, x, y - 1);
            }
            if(isRoad(x, y + 1))
            {
                addUniqueLink(road, x, y + 1);
            }
        }
    }
}

void ParkingRoads::addUniqueLink(const QSharedPointer<ParkingRoadInfo>& road, int x, int y)
{
    int roadIndex = getRoadIndexByPosition(x, y);
    if(roadIndex == 0 || roadIndex == road.get()->GetIndex())
    {
        return;
    }
    if(!road.get()->GetLinkRoads().contains(roadIndex))
    {
        road.get()->AddLinkRoad(roadIndex);
    }
}

int ParkingRoads::getRoadIndexByPosition(int x, int y)
{
    int index = y * MAP_WIDTH + x;
    return ParkMapGridInfo::GetIns()->GetRoadIndex(index);
}
