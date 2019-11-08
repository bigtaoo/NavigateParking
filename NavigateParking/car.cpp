#include "car.h"
#include "parkingroadinfo.h"
#include "parkmapgridinfo.h"
#include "parkingpositioninfo.h"
#include <QRandomGenerator>

QSharedPointer<Car> Car::m_Ins;

Car* Car::GetIns()
{
    if (nullptr == m_Ins.get())
    {
        m_Ins.reset(new Car());
    }
    return m_Ins.get();
}

Car::Car()
{
    m_Path.reserve(1000);
}

Car::~Car()
{
    m_Path.clear();
}

void Car::InitPosition(int pos)
{
    m_StartPos = pos;

    findPath();
}

void Car::RandomPosition()
{
    const QVector<QSharedPointer<ParkingRoadInfo>>& roads =  ParkingRoads::GetIns()->GetRoads();
    int roadInex = QRandomGenerator::global()->bounded(0, roads.size());
    const ParkingRoadInfo* road = roads[roadInex].get();

    const QVector<int>& grids = road->GetGrids();
    int randomIndex = QRandomGenerator::global()->bounded(0, grids.size());
    m_StartPos = grids[randomIndex];

    findPath();

    setCamera();
}

void Car::setCamera()
{
    // move camera
    int x = m_StartPos % MAP_WIDTH;
    int y = m_StartPos / MAP_WIDTH;
    ParkMapGridInfo::GetIns()->SetOffsetX(-x * GRID_SIZE + ParkMapGridInfo::GetIns()->GetMapWidth() / 2);
    ParkMapGridInfo::GetIns()->SetOffsetY(-y * GRID_SIZE + ParkMapGridInfo::GetIns()->GetMapHeight() / 2);
}

void Car::Update()
{
    ++m_TimeCount;
    if(m_TimeCount == 6)
    {
        const ParkingPositionInfo* parkingPosition = ParkingPositions::GetIns()->GetParkingPositionInfoByIndex(m_TargetPos);
        if(parkingPosition != nullptr)
        {
            parkingPosition->Use();
        }
    }
    if(m_TimeCount == 8)
    {
        m_TimeCount = 0;
        RandomPosition();
    }
}

