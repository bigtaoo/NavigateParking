#include "parkingdata.h"
#include "parkmapgridinfo.h"
#include "parkingpositioninfo.h"
#include "parkingroadinfo.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

QSharedPointer<ParkingData> ParkingData::m_Ins;

ParkingData* ParkingData::GetIns()
{
    if (nullptr == m_Ins.get())
    {
        m_Ins.reset(new ParkingData());
    }
    return m_Ins.get();
}

ParkingData::ParkingData()
{
    m_JsonFileName = ":/json/parking.json";
}

ParkingData::~ParkingData()
{

}

void ParkingData::InitializeData()
{
    // check if there is have data that has saved
    QFile loadFile(m_JsonFileName);
    if (!loadFile.exists())
    {
        ParkMapGridInfo::GetIns()->Initialize();
        ParkingPositions::GetIns();
        ParkingRoads::GetIns()->Initialize();

        // wirte data
        saveData();
    }
    else
    {
        loadData();
    }
}

bool ParkingData::HasJson()
{
    QFile loadFile(m_JsonFileName);
    return loadFile.exists();
}

void ParkingData::saveData()
{
    QFile saveFile("../../../parking.json");

    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open json file.");
        return;
    }

    QJsonObject gameObject;

    // map grids
    QJsonObject parkMapGridInfoObject;
    ParkMapGridInfo::GetIns()->Write(parkMapGridInfoObject);
    gameObject["parkMapGridInfo"] = parkMapGridInfoObject;

    // parking positions
    QJsonObject parkingPositionsObject;
    ParkingPositions::GetIns()->Write(parkingPositionsObject);
    gameObject["parkingPositions"] = parkingPositionsObject;

    // parking roads
    QJsonObject parkingRoadsObject;
    ParkingRoads::GetIns()->Write(parkingRoadsObject);
    gameObject["parkingRoads"] = parkingRoadsObject;

    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
}

void ParkingData::loadData()
{
    QFile loadFile(m_JsonFileName);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open json file.");
        return;
    }

    QByteArray loadData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));

    const QJsonObject& json = loadDoc.object();

    if (json.contains("parkMapGridInfo") && json["parkMapGridInfo"].isObject())
    {
        ParkMapGridInfo::GetIns()->Read(json["parkMapGridInfo"].toObject());
    }

    if (json.contains("parkingPositions") && json["parkingPositions"].isObject())
    {
        ParkingPositions::GetIns()->Read(json["parkingPositions"].toObject());
    }

    if (json.contains("parkingRoads") && json["parkingRoads"].isObject())
    {
        ParkingRoads::GetIns()->Read(json["parkingRoads"].toObject());
    }
}
