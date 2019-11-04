#ifndef PARKMAPGRIDINFO_H
#define PARKMAPGRIDINFO_H

#include <QSharedPointer>

static const int MAP_WIDTH = 1000;
static const int MAP_HEIGHT = 1000;
static const int GRID_SIZE = 5;

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

    inline void SetOffsetX(int x) { offsetX += x; }
    inline int GetOffsetX() const { return offsetX; }
    inline void SetOffsetY(int y) { offsetY += y; }
    inline int GetOffsetY() const { return offsetY; }
    inline MapGrid GetGrid(int index){
        if(index < 0 || index >= MAP_WIDTH * MAP_HEIGHT){
            return MG_None;
        }
        return map[index];
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
    int offsetX = 0;
    int offsetY = 0;

    MapGrid map[MAP_WIDTH * MAP_HEIGHT];
};

#endif // PARKMAPGRIDINFO_H
