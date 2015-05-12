#ifndef MINUTEA_H
#define MINUTEA_H

#include <QList>

class Minutea
{
public:
    int x;
    int y;
    int index;
    bool type;
    bool checked;
    QList<Minutea*> neighbours;

public:
    Minutea();
    Minutea(int, int, bool, int);
    double distance(Minutea* m);
};
#endif // MINUTEA_H