void Car::findPath()
{
    // currently road
    int roadIndex = ParkMapGridInfo::GetIns()->GetRoadIndex(m_StartPos);
    QVector<int> emptyParkingPosition;
    QVector<int> nextList;
    QVector<int> openList;
    QVector<int> closeList;
    nextList.push_back(roadIndex);
    // find the empty parking position in nearly road
    while (emptyParkingPosition.empty())
    {
        openList = nextList;
        nextList.clear();
        foreach (int checkRoadIndex, openList)
        {
            const ParkingRoadInfo* road = ParkingRoads::GetIns()->GetRoadByIndex(checkRoadIndex);
            foreach(int parkingPosition, road->GetParkingPositions())
            {
                const ParkingPositionInfo* parkingPositionInfo = ParkingPositions::GetIns()->GetParkingPositionInfoByIndex(parkingPosition);
                if(!parkingPositionInfo->IsUsed())
                {
                    emptyParkingPosition.push_back(parkingPosition);
                }
            }
            foreach(int newRoad, road->GetLinkRoads())
            {
                if(!closeList.contains(newRoad))
                {
                    nextList.push_back(newRoad);
                }
            }
            closeList.push_back(checkRoadIndex);
        }
    }
    // find the nearest parking position
    QVector<int> path;
    int lastLength = MAP_WIDTH * MAP_HEIGHT;
    foreach(int target, emptyParkingPosition)
    {
        path.clear();
        int startX = m_StartPos % MAP_WIDTH;
        int startY = m_StartPos / MAP_WIDTH;
        const ParkingPositionInfo* targetParkingPosition = ParkingPositions::GetIns()->GetParkingPositionInfoByIndex(target);
        int endX = targetParkingPosition->GetGridIndex() % MAP_WIDTH;
        int endY = targetParkingPosition->GetGridIndex() / MAP_WIDTH;

        path.push_back(m_StartPos);
        // move the target position to road
        for(int i = 0; i < 10; ++i)
        {
            if(targetParkingPosition->GetDirection() == ParkingDirection::PD_Horizontal)
            {
                if(isRoad(endX - i, endY))
                {
                    endX -= i;
                    break;
                }
                if(isRoad(endX + i, endY))
                {
                    endX += i;
                    break;
                }
            }
            else
            {
                if(isRoad(endX, endY - i))
                {
                    endY -= i;
                    break;
                }
                if(isRoad(endX, endY + i))
                {
                    endY += i;
                    break;
                }
            }
        }
        int targetRoadIndex = ParkMapGridInfo::GetIns()->GetRoadIndex(endX + endY * MAP_WIDTH);

        // find path for the target position
        bool yUpward = false;
        if(endY < startY)
        {
            yUpward = true;
        }
        while(startX != endX || startY != endY)
        {
            bool moveX = false;
            if(endX > startX)
            {
                if(isRoad(startX + 1, startY))
                {
                    ++startX;
                    moveX = true;
                }
            }
            else if(endX < startX)
            {
                if(isRoad(startX - 1, startY))
                {
                    --startX;
                    moveX = true;
                }
            }
            if(!moveX)
            {
                // if the target position and the start position in the same road
                if(roadIndex == targetRoadIndex)
                {
                    if(yUpward)
                    {
                        --startY;
                    }
                    else
                    {
                        ++startY;
                    }
                }
                else
                {
                    // find the road cross point
                    QVector<int> crossGrids;
                    int currentCheckRoadIndex = roadIndex;
                    int chooseGrid = 0;
                    bool find = false;
                    while (!find)
                    {
                        // each once choose the nearest cross point
                        const ParkingRoadInfo* checkCrossRoadInfo = ParkingRoads::GetIns()->GetRoadByIndex(currentCheckRoadIndex);
                        // reach the target road
                        if(checkCrossRoadInfo->GetLinkRoads().contains(targetRoadIndex))
                        {
                            crossGrids.push_back(checkCrossRoadInfo->GetStartGrid());
                            find = true;
                            break;
                        }
                        int checkDistance = MAP_WIDTH * MAP_WIDTH + MAP_HEIGHT * MAP_HEIGHT;
                        foreach(int checkRoadIndex,checkCrossRoadInfo->GetLinkRoads())
                        {
                            const ParkingRoadInfo* tempRoadInfo = ParkingRoads::GetIns()->GetRoadByIndex(checkRoadIndex);
                            // reach the target road
                            if(tempRoadInfo->GetLinkRoads().contains(targetRoadIndex))
                            {
                                crossGrids.push_back(tempRoadInfo->GetStartGrid());
                                find = true;
                                break;
                            }
//                            // not the cross road
//                            if(!tempRoadInfo->GetParkingPositions().empty())
//                            {
//                                continue;
//                            }
                            // find the nearest cross road
                            int tempIndex = tempRoadInfo->GetStartGrid();
                            int tempX = tempIndex % MAP_WIDTH - endX;
                            int tempY = tempIndex / MAP_WIDTH - endY;
                            int tempDistance = tempX * tempX + tempY + tempY;
                            if(tempDistance < checkDistance)
                            {
                                checkDistance = tempDistance;
                                currentCheckRoadIndex = checkRoadIndex;
                                chooseGrid = tempIndex;
                            }
                        }
                        if(chooseGrid == 0)
                        {
                            continue;
                        }
                        crossGrids.push_back(chooseGrid);
                    }
                    crossGrids.push_back(endX + endY * MAP_WIDTH);
                    foreach(int crossGrid, crossGrids)
                    {
                        int crossX = crossGrid % MAP_WIDTH;
                        int crossY = crossGrid / MAP_WIDTH;
                        while(startX != crossX || startY != crossY)
                        {
                            bool moveX = false;
                            if(crossX > startX)
                            {
                                if(isRoad(startX + 1, startY))
                                {
                                    ++startX;
                                    moveX = true;
                                }
                            }
                            else if(crossX < startX)
                            {
                                if(isRoad(startX - 1, startY))
                                {
                                    --startX;
                                    moveX = true;
                                }
                            }
                            if(!moveX)
                            {
                                if(startY < crossY)
                                {
                                    ++startY;
                                }
                                else {
                                    --startY;
                                }
                            }
                            path.push_back(startY * MAP_WIDTH + startX);
                        }
                    }
                }
            }
            path.push_back(startY * MAP_WIDTH + startX);
        }
        path.push_back(targetParkingPosition->GetGridIndex());
        if(path.size() < lastLength)
        {
            lastLength = path.size();
            m_Path = path;
            m_TargetPos = target;
        }
    }
}

bool Car::isRoad(int x, int y)
{
    if(ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_Road_Left
            || ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_Road_Right)
    {
        return true;
    }
    return false;
}
