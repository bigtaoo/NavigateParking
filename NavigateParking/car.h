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

    void SetPath(QVector<int> path);

    inline int GetCurrentPos() const { return m_StartPos; }
    inline int GetTargetPos() const { return m_TargetPos; }
    inline const QVector<int> GetPath() const { return m_Path; }

    void InitPosition(int pos);

private:
    void findPath();

private:
    static QSharedPointer<Car> m_Ins;
    int m_StartPos;
    int m_TargetPos;
    QVector<int> m_Path;
};

#endif // CAR_H
