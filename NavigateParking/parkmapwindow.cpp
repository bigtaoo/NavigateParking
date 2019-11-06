#include "parkmapwindow.h"
#include "parkmapgridinfo.h"
#include "parkingpositioninfo.h"
#include "randomparking.h"
#include "car.h"
#include "parkingroadinfo.h"

ParkMapWindow::ParkMapWindow(QWindow *parent)
    : QWindow(parent)
    , m_backingStore(new QBackingStore(this))
    , m_TouchBeginTime(0)
    , m_TouchBeginX(0)
    , m_TouchBeginY(0)
{
    setGeometry(100, 100, 5000, 5000);

    ParkMapGridInfo::GetIns();

    RandomParking::GetIns();
}

void ParkMapWindow::render()
{
    QRect rect(0, 0, width(), height());
    m_backingStore->beginPaint(rect);

    QPaintDevice *device = m_backingStore->paintDevice();
    QPainter painter(device);

    painter.fillRect(0, 0, width(), height(), QGradient::SpringWarmth);
    renderMap(&painter);
    painter.end();

    m_backingStore->endPaint();
    m_backingStore->flush(rect);
}

void ParkMapWindow::renderMap(QPainter *painter)
{
    //p->setRenderHint(QPainter::Antialiasing);

    painter->translate(ParkMapGridInfo::GetIns()->GetOffsetX(), ParkMapGridInfo::GetIns()->GetOffsetY());

    // draw grid
    for(int i = 0; i < MAP_WIDTH; ++i){
        if(needDraw(0, i * GRID_SIZE))
        {
            painter->drawLine(0, i * GRID_SIZE, GRID_SIZE * MAP_WIDTH, i * GRID_SIZE);
        }
    }
    for(int i = 0; i < MAP_HEIGHT; ++i){
        if(needDraw(i * GRID_SIZE, 0))
        {
            painter->drawLine(i * GRID_SIZE, 0, i * GRID_SIZE, GRID_SIZE * MAP_WIDTH);
        }
    }
    // draw wall
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(244, 164, 96));
    painter->save();
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i){
        if(ParkMapGridInfo::GetIns()->GetGrid(i) == MG_Wall){
            int y = i / MAP_WIDTH;
            int x = i % MAP_WIDTH;
            if(needDraw(x * GRID_SIZE, y * GRID_SIZE))
            {
                painter->drawRect(x * GRID_SIZE, y * GRID_SIZE, x * GRID_SIZE + GRID_SIZE, y * GRID_SIZE + GRID_SIZE);
            }
        }
    }
    painter->restore();

    // draw road
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(123, 104, 238));
    painter->save();
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i){
        if(ParkMapGridInfo::GetIns()->GetGrid(i) == MG_Road_Left){
            int y = i / MAP_WIDTH;
            int x = i % MAP_WIDTH;
            if(needDraw(x * GRID_SIZE, y * GRID_SIZE))
            {
                painter->drawRect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
            }
        }
    }
    painter->restore();

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(147, 112, 219));
    painter->save();
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i){
        if(ParkMapGridInfo::GetIns()->GetGrid(i) == MG_Road_Right){
            int y = i / MAP_WIDTH;
            int x = i % MAP_WIDTH;
            if(needDraw(x * GRID_SIZE, y * GRID_SIZE))
            {
                painter->drawRect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
            }
        }
    }
    painter->restore();

    // draw parking place
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(65, 105, 225));
    painter->save();
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i){
        if(ParkMapGridInfo::GetIns()->GetGrid(i) == MG_ParkPosition){
            int y = i / MAP_WIDTH;
            int x = i % MAP_WIDTH;
            if(needDraw(x * GRID_SIZE, y * GRID_SIZE))
            {
                painter->drawRect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
            }
        }
    }
    painter->restore();

    // draw number
    painter->setPen(QColor(255, 0, 0));
    painter->setBrush(QColor(255, 182, 193));
    painter->save();
    for(int i = 0; i < MAP_WIDTH; ++i){
        if(i % 50 == 0){
            if(needDraw(i * GRID_SIZE, GRID_SIZE))
            {
                painter->drawText(i * GRID_SIZE, GRID_SIZE, QString::number(i));
            }
        }
    }
    for(int i = 0; i < MAP_HEIGHT; ++i){
        if(i % 50 == 0){
            if(needDraw(0, i * GRID_SIZE + GRID_SIZE))
            {
                painter->drawText(0,i * GRID_SIZE + GRID_SIZE,  QString::number(i));
            }
        }
    }
    painter->restore();

    renderParkingUseInfo(painter);
    //renderParkingIndex(painter);
    renderPath(painter);
    renderRoadIndex(painter);
}

void ParkMapWindow::renderParkingUseInfo(QPainter* painter)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(178,34,34));
    painter->save();
    const QVector<QSharedPointer<ParkingPositionInfo>>& all = ParkingPositions::GetIns()->GetAllPositions();
    foreach (const QSharedPointer<ParkingPositionInfo> iter, all)
    {
        if(iter.get()->IsUsed())
        {
            int index = iter.get()->GetGridIndex();
            int x = index % MAP_WIDTH;
            int y = index / MAP_WIDTH;
            if(needDraw(x * GRID_SIZE, y * GRID_SIZE + GRID_SIZE))
            {
                for(int i = 0; i < 3; ++i)
                {
                    for(int j = 0; j < 3; ++j)
                    {
                        painter->drawRect((x + i) * GRID_SIZE, (y + j) * GRID_SIZE, GRID_SIZE, GRID_SIZE);
                    }
                }
            }
        }
    }
    painter->restore();
}

