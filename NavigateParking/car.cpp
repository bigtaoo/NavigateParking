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

//    m_StartPos = 495 * 1000 + 139;

    findPath();
    setCamera();
    m_Render = true;
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
            foreach(const int& parkingPosition, road->GetParkingPositions())
            {
                const ParkingPositionInfo* parkingPositionInfo = ParkingPositions::GetIns()->GetParkingPositionInfoByIndex(parkingPosition);
                if(!parkingPositionInfo->IsUsed()){
                    emptyParkingPosition.push_back(parkingPosition);
                }
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
