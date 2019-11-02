#include "parkmapwindow.h"

ParkMapWindow::ParkMapWindow(QWindow *parent)
    : QWindow(parent)
    , m_backingStore(new QBackingStore(this))
    , m_MapInfo(new ParkMapGridInfo)
    , m_TouchBeginTime(0)
    , m_TouchBeginX(0)
    , m_TouchBeginY(0)
{
    setGeometry(100, 100, 5000, 5000);
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

void ParkMapWindow::renderMap(QPainter *p)
{
    //p->setRenderHint(QPainter::Antialiasing);

    p->translate(m_MapInfo->GetOffsetX(), m_MapInfo->GetOffsetY());

    // draw grid
    for(int i = 0; i < MAP_WIDTH; ++i){
        p->drawLine(0, i * GRID_SIZE, GRID_SIZE * MAP_WIDTH, i * GRID_SIZE);
    }
    for(int i = 0; i < MAP_HEIGHT; ++i){
        p->drawLine(i * GRID_SIZE, 0, i * GRID_SIZE, GRID_SIZE * MAP_WIDTH);
    }
    // draw wall
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(244, 164, 96));
    p->save();
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i){
        if(m_MapInfo->GetGrid(i) == MG_Wall){
            int y = i / MAP_WIDTH;
            int x = i % MAP_WIDTH;
            p->drawRect(x * GRID_SIZE, y * GRID_SIZE, x * GRID_SIZE + GRID_SIZE, y * GRID_SIZE + GRID_SIZE);
        }
    }
    p->restore();

    // draw road
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(123, 104, 238));
    p->save();
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i){
        if(m_MapInfo->GetGrid(i) == MG_Road_Left){
            int y = i / MAP_WIDTH;
            int x = i % MAP_WIDTH;
            p->drawRect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
        }
    }
    p->restore();

    p->setPen(Qt::NoPen);
    p->setBrush(QColor(147, 112, 219));
    p->save();
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i){
        if(m_MapInfo->GetGrid(i) == MG_Road_Right){
            int y = i / MAP_WIDTH;
            int x = i % MAP_WIDTH;
            p->drawRect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
        }
    }
    p->restore();

    // draw parking place
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(65, 105, 225));
    p->save();
    for(int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i){
        if(m_MapInfo->GetGrid(i) == MG_ParkPosition){
            int y = i / MAP_WIDTH;
            int x = i % MAP_WIDTH;
            p->drawRect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
        }
    }
    p->restore();

    // draw number
    p->setPen(QColor(255, 0, 0));
    p->setBrush(QColor(255, 182, 193));
    p->save();
    for(int i = 0; i < MAP_WIDTH; ++i){
        if(i % 50 == 0){
            p->drawText(0, i * GRID_SIZE, QString::number(i));
        }
    }
    for(int i = 0; i < MAP_HEIGHT; ++i){
        if(i % 50 == 0){
            p->drawText(i * GRID_SIZE, GRID_SIZE, QString::number(i));
        }
    }
    p->restore();
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

        m_MapInfo->SetOffsetX(deltaX);
        m_MapInfo->SetOffsetY(deltaY);

        static int renderCount = 0;
        ++renderCount;
        if (renderCount % 5 == 0)
        render();
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

    m_MapInfo->SetOffsetX(deltaX);
    m_MapInfo->SetOffsetY(deltaY);

    render();
}


