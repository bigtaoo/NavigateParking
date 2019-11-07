#include "parkingpositioninfo.h"
#include "parkmapgridinfo.h"
#include <qjsonarray.h>
#include <qjsonobject.h>

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

void ParkingPositions::Write(QJsonObject &json) const
{
    QJsonArray parkingPositionsArray;
    foreach (const QSharedPointer<ParkingPositionInfo>& iter, m_AllParkingPositions)
    {
        QJsonObject parkingPositionObject;
        iter.get()->Write(parkingPositionObject);
        parkingPositionsArray.append(parkingPositionObject);
    }
    json["parkingPositions"] = parkingPositionsArray;
}

void ParkingPositions::Read(const QJsonObject &json)
{
    if (json.contains("parkingPositions") && json["parkingPositions"].isArray())
    {
        QJsonArray parkingPositionsArray = json["parkingPositions"].toArray();
        m_AllParkingPositions.clear();
        m_AllParkingPositions.reserve(parkingPositionsArray.size());
        for (int i = 0; i < parkingPositionsArray.size(); ++i)
        {
            QJsonObject positionObject = parkingPositionsArray[i].toObject();
            ParkingPositionInfo* parkingPosition = new ParkingPositionInfo(0,ParkingDirection::PD_Vertical,0,0,0);
            parkingPosition->Read(positionObject);
            m_AllParkingPositions.append(QSharedPointer<ParkingPositionInfo>(parkingPosition));
        }
    }
}

void ParkingPositionInfo::Write(QJsonObject &json) const
{
    json["ParkingIndex"] =  m_ParkingIndex;
    json["Direction"] =  m_Direction;
    json["GridIndex"] =  m_GridIndex;
    json["Height"] =  m_Height;
    json["Width"] =  m_Width;
    json["IsUsed"] =  m_IsUsed;
}

void ParkingPositionInfo::Read(const QJsonObject &json)
{
    if (json.contains("ParkingIndex"))
    {
        m_ParkingIndex = json["ParkingIndex"].toInt();
    }
    if (json.contains("Direction"))
    {
        m_Direction = static_cast<ParkingDirection>(json["Direction"].toInt());
    }
    if (json.contains("GridIndex"))
    {
        m_GridIndex = json["GridIndex"].toInt();
    }
    if (json.contains("Height"))
    {
        m_Height = json["Height"].toInt();
    }
    if (json.contains("Width"))
    {
        m_Width = json["Width"].toInt();
    }
    if (json.contains("IsUsed") && json["IsUsed"].isBool())
    {
        m_IsUsed = json["IsUsed"].toInt();
    }
}
