#include "parkmapgridinfo.h"
#include "parkingpositioninfo.h"
#include <QJsonObject>
#include <QJsonArray>

QSharedPointer<ParkMapGridInfo> ParkMapGridInfo::m_Ins;

ParkMapGridInfo* ParkMapGridInfo::GetIns()
{
    if (nullptr == m_Ins.get())
    {
        m_Ins.reset(new ParkMapGridInfo());
    }
    return m_Ins.get();
}

ParkMapGridInfo::ParkMapGridInfo()
{

}

ParkMapGridInfo::~ParkMapGridInfo()
{

}

void ParkMapGridInfo::Write(QJsonObject &json) const
{
    QJsonArray gridArray;
    QJsonArray gridToRoadArray;
    QJsonArray gridToParkingPositionArray;
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i)
    {
        gridArray.append(m_Map[i]);
        gridToRoadArray.append(m_GridToRoad[i]);
        gridToParkingPositionArray.append(m_GridToParkingPosition[i]);
    }
    json["grids"] = gridArray;
    json["gridToRoad"] = gridToRoadArray;
    json["gridToParkingPosition"] = gridToParkingPositionArray;
}

void ParkMapGridInfo::Read(const QJsonObject &json)
{
    QJsonArray gridsArray;
    QJsonArray gridToRoadArray;
    QJsonArray gridToParkingPositionArray;
    if (json.contains("grids") && json["grids"].isArray())
    {
        gridsArray = json["grids"].toArray();
    }
    if (json.contains("gridToRoad") && json["gridToRoad"].isArray())
    {
        gridToRoadArray = json["gridToRoad"].toArray();
    }
    if (json.contains("gridToParkingPosition") && json["gridToParkingPosition"].isArray())
    {
        gridToParkingPositionArray = json["gridToParkingPosition"].toArray();
    }
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i)
    {
        if(i < gridsArray.size())
        {
            m_Map[i] = static_cast<MapGrid>(gridsArray[i].toInt());
        }
        if(i < gridToRoadArray.size())
        {
            m_GridToRoad[i] = gridToRoadArray[i].toInt();
        }
        if(i < gridToParkingPositionArray.size())
        {
            m_GridToParkingPosition[i] = gridToParkingPositionArray[i].toInt();
        }
    }
}

void ParkMapGridInfo::Initialize()
{
    // default value
    for(int i = 0; i < MAP_WIDTH; ++i)
    {
        for(int j = 0; j < MAP_HEIGHT; ++j)
        {
            int index = MAP_WIDTH * j + i;
            m_GridToRoad[index] = 0;
            m_GridToParkingPosition[index] = 0;
            if(i == 0 || j == 0 || i == MAP_WIDTH - 1 || j == MAP_HEIGHT - 1)
            {
                m_Map[index] = MapGrid::MG_Wall;
                continue;
            }
            m_Map[index] = MapGrid::MG_None;
        }
    }

    buildLeftPart();
    buildRightPart();
    buildCenterHorizontal();
    buildTopAndBottom();
    buildVerticalRoad();
    buildParkingPosition();
}

void ParkMapGridInfo::buildLeftPart()
{
    // build road
    for(int i = 1; i < MAP_HEIGHT - 1; ++i)
    {
        // left main
        for(int j = 0; j < 3; ++j)
        {
            int index = i * MAP_WIDTH + 6 + j;
            m_Map[index] = MapGrid::MG_Road_Left;
        }
        for(int j = 0; j < 3; ++j)
        {
            int index = i * MAP_WIDTH + 9 + j;
            m_Map[index] = MapGrid::MG_Road_Right;
        }
    }
    // build park position
    for(int i = 1; i < MAP_HEIGHT - 1; ++i)
    {
        // left near wall
        for(int j = 0; j  < 5; ++j)
        {
            int index = i * MAP_WIDTH + 1 + j;
            m_Map[index] = MapGrid::MG_ParkPosition;
        }
        // left near road
        for(int j = 0; j < 10; ++j)
        {
            int index = i * MAP_WIDTH + 12 + j;
            m_Map[index] = MapGrid::MG_ParkPosition;
        }
    }
    // left part
    for(int iter = 0; iter < 29; ++iter)
    {
        for(int i = 1; i < MAP_HEIGHT - 1; ++i)
        {
            // road left
            for(int j = 0; j  < 3; ++j)
            {
                int index = i * MAP_WIDTH + (22 + iter * 16) + j;
                m_Map[index] = MapGrid::MG_Road_Left;
            }
            // road right
            for(int j = 0; j  < 3; ++j)
            {
                int index = i * MAP_WIDTH + (25 + iter * 16) + j;
                m_Map[index] = MapGrid::MG_Road_Right;
            }
            // park
            for(int j = 0; j < 10; ++j)
            {
                int index = i * MAP_WIDTH + (28 + iter * 16) + j;
                m_Map[index] = MapGrid::MG_ParkPosition;
            }
        }
    }
}

