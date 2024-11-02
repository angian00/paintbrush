#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <QPixmap>
#include <QColor>
#include <QPoint>


class Algorithms {
public:
    static void floodFill(QPainter &painter, const QColor &color, const QPoint & startPos);
};
#endif // ALGORITHMS_H
