#include "car.h"

QSharedPointer<Car> Car::m_Ins;

Car* Car::GetIns()
{
    if (nullptr == m_Ins.get())
    {
        m_Ins.reset(new Car());
    }
    return m_Ins.get();
}

Car::Car()
{
    m_Path.reserve(1000);
}

Car::~Car()
{
    m_Path.clear();
}

void Car::SetPath(QVector<int> path)
{
    m_StartPos = path.first();
    m_TargetPos = path.last();
    m_Path = path;
}

void Car::InitPosition(int pos)
{
    m_StartPos = pos;

    findPath();
}

void Car::findPath()
{
    // currently road

}