void ParkMapGridInfo::buildRightPart()
{
    // build road
    for(int i = 1; i < MAP_HEIGHT - 1; ++i)
    {
        // left main
        for(int j = 0; j < 3; ++j)
        {
            int index = i * MAP_WIDTH + (MAP_WIDTH - 9) - j;
            m_Map[index] = MapGrid::MG_Road_Left;
        }
        for(int j = 0; j < 3; ++j)
        {
            int index = i * MAP_WIDTH + (MAP_WIDTH - 6) - j;
            m_Map[index] = MapGrid::MG_Road_Right;
        }
    }
    // build park position
    for(int i = 1; i < MAP_HEIGHT - 1; ++i)
    {
        // left near wall
        for(int j = 0; j  < 5; ++j)
        {
            int index = i * MAP_WIDTH + (MAP_WIDTH - 1) - j;
            m_Map[index] = MapGrid::MG_ParkPosition;
        }
        // left near road
        for(int j = 0; j < 10; ++j)
        {
            int index = i * MAP_WIDTH + (MAP_WIDTH - 12) - j;
            m_Map[index] = MapGrid::MG_ParkPosition;
        }
    }
    // left part
    for(int iter = 0; iter < 29; ++iter)
    {
        for(int i = 1; i < MAP_HEIGHT - 1; ++i)
        {
            // road left
            for(int j = 0; j  < 3; ++j)
            {
                int index = i * MAP_WIDTH + (MAP_WIDTH - 25 - iter * 16) - j;
                m_Map[index] = MapGrid::MG_Road_Left;
            }
            // road right
            for(int j = 0; j  < 3; ++j)
            {
                int index = i * MAP_WIDTH + (MAP_WIDTH - 22 - iter * 16) - j;
                m_Map[index] = MapGrid::MG_Road_Right;
            }
            // park
            for(int j = 0; j < 10; ++j)
            {
                int index = i * MAP_WIDTH + (MAP_WIDTH - 28 - iter * 16) - j;
                m_Map[index] = MapGrid::MG_ParkPosition;
            }
        }
    }
}

void ParkMapGridInfo::buildCenterHorizontal()
{
    for(int i = 1; i < MAP_HEIGHT - 1; ++i)
    {
        // left
        for(int j = 0; j  < 5; ++j)
        {
            int index = i * MAP_WIDTH + 486 + j;
            m_Map[index] = MapGrid::MG_Road_Left;
        }
        // right
        for(int j = 0; j < 5; ++j)
        {
            int index = i * MAP_WIDTH + 490 + j;
            m_Map[index] = MapGrid::MG_Road_Right;
        }
        // park
        for(int j = 0; j < 12; ++j)
        {
            int index = i * MAP_WIDTH + 494 + j;
            m_Map[index] = MapGrid::MG_ParkPosition;
        }
        // left
        for(int j = 0; j  < 5; ++j)
        {
            int index = i * MAP_WIDTH + 506 + j;
            m_Map[index] = MapGrid::MG_Road_Left;
        }
        // right
        for(int j = 0; j < 5; ++j)
        {
            int index = i * MAP_WIDTH + 510 + j;
            m_Map[index] = MapGrid::MG_Road_Right;
        }
    }
}

void ParkMapGridInfo::buildTopAndBottom()
{
    // top
    for(int i = 12; i < MAP_WIDTH - 12; ++i)
    {
        for(int j = 0; j < 5; ++j)
        {
            int index = (j + 1) * MAP_WIDTH + i;
            m_Map[index] = MapGrid::MG_ParkPosition;
        }
    }
    // bottom
    for(int i = 12; i < MAP_WIDTH - 12; ++i)
    {
        for(int j = 0; j < 5; ++j)
        {
            int index = (MAP_HEIGHT - j - 2) * MAP_WIDTH + i;
            m_Map[index] = MapGrid::MG_ParkPosition;
        }
    }
}

void ParkMapGridInfo::buildVerticalRoad()

