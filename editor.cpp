#include "editor.h"

#include <QPainter>
#include <QPen>


const Qt::GlobalColor& bkgColor { Qt::lightGray };


Editor::Editor(int width, int height): m_width(width), m_height(height) {
    m_buffer = QPixmap(m_width, m_height);
    m_buffer.fill(bkgColor);
}

bool Editor::loadFile(const QString filename) {
    return m_buffer.load(filename);
}

bool Editor::saveFile(const QString filename) {
    return m_buffer.save(filename);
}


void Editor::dragLine(const QPoint start, const QPoint end) {
    QColor currColor;
    int currWidth;
    if (m_activeTool == Draw) {
        //currColor = drawColor;
        currColor = m_activeColor;
        currWidth = 2;
    } else {
        currColor = bkgColor;
        currWidth = 10;
    }

    QPainter painter {&m_buffer};
    painter.setPen(QPen(currColor, currWidth));
    painter.drawLine(start, end);
}