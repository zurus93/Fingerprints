#ifndef MINUTIAE_H
#define MINUTIAE_H

#include <QList>

class Minutiae
{
public:
    int x;
    int y;
    int index;
    bool type;
    bool checked;
    QList<Minutiae*> neighbours;

public:
    Minutiae();
    Minutiae(int, int, bool, int);
    double distance(Minutiae* m);
};
#endif // MINUTIAE_H

