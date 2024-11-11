#include "command.h"

#include "constants.h"
#include "editor.h"
#include "algorithms.h"

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


void CommandFill::perform(QPainter &painter) const {
    Algorithms::floodFill(painter, m_color, m_targetPos); 
    // auto tmpPos = QPoint {100, 100};
    // Algorithms::floodFill(painter, m_color, tmpPos);
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


void CommandCut::perform(QPainter &painter) const {
    painter.fillRect(m_targetArea, bkgColor);    
}

void CommandPaste::perform(QPainter &painter) const {
    painter.drawPixmap(m_targetArea, *m_data);
}


void CommandScroll::perform() const {
    std::cout << "CommandScroll::perform" << std::endl;

    QPoint delta {};
    if (m_mode == Primary)
        delta.setY(m_scrollAmount);
    else
        delta.setX(m_scrollAmount);

    m_editor->viewMovedBy(delta);
}

void CommandZoom::perform() const {
    //TODO: center movement around m_targetPos
    std::cout << "CommandZoom::perform" << std::endl;

    if (m_mode == Primary)
        m_editor->zoom(m_zoomFactor, m_targetPos);
    else
        m_editor->zoom(1.0/m_zoomFactor, m_targetPos);
}
