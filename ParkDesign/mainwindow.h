#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QPushButton>
#include "mapinfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWindow *parent = nullptr);
    ~MainWindow() override;

    virtual void render();

protected:
    bool event(QEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;

private:
    void createButtons();
    void renderMap(QPainter* painter);
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

private slots:
    void handleButton();

private:
    Ui::MainWindow *ui;
    QBackingStore *m_backingStore;
    QPushButton *m_WallButton;

    MapInfo* m_MapInfo;
    int m_TouchBeginTime;
    int m_TouchBeginX;
    int m_TouchBeginY;
};
#endif // MAINWINDOW_H
