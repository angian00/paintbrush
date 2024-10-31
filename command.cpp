#include "command.h"

#include "constants.h"
#include "editor.h"


#include <QPen>



void CommandDraw::continueDrag(const QPoint from, const QPoint to) {
    m_lines->push_back(QPair<QPoint, QPoint> {from, to});
}

void CommandDraw::perform(QPainter &painter) const {
    painter.setPen(QPen(m_color, m_width));
    for (auto line: *m_lines) {
        painter.drawLine(line.first, line.second);
    }
}

void CommandDraw::paintCustomCursor(QPainter &painter, QPoint pos) const {
    auto radius = m_width / 2;
    painter.setPen(Qt::NoPen);
    painter.setBrush(cursorColor);
    painter.drawEllipse(pos.x() - radius, pos.y() - radius, radius * 2, radius * 2);
}


void CommandErase::continueDrag(const QPoint from, const QPoint to) {
    m_lines->push_back(QPair<QPoint, QPoint> {from, to});
}

void CommandErase::perform(QPainter &painter) const {
    painter.setPen(QPen(bkgColor, m_width));
    for (auto line: *m_lines) {
        painter.drawLine(line.first, line.second);
    }

}

void CommandErase::paintCustomCursor(QPainter &painter, QPoint pos) const {
    auto radius = m_width / 2;
    painter.setBrush(cursorColor);
    painter.drawEllipse(pos.x() - radius, pos.y() - radius, radius * 2, radius * 2);
}

void CommandSelect::startDrag(const QPoint pos) {
    m_from = pos;
}

void CommandSelect::continueDrag(const QPoint from, const QPoint to) {
    //m_from = from;
    m_to = to;

    m_editor->setCurrentSelection((QRect {m_from, m_to}).normalized() );
}

void CommandSelect::perform(QPainter &painter) const {
    //painter.setPen(QPen(Qt::DashLine));

    //painter.drawRect(QRect {m_from, m_to});
}
