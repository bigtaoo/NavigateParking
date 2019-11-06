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
    void renderMap(QPainter* painter);
    void renderParkingIndex(QPainter* painter);
    void renderParkingUseInfo(QPainter* painter);
    void renderPath(QPainter* painter);
    void renderRoadIndex(QPainter* painter);
    bool needDraw(int x, int y);
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

private:
    QBackingStore *m_backingStore;

    int m_TouchBeginTime;
    int m_TouchBeginX;
    int m_TouchBeginY;
};
#endif // PARKMAPWINDOW_H
