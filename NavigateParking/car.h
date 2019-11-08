#ifndef CAR_H
#define CAR_H

#include <QSharedPointer>
#include <QVector>

class Car
{
public:
    Car();
    ~Car();

    static Car* GetIns();

    inline int GetCurrentPos() const { return m_StartPos; }
    inline int GetTargetPos() const { return m_TargetPos; }
    inline const QVector<int> GetPath() const { return m_Path; }

    void InitPosition(int pos);
    void RandomPosition();
    void Update();

private:
    void findPath();
    bool isRoad(int x, int y);
    void setCamera();

private:
    static QSharedPointer<Car> m_Ins;
    int m_StartPos;
    int m_TargetPos;
    int m_TimeCount = 0;
    QVector<int> m_Path;   
};

#endif // CAR_H
