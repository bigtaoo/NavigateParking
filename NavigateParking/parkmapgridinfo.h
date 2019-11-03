#ifndef PARKMAPGRIDINFO_H
#define PARKMAPGRIDINFO_H

static const int MAP_WIDTH = 1000;
static const int MAP_HEIGHT = 1000;
static const int GRID_SIZE = 10;

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

    void SetOffsetX(int x){
        offsetX += x;
    }
    int GetOffsetX() const{
        return offsetX;
    }
    void SetOffsetY(int y){
        offsetY += y;
    }
    int GetOffsetY() const{
        return offsetY;
    }
    MapGrid GetGrid(int index){
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
    // map offset for drawing
    int offsetX = 0;
    int offsetY = 0;

    MapGrid map[MAP_WIDTH * MAP_HEIGHT];
};

#endif // PARKMAPGRIDINFO_H
