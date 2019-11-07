#ifndef PARKMAPGRIDINFO_H
#define PARKMAPGRIDINFO_H

#include <QSharedPointer>

static const int MAP_WIDTH = 1000;
static const int MAP_HEIGHT = 1000;
static const int GRID_SIZE = 15;

enum MapGrid
{
    MG_None = 1 << 0,
    MG_Wall = 1 << 1,
    MG_Road_Left = 1 << 2,
    MG_Road_Right = 1 << 3,
    MG_ParkPosition = 1 << 4,
};

class ParkMapGridInfo
{
public:
    explicit ParkMapGridInfo();
    ~ParkMapGridInfo();

    static ParkMapGridInfo* GetIns();
    void Initialize();

    inline void SetOffsetX(int x) { m_OffsetX += x; }
    inline int GetOffsetX() const { return m_OffsetX; }
    inline void SetOffsetY(int y) { m_OffsetY += y; }
    inline int GetOffsetY() const { return m_OffsetY; }
    inline MapGrid GetGrid(int index){
        if(index < 0 || index >= MAP_WIDTH * MAP_HEIGHT){
            return MG_None;
        }
        return m_Map[index];
    }
    inline void SetRoadIndex(int gridIndex, int roadIndex)
    {
        if(gridIndex < 0 || gridIndex >= MAP_WIDTH * MAP_HEIGHT)
        {
            return;
        }
        m_GridToRoad[gridIndex] = roadIndex;
    }
    inline int GetRoadIndex(int gridIndex)
    {
        if(gridIndex < 0 || gridIndex >= MAP_WIDTH * MAP_HEIGHT)
        {
            return 0;
        }
        return m_GridToRoad[gridIndex];
    }
    inline void SetParkingPositionIndex(int gridIndex, int parkingPositionIndex)
    {
        if(gridIndex < 0 || gridIndex >= MAP_WIDTH * MAP_HEIGHT)
        {
            return;
        }
        m_GridToParkingPosition[gridIndex] = parkingPositionIndex;
    }
    inline int GetParkingPositionIndex(int gridIndex)
    {
        if(gridIndex < 0 || gridIndex >= MAP_WIDTH * MAP_HEIGHT)
        {
            return 0;
        }
        return m_GridToParkingPosition[gridIndex];
    }
private:
    void buildLeftPart();
    void buildRightPart();
    void buildCenterHorizontal();
    void buildTopAndBottom();
    void buildVerticalRoad();
    void buildParkingPosition();

private:
    static QSharedPointer<ParkMapGridInfo> m_Ins;
    // map offset for drawing
    int m_OffsetX = 0;
    int m_OffsetY = 0;

    MapGrid m_Map[MAP_WIDTH * MAP_HEIGHT];
    int m_GridToRoad[MAP_WIDTH * MAP_HEIGHT];
    int m_GridToParkingPosition[MAP_WIDTH * MAP_HEIGHT];
};

#endif // PARKMAPGRIDINFO_H
