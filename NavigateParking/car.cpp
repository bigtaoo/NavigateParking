#include "car.h"
#include "parkingroadinfo.h"
#include "parkmapgridinfo.h"
#include "parkingpositioninfo.h"
#include "astar.h"
#include <QRandomGenerator>

QSharedPointer<Car> Car::m_Ins;

Car* Car::GetIns()
{
    if (nullptr == m_Ins.get()){
        m_Ins.reset(new Car());
    }
    return m_Ins.get();
}

Car::Car() : m_StartPos(0), m_TargetPos(0), m_TimeCount(0), m_Render(false)
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

//    m_StartPos = 838 * 1000 + 981;

    findPath();
    setCamera();
    m_Render = true;
}

void Car::setCamera()
{
    // move camera
    int x = m_StartPos % MAP_WIDTH;
    int y = m_StartPos / MAP_WIDTH;
    int offsetX = -x * GRID_SIZE + ParkMapGridInfo::GetIns()->GetMapWidth() / 2;
    int offsetY = -y * GRID_SIZE + ParkMapGridInfo::GetIns()->GetMapHeight() / 2;
    if(offsetX > 0){
        offsetX = 0;
    }else if(offsetX < -MAP_WIDTH * GRID_SIZE + ParkMapGridInfo::GetIns()->GetMapWidth()){
        offsetX = -MAP_WIDTH * GRID_SIZE + ParkMapGridInfo::GetIns()->GetMapWidth();
    }
    if(offsetY > 0){
        offsetY = 0;
    }else if(offsetY < -MAP_WIDTH * GRID_SIZE + ParkMapGridInfo::GetIns()->GetMapHeight()){
        offsetY = -MAP_WIDTH * GRID_SIZE + ParkMapGridInfo::GetIns()->GetMapHeight();
    }
    ParkMapGridInfo::GetIns()->SetOffsetX(offsetX);
    ParkMapGridInfo::GetIns()->SetOffsetY(offsetY);
}

void Car::Update()
{
//    return;
    ++m_TimeCount;
    if(m_TimeCount == 3){
        const ParkingPositionInfo* parkingPosition = ParkingPositions::GetIns()->GetParkingPositionInfoByIndex(m_TargetPos);
        if(parkingPosition != nullptr){
            parkingPosition->Use();
        }
    }
    if(m_TimeCount == 5){
        m_TimeCount = 0;
        RandomPosition();
    }
}

void Car::findEmptyParkingPosition(QVector<int>& emptyParkingPosition)
{
    // currently road
    int roadIndex = ParkMapGridInfo::GetIns()->GetRoadIndex(m_StartPos);
    int startX = m_StartPos % MAP_WIDTH;
    int startY = m_StartPos / MAP_WIDTH;
    QVector<int> nextList;
    QVector<int> openList;
    QVector<int> closeList;
    nextList.push_back(roadIndex);
    int limit = 0;
    while (emptyParkingPosition.empty())
    {
        ++limit;
        if(limit > 20){
            m_TargetPos = m_StartPos;
            return;
        }
        openList = nextList;
        nextList.clear();
        foreach (const int& checkRoadIndex, openList)
        {
            const ParkingRoadInfo* road = ParkingRoads::GetIns()->GetRoadByIndex(checkRoadIndex);
            // each road only choose the nearest one
            int dis = MAP_WIDTH * MAP_HEIGHT;
            int choosePos = 0;
            foreach(const int& parkingPosition, road->GetParkingPositions())
            {
                const ParkingPositionInfo* parkingPositionInfo = ParkingPositions::GetIns()->GetParkingPositionInfoByIndex(parkingPosition);
                if(!parkingPositionInfo->IsUsed()){
                    int posIndex = parkingPositionInfo->GetGridIndex();
                    int delta = qAbs(startX - posIndex % MAP_WIDTH) + qAbs(startY - posIndex / MAP_WIDTH);
                    if(delta < dis){
                        dis = delta;
                        choosePos = parkingPosition;
                    }
                }
            }
            if(choosePos != 0){
                emptyParkingPosition.push_back(choosePos);
            }
            foreach(const int& newRoad, road->GetLinkRoads()){
                if(!closeList.contains(newRoad)){
                    nextList.push_back(newRoad);
                }
            }
            closeList.push_back(checkRoadIndex);
        }
    }
}

void Car::findPath()
{
    // find the empty parking position in nearly road
    QVector<int> emptyParkingPosition;
    findEmptyParkingPosition(emptyParkingPosition);

    // find the nearest parking position
    QVector<int> path;
    int lastLength = MAP_WIDTH * MAP_HEIGHT;
    foreach(const int& target, emptyParkingPosition)
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
                if(isRoad(endX - i, endY)){
                    endX -= i;
                    break;
                }if(isRoad(endX + i, endY)){
                    endX += i;
                    break;
                }
            }else{
                if(isRoad(endX, endY - i)){
                    endY -= i;
                    break;
                }if(isRoad(endX, endY + i)){
                    endY += i;
                    break;
                }
            }
        }
        AStar::FindPath(startX + startY * MAP_WIDTH, endX + endY * MAP_WIDTH, path);
        path.push_back(targetParkingPosition->GetGridIndex());
        if(path.size() < lastLength){
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
