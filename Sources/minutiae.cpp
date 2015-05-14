#include "minutiae.h"
#include <qmath.h>

Minutiae::Minutiae()
{
    x = -1;
    y = -1;
    type = false;
}

Minutiae::Minutiae(int _x, int _y, bool _type, int _index)
{
    x = _x;
    y = _y;
    type = _type;
    index = _index;
    checked = false;
}

double Minutiae::distance(Minutiae *m)
{
    return qSqrt(qPow((double)m->x - (double)x, 2) + qPow((double)m->y - (double)y, 2));
}