{
    // top
    for(int i = 12; i < MAP_WIDTH - 12; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            int index = (j + 6) * MAP_WIDTH + i;
            m_Map[index] = MapGrid::MG_Road_Left;
        }
        for(int j = 0; j < 3; ++j)
        {
            int index = (j + 9) * MAP_WIDTH + i;
            m_Map[index] = MapGrid::MG_Road_Right;
        }
    }
    // bottom
    for(int i = 12; i < MAP_WIDTH - 12; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            int index = (MAP_HEIGHT - 9 - j) * MAP_WIDTH + i;
            m_Map[index] = MapGrid::MG_Road_Left;
        }
        for(int j = 0; j < 3; ++j)
        {
            int index = (MAP_HEIGHT - 6 - j) * MAP_WIDTH + i;
            m_Map[index] = MapGrid::MG_Road_Right;
        }
    }
    // other road
    for(int iter = 1; iter < 3; ++iter)
    {
        for(int i = 12; i < MAP_WIDTH - 12; ++i)
        {
            for(int j = 0; j < 3; ++j)
            {
                int index = (iter * 162 + j) * MAP_WIDTH + i;
                m_Map[index] = MapGrid::MG_Road_Left;
            }
            for(int j = 0; j < 3; ++j)
            {
                int index = (iter * 162 + 3 + j) * MAP_WIDTH + i;
                m_Map[index] = MapGrid::MG_Road_Right;
            }
            for(int j = 0; j < 3; ++j)
            {
                int index = (MAP_HEIGHT - iter * 162 - 3 - j) * MAP_WIDTH + i;
                m_Map[index] = MapGrid::MG_Road_Left;
            }
            for(int j = 0; j < 3; ++j)
            {
                int index = (MAP_HEIGHT - iter * 162 - j) * MAP_WIDTH + i;
                m_Map[index] = MapGrid::MG_Road_Right;
            }
        }
    }
    // center
    for(int i = 12; i < MAP_WIDTH - 12; ++i)
    {
        for(int j = 0; j < 5; ++j)
        {
            int index = (MAP_WIDTH / 2 - 5 + j) * MAP_WIDTH + i;
            m_Map[index] = MapGrid::MG_Road_Left;
        }
        for(int j = 0; j < 5; ++j)
        {
            int index = (MAP_WIDTH / 2 + j) * MAP_WIDTH + i;
            m_Map[index] = MapGrid::MG_Road_Right;
        }
    }
}

void ParkMapGridInfo::buildParkingPosition()
{
    // near wall parking position
    for(int i = 1; i < MAP_HEIGHT - 1;)
    {
        if(i == 1 || i == 995)
        {
            ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                           ,ParkingDirection::PD_Horizontal
                                                           ,i * MAP_WIDTH + 1,4,5);
            i += 4;
        }
        else
        {
            ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                           ,ParkingDirection::PD_Horizontal
                                                           ,i * MAP_WIDTH + 1,3,5);
            i += 3;
        }
    }
    for(int i = 12; i < MAP_WIDTH - 12;)
    {
        if(i == 12)
        {
            ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                           ,ParkingDirection::PD_Vertical
                                                           ,i + MAP_WIDTH,5,4);
            i += 4;
        }
        else
        {
            ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                           ,ParkingDirection::PD_Vertical
                                                           ,i + MAP_WIDTH,5,3);
            i += 3;
        }
    }
    for(int i = 1; i < MAP_HEIGHT - 1;)
    {
        if(i == 1 || i == 995)
        {
            ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                           ,ParkingDirection::PD_Horizontal
                                                           ,i * MAP_WIDTH + MAP_WIDTH - 5,4,5);
            i += 4;
        }
        else
        {
            ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                           ,ParkingDirection::PD_Horizontal
                                                           ,i * MAP_WIDTH + MAP_WIDTH - 5,3,5);
            i += 3;
        }
    }
    for(int i = 12; i < MAP_WIDTH - 12;)
    {
        if(i == 12)
        {
            ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                           ,ParkingDirection::PD_Vertical
                                                           ,i + MAP_WIDTH * (MAP_HEIGHT - 5),5,4);
            i += 4;
        }
        else
        {
            ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                           ,ParkingDirection::PD_Vertical
                                                           ,i + MAP_WIDTH * (MAP_HEIGHT - 5),5,3);
            i += 3;
        }
    }

    // middle
    int iCount = 0;
    for(int i = 12; i < MAP_WIDTH - 12; ++i)
    {
        for(int j = 12; j < MAP_HEIGHT - 12; ++j)
        {
            int index = j * MAP_WIDTH + i;
            if(m_Map[index] != MapGrid::MG_ParkPosition)
            {
                continue;
            }
            if(m_Map[(j + 2) * MAP_WIDTH + i] == MapGrid::MG_ParkPosition)
            {
                if(i == 494 || i == 500)
                {
                    ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                               ,ParkingDirection::PD_Horizontal
                                                               ,index,3,6);
                }
                else
                {
                    ParkingPositions::GetIns()->AddParkingPosition(ParkingPositions::GetIns()->GenerateIndex()
                                                               ,ParkingDirection::PD_Horizontal
                                                               ,index,3,5);
                }
            }
            j += 2;
        }
        ++iCount;
        if(iCount % 2 == 0)
        {
            i += 10;
        }
        else
        {
            i += 4;
        }
        // center
        if(i == 496)
        {
            i = 493;
        }
        static bool hasCheck504 = false;
        if(i == 504 && !hasCheck504)
        {
            hasCheck504 = true;
            i = 499;
        }
        if(i == 504 && hasCheck504)
        {
            i = 514;
            iCount = 0;
        }
    }
}
