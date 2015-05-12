#include "minutea.h"
#include <qmath.h>

Minutea::Minutea()
{
    x = -1;
    y = -1;
    type = false;
}

Minutea::Minutea(int _x, int _y, bool _type, int _index)
{
    x = _x;
    y = _y;
    type = _type;
    index = _index;
    checked = false;
}

double Minutea::distance(Minutea *m)
{
    return qSqrt(qPow((double)m->x - (double)x, 2) + qPow((double)m->y - (double)y, 2));
}
