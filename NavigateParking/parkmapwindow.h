#ifndef PARKMAPWINDOW_H
#define PARKMAPWINDOW_H

#include <QtGui>

class ParkMapWindow : public QWindow
{
    Q_OBJECT

public:
    ParkMapWindow(QWindow *parent = nullptr);

protected:
    void render();
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;

private:
    void initialize();
    void renderMap(QPainter* painter);
    void renderParkingIndex(QPainter* painter);
    void renderParkingUseInfo(QPainter* painter);
    void renderPath(QPainter* painter);
    void renderRoadIndex(QPainter* painter);
    void renderNumber(QPainter* painter);
    void renderParkPlace(QPainter* painter);
    void renderRoad(QPainter* painter);
    void renderWall(QPainter* painter);
    void renderGrid(QPainter* painter);
    bool needDraw(int x, int y);
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void timerEvent(QTimerEvent *event) override;

private:
    QBackingStore *m_backingStore;
    int m_TimerId;

    int m_TouchBeginTime;
    int m_TouchBeginX;
    int m_TouchBeginY;
    bool m_Initialized;
};
#endif // PARKMAPWINDOW_H
