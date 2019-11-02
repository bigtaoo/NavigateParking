#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>

MainWindow::MainWindow(QWindow *parent)
    : QWindow(parent)
    , ui(new Ui::MainWindow)
    , m_backingStore(new QBackingStore(this))
    , m_MapInfo(new MapInfo)
    , m_TouchBeginTime(0)
    , m_TouchBeginX(0)
    , m_TouchBeginY(0)
{
    setGeometry(100, 100, 5000, 5000);

    createButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::render()
{
    QRect rect(0, 0, width(), height());
    m_backingStore->beginPaint(rect);

    QPaintDevice *device = m_backingStore->paintDevice();
    QPainter painter(device);

    painter.fillRect(0, 0, width(), height(), QGradient::NightFade);
    renderMap(&painter);
    painter.end();

    m_backingStore->endPaint();
    m_backingStore->flush(rect);
}

void MainWindow::renderMap(QPainter *p)
{
    p->setRenderHint(QPainter::Antialiasing);

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

void MainWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    m_backingStore->resize(resizeEvent->size());
}

void MainWindow::exposeEvent(QExposeEvent *)
{
    if (isExposed())
        render();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        m_TouchBeginTime = 0;
        return true;
    }
    else if(event->type() == QEvent::TouchBegin)
    {
        int a = 0;
    }
    else if(event->type() == QEvent::TouchEnd)
    {
        int a = 0;
    }
    return QWindow::event(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_TouchBeginTime = QTime::currentTime().msec();

    m_TouchBeginX = event->x();
    m_TouchBeginY = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(m_TouchBeginTime == 0)
    {
        return;
    }
    int deltaX = event->pos().x() - m_TouchBeginX;
    int deltaY = event->pos().y() - m_TouchBeginY;

    m_MapInfo->SetOffsetX(deltaX / 10);
    m_MapInfo->SetOffsetY(deltaY / 10);

    render();
}

void MainWindow::createButtons()
{
    m_WallButton = new QPushButton("My Button");
    m_WallButton->setGeometry(QRect(QPoint(100, 100),QSize(200, 50)));

    // Connect button signal to appropriate slot
    connect(m_WallButton, SIGNAL (released()), this, SLOT (handleButton()));
}

void MainWindow::handleButton()
{
   // change the text
   m_WallButton->setText("Example");
   // resize button
   m_WallButton->resize(100,100);
}
