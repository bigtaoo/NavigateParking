#include "parkingroadinfo.h"
#include "parkmapgridinfo.h"
#include "parkingpositioninfo.h"
#include <QJsonArray>
#include <QJsonObject>

QSharedPointer<ParkingRoads> ParkingRoads::m_Ins;

ParkingRoads* ParkingRoads::GetIns()
{
    if (nullptr == m_Ins.get()){
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

int ParkingRoadInfo::GetStartGrid() const
{
    if(m_Grids.empty()){
        return 0;
    }
    return m_Grids.first();
}

int ParkingRoadInfo::GetLastGrid() const
{
    if(m_Grids.empty()){
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
}

ParkingRoads::~ParkingRoads()
{
    m_kAllRoads.clear();
}

void ParkingRoads::Initialize()
{
    buildVerticalRoad();
    buildHorizontalRoad();

    buildLiknRoads();
    buildParkingPositions();
}

const ParkingRoadInfo* ParkingRoads::GetRoadByIndex(int index) const
{
    foreach(const QSharedPointer<ParkingRoadInfo>& iter, m_kAllRoads) {
        if(iter.get()->GetIndex() == index){
            return iter.get();
        }
    }
    return nullptr;
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
    if(ParkMapGridInfo::GetIns()->GetGrid(MAP_WIDTH * y + x) == MapGrid::MG_ParkPosition){
        return true;
    }
    return false;
}

void ParkingRoads::buildVerticalRoad()
{
    bool findRoad = false;
    ParkingRoadInfo* road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Vertical);
    m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
    // left
    for(int k = 1; k < MAP_HEIGHT - 1; ++k){
        if(isRoad(13, k) == findRoad){
            for(int i = 6; i < 12; ++i){
                int index = k * MAP_WIDTH + i;
                road->AddGrid(index);
            }
        }else{
            findRoad = !findRoad;
            road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Vertical);
            m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
            --k;
        }
    }
    // middle
    for(int m = 13; m < MAP_WIDTH - 13; ++m){
        // check road width
        int width = 1;
        if(isRoad(m, 13)){
            while (isRoad(m + width, 13)){
                ++width;
            }

            if(width > 0){
                road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Vertical);
                m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));

                bool newRoad = false;
                for(int k = 8; k < MAP_HEIGHT - 11; ++k){
                    if(!isRoad(m - 1, k)){
                        newRoad = true;
                        for(int i = 0; i < width; ++i){
                            int index = k * MAP_WIDTH + m + i;
                            road->AddGrid(index);
                        }
                    }else{
                        if(newRoad){
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
    for(int k = 1; k < MAP_HEIGHT - 1; ++k){
        if(isRoad(MAP_WIDTH - 19, k) == findRoad){
            for(int i = 6; i < 12; ++i){
                int index = k * MAP_WIDTH + i + MAP_WIDTH - 17;
                road->AddGrid(index);
            }
        }else{
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
    for(int k = 12; k < MAP_WIDTH - 12; ++k){
        if(isRoad(k, 13) == findRoad){
            for(int i = 6; i < 12; ++i){
                int index = i * MAP_WIDTH + k;
                road->AddGrid(index);
            }
        }else{
            findRoad = !findRoad;
            road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Horizontal);
            m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
            --k;
        }
    }
    // middle
    for(int i = 20; i < MAP_HEIGHT - 20; ++i){
        if(isRoad(13, i)){
            // road width
            int width = 1;
            while (isRoad(13, i + width)){
                ++width;
            }

            findRoad = true;
            for(int j = 12; j < MAP_WIDTH - 13; ++j){
                if(isRoad(j, i - 1) == findRoad){
                    for(int m = 0; m < width; ++m){
                        int index = (i + m) * MAP_WIDTH + j;
                        road->AddGrid(index);
                    }
                }else{
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
    findRoad = true;
    for(int k = 12; k < MAP_WIDTH - 12; ++k){
        if(isRoad(k, MAP_HEIGHT - 13) == findRoad){
            for(int i = 10; i > 4; --i){
                int index = (MAP_HEIGHT - i) * MAP_WIDTH + k;
                road->AddGrid(index);
            }
        }else{
            findRoad = !findRoad;
            road = new ParkingRoadInfo(generateIndex(), ParkingDirection::PD_Horizontal);
            m_kAllRoads.push_back(QSharedPointer<ParkingRoadInfo>(road));
            --k;
        }
    }
}

void ParkingRoads::buildLiknRoads()
{
    foreach (const QSharedPointer<ParkingRoadInfo>& road, m_kAllRoads){
        // find the road around this road
        const QVector<int>& grids = road.get()->GetGrids();
        foreach(const int& grid, grids){
            int x = grid % MAP_WIDTH;
            int y = grid / MAP_WIDTH;
            if(isRoad(x - 1, y)){
                addUniqueLinkRoad(road, x - 1, y);
            }
            if(isRoad(x + 1, y)){
                addUniqueLinkRoad(road, x + 1, y);
            }
            if(isRoad(x, y - 1)){
                addUniqueLinkRoad(road, x, y - 1);
            }
            if(isRoad(x, y + 1)){
                addUniqueLinkRoad(road, x, y + 1);
            }
        }
    }
}

void ParkingRoads::addUniqueLinkRoad(const QSharedPointer<ParkingRoadInfo>& road, int x, int y)
{
    int roadIndex = ParkMapGridInfo::GetIns()->GetRoadIndex(y * MAP_WIDTH + x);
    if(roadIndex == 0 || roadIndex == road.get()->GetIndex()){
        return;
    }
    if(!road.get()->GetLinkRoads().contains(roadIndex)){
        road.get()->AddLinkRoad(roadIndex);
    }
}

void ParkingRoads::buildParkingPositions()
{
    foreach (const QSharedPointer<ParkingRoadInfo>& road, m_kAllRoads){
        // find the road around this road
        const QVector<int>& grids = road.get()->GetGrids();
        foreach(const int& grid, grids){
            int x = grid % MAP_WIDTH;
            int y = grid / MAP_WIDTH;
            if(ParkMapGridInfo::GetIns()->GetGrid(x - 1 + y * MAP_WIDTH) == MapGrid::MG_ParkPosition){
                addUniqueParkingPosition(road, x - 1, y);
            }
            if(ParkMapGridInfo::GetIns()->GetGrid(x + 1 + y * MAP_WIDTH) == MapGrid::MG_ParkPosition){
                addUniqueParkingPosition(road, x + 1, y);
            }
            if(ParkMapGridInfo::GetIns()->GetGrid(x + (y - 1) * MAP_WIDTH) == MapGrid::MG_ParkPosition){
                addUniqueParkingPosition(road, x, y - 1);
            }
            if(ParkMapGridInfo::GetIns()->GetGrid(x + (y + 1) * MAP_WIDTH) == MapGrid::MG_ParkPosition){
                addUniqueParkingPosition(road, x, y + 1);
            }
        }
    }
}
void ParkingRoads::addUniqueParkingPosition(const QSharedPointer<ParkingRoadInfo>& road, int x, int y)
{
    int parkingPositionIndex = ParkMapGridInfo::GetIns()->GetParkingPositionIndex(x + y * MAP_WIDTH);
    if(parkingPositionIndex == 0){
        return;
    }
    const ParkingPositionInfo* info = ParkingPositions::GetIns()->GetParkingPositionInfoByIndex(parkingPositionIndex);
    if(info == nullptr || info->GetDirection() == road.get()->GetDirection()){
        return;
    }
    if(!road.get()->GetParkingPositions().contains(parkingPositionIndex)){
        road.get()->AddParkingPosition(parkingPositionIndex);
    }
}

void ParkingRoads::Write(QJsonObject &json) const
{
    QJsonArray allRoadsArray;
    foreach (const QSharedPointer<ParkingRoadInfo>& iter, m_kAllRoads){
        QJsonObject roadObject;
        iter.get()->Write(roadObject);
        allRoadsArray.append(roadObject);
    }
    json["roads"] = allRoadsArray;
}

void ParkingRoads::Read(const QJsonObject &json)
{
    if (json.contains("roads") && json["roads"].isArray()){
        QJsonArray roadsArray = json["roads"].toArray();
        m_kAllRoads.clear();
        m_kAllRoads.reserve(roadsArray.size());
        for (int i = 0; i < roadsArray.size(); ++i){
            QJsonObject roadObject = roadsArray[i].toObject();
            ParkingRoadInfo* road = new ParkingRoadInfo(0,ParkingDirection::PD_Vertical);
            road->Read(roadObject);
            m_kAllRoads.append(QSharedPointer<ParkingRoadInfo>(road));
        }
    }
}

void ParkingRoadInfo::Write(QJsonObject &json) const
{
    json["Index"] =  m_Index;
    json["Direction"] =  m_Direction;
    QJsonArray gridsArray;
    foreach(const int& iter, m_Grids){
        gridsArray.append(iter);
    }
    json["grids"] = gridsArray;
    QJsonArray parkingPositionsArray;
    foreach(const int& iter, m_ParkingPositions){
        parkingPositionsArray.append(iter);
    }
    json["parkingPositions"] = parkingPositionsArray;
    QJsonArray linkRoadsArray;
    foreach(const int& iter, m_LinkRoads){
        linkRoadsArray.append(iter);
    }
    json["linkRoads"] = linkRoadsArray;
}

void ParkingRoadInfo::Read(const QJsonObject &json)
{
    if (json.contains("Index")){
        m_Index = json["Index"].toInt();
    }
    if (json.contains("Direction")){
        m_Direction = static_cast<ParkingDirection>(json["Direction"].toInt());
    }
    if (json.contains("grids") && json["grids"].isArray()){
        QJsonArray gridsArray = json["grids"].toArray();
        m_Grids.clear();
        m_Grids.resize(gridsArray.size());
        for(int i = 0; i < gridsArray.size(); ++i){
            m_Grids[i] = gridsArray[i].toInt();
        }
    }
    if (json.contains("parkingPositions") && json["parkingPositions"].isArray()){
        QJsonArray parkingPositionsArray = json["parkingPositions"].toArray();
        m_ParkingPositions.clear();
        m_ParkingPositions.resize(parkingPositionsArray.size());
        for(int i = 0; i < parkingPositionsArray.size(); ++i){
            m_ParkingPositions[i] = parkingPositionsArray[i].toInt();
        }
    }
    if (json.contains("linkRoads") && json["linkRoads"].isArray()){
        QJsonArray linkRoadsArray = json["linkRoads"].toArray();
        m_LinkRoads.clear();
        m_LinkRoads.resize(linkRoadsArray.size());
        for(int i = 0; i < linkRoadsArray.size(); ++i){
            m_LinkRoads[i] = linkRoadsArray[i].toInt();
        }
    }
}
