#include "algorithms.h"

#include <QPixmap>
#include <QColor>
#include <QPoint>
#include <QImage>
#include <QPainter>

#include <memory>
#include <iostream>
#include <queue>


void Algorithms::floodFill(QPainter &painter, const QColor &fillColor, const QPoint & startPos) {
    const int threshold = 1; 

    auto srcBuffer = static_cast<QPixmap *> (painter.device());
    auto image = srcBuffer->toImage();

    const int w = image.width();
    const int h = image.height();

    bool visited[w][h];
    memset(visited, false, sizeof(visited));

    auto startColor = image.pixelColor(startPos.x(), startPos.y());

    auto nodeQueue = std::queue<QPoint>();
    nodeQueue.push(startPos);

    while (!nodeQueue.empty()) {
        auto currNode = nodeQueue.front();
        nodeQueue.pop();

        auto x = currNode.x();
        auto y = currNode.y();
        if (visited[x][y])
            continue;

        visited[x][y] = true;
        auto currColor = image.pixelColor(x, y);

        if ( (abs(startColor.red() - currColor.red()) > threshold) ||
            (abs(startColor.green() - currColor.green()) > threshold) ||
            (abs(startColor.blue() - currColor.blue()) > threshold) ||
            (abs(startColor.alpha() - currColor.alpha()) > threshold) ) {
            //not inside
            continue;
        }

        image.setPixelColor(x, y, fillColor);

        if (x > 0)
            nodeQueue.push(QPoint(x-1, y  ));
        if (x < w-1)
            nodeQueue.push(QPoint(x+1, y)  );
        if (y > 0)
            nodeQueue.push(QPoint(x  , y-1));
        if (y < h-1)
            nodeQueue.push(QPoint(x  , y+1));
    }

    painter.drawImage(QPoint {0, 0}, image);
}