void ParkMapWindow::renderParkingIndex(QPainter* painter)
{
    painter->setPen(QColor(255, 0, 0));
    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);
    painter->setBrush(QColor(255, 182, 193));
    painter->save();

    const QVector<QSharedPointer<ParkingPositionInfo>>& all = ParkingPositions::GetIns()->GetAllPositions();
    foreach (const QSharedPointer<ParkingPositionInfo> iter, all)
    {
        int index = iter.get()->GetGridIndex();
        int x = index % MAP_WIDTH;
        int y = index / MAP_WIDTH;

        if(needDraw(x * GRID_SIZE, y * GRID_SIZE + GRID_SIZE))
        {
            painter->drawText(x * GRID_SIZE, y * GRID_SIZE + GRID_SIZE, QString::number(iter.get()->GetIndex()));
        }
    }
    painter->restore();
}

void ParkMapWindow::renderRoadIndex(QPainter* painter)
{
    painter->setPen(QColor(255, 255, 0));
    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);
    painter->setBrush(QColor(255, 182, 193));
    painter->save();

    const QVector<QSharedPointer<ParkingRoadInfo>>& all = ParkingRoads::GetIns()->GetRoads();
    foreach (const QSharedPointer<ParkingRoadInfo> iter, all)
    {
        int index = iter.get()->GetStartGrid();
        int x = index % MAP_WIDTH;
        int y = index / MAP_WIDTH;

        if(needDraw(x * GRID_SIZE, y * GRID_SIZE + GRID_SIZE))
        {
            painter->drawText(x * GRID_SIZE, y * GRID_SIZE + GRID_SIZE, QString::number(iter.get()->GetIndex()));
        }
    }
    painter->restore();
}

void ParkMapWindow::renderPath(QPainter* painter)
{
    // path
    const QVector<int> path = Car::GetIns()->GetPath();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(34,139,34));
    painter->save();
    foreach (int iter, path)
    {
        int x = iter % MAP_WIDTH;
        int y = iter / MAP_WIDTH;

        if(needDraw(x * GRID_SIZE, y * GRID_SIZE + GRID_SIZE))
        {
            painter->drawRect(x * GRID_SIZE, y * GRID_SIZE,GRID_SIZE, GRID_SIZE);
        }
    }
    painter->restore();

    // car
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255,215,0));
    painter->save();
    int x = Car::GetIns()->GetCurrentPos() % MAP_WIDTH;
    int y = Car::GetIns()->GetCurrentPos() / MAP_WIDTH;
    for(int i = 0; i < 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            painter->drawRect((x + i) * GRID_SIZE, (y + j) * GRID_SIZE, GRID_SIZE, GRID_SIZE);
        }
    }

    painter->restore();
}

void ParkMapWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    m_backingStore->resize(resizeEvent->size());
}

void ParkMapWindow::exposeEvent(QExposeEvent *)
{
    if (isExposed())
        render();
}

bool ParkMapWindow::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        m_TouchBeginTime = 0;
        return true;
    }
    else if(event->type() == QEvent::TouchBegin)
    {
        m_TouchBeginTime = QTime::currentTime().msec();

        m_TouchBeginX = static_cast<int>(dynamic_cast<QTouchEvent*>(event)->touchPoints()[0].pos().x());
        m_TouchBeginY = static_cast<int>(dynamic_cast<QTouchEvent*>(event)->touchPoints()[0].pos().y());
        return true;
    }
    else if(event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel)
    {
        m_TouchBeginTime = 0;
        return true;
    }
    else if(event->type() == QEvent::TouchUpdate)
    {
        if(m_TouchBeginTime == 0)
        {
            return true;
        }
        int curX = static_cast<int>(dynamic_cast<QTouchEvent*>(event)->touchPoints()[0].pos().x());
        int curY = static_cast<int>(dynamic_cast<QTouchEvent*>(event)->touchPoints()[0].pos().y());
        int deltaX = curX - m_TouchBeginX;
        int deltaY = curY - m_TouchBeginY;
        m_TouchBeginX = curX;
        m_TouchBeginY = curY;

        ParkMapGridInfo::GetIns()->SetOffsetX(deltaX);
        ParkMapGridInfo::GetIns()->SetOffsetY(deltaY);

        static int renderTime = QTime::currentTime().msec();
        int curTime = QTime::currentTime().msec();
        if(curTime - renderTime > 30)
        {
            renderTime += 30;
            if(curTime - renderTime > 30)
            {
                renderTime = curTime;
            }
            render();
        }
    }
    return QWindow::event(event);
}

void ParkMapWindow::mousePressEvent(QMouseEvent *event)
{
    m_TouchBeginTime = QTime::currentTime().msec();

    m_TouchBeginX = event->x();
    m_TouchBeginY = event->y();
}

void ParkMapWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(m_TouchBeginTime == 0)
    {
        return;
    }
    int curX = event->pos().x();
    int curY = event->pos().y();
    int deltaX = curX - m_TouchBeginX;
    int deltaY = curY - m_TouchBeginY;
    m_TouchBeginX = curX;
    m_TouchBeginY = curY;

    ParkMapGridInfo::GetIns()->SetOffsetX(deltaX);
    ParkMapGridInfo::GetIns()->SetOffsetY(deltaY);

    render();
}

bool ParkMapWindow::needDraw(int x, int y)
{
    int adjustX = x + ParkMapGridInfo::GetIns()->GetOffsetX();
    int adjustY = y + ParkMapGridInfo::GetIns()->GetOffsetY();
    if(adjustX > -GRID_SIZE * 2 && adjustX < width() + GRID_SIZE * 2
            && adjustY > -GRID_SIZE * 2 && adjustY < height() + GRID_SIZE * 2)
    {
        return true;
    }
    return false;
}
