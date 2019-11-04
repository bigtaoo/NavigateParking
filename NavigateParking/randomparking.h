#ifndef RANDOMPARKING_H
#define RANDOMPARKING_H

#include <QSharedPointer>

class RandomParking
{
public:
    static RandomParking* GetIns();
    explicit RandomParking();
    ~RandomParking();

    void Update();

private:
    void InitParking();

private:
    static QSharedPointer<RandomParking> m_Ins;
};

#endif // RANDOMPARKING_H
