#include "paintbrush_canvas.h"
#include "command.h"
#include "editor.h"
#include "constants.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QColor>


void paintBackgroundPattern(QWidget * target);


PaintbrushCanvas::PaintbrushCanvas(QWidget *parent, Editor *editor) : QWidget(parent), m_editor(editor) {
    setFixedSize(parent->width(), parent->height());
    setMouseTracking(true);
}


void PaintbrushCanvas::paintEvent(QPaintEvent * _) {
    paintBackgroundPattern(this);
    m_editor->paintCurrentBuffer(this);
    m_editor->performCurrentCommand(this);

    m_editor->paintCustomCursor(m_currMousePos, this);
    m_editor->paintCurrentSelection(this);
}


void PaintbrushCanvas::mouseMoveEvent(QMouseEvent *event) {
    m_currMousePos = event->pos();
    update();
    
    if (!m_isDragging)
        return;

    QPoint dragEnd { event->x(), event->y() };

    dragContinued(m_dragStart, dragEnd);

    m_dragStart = dragEnd;
}


void PaintbrushCanvas::mousePressEvent(QMouseEvent *event) {
    if ((!m_isDragging) && (event->buttons() & Qt::LeftButton)) {
        m_isDragging = true;
        m_dragStart = { event->x(), event->y() };
        dragStarted(event->pos());
    }
}

void PaintbrushCanvas::mouseReleaseEvent(QMouseEvent *event) {
    clicked(event->pos());
    
    if ((m_isDragging) && !(event->buttons() & Qt::LeftButton)) {
        m_isDragging = false;
        dragEnded(event->pos());
    }
}


void paintBackgroundPattern(QWidget * target) {
    QPainter painter { target };

    for (int xTile=0; xTile < target->width(); xTile++) {
        for (int yTile=0; yTile < target->height(); yTile++) {
            QColor color;

            if ((xTile + yTile) % 2 == 0)
                color = bkgPatternColor1;
            else
                color = bkgPatternColor2;

            int tileW = std::min(bkgPatternSize, target->width()  - xTile*bkgPatternSize);
            int tileH = std::min(bkgPatternSize, target->height() - yTile*bkgPatternSize);
            auto targetArea = QRect {xTile*bkgPatternSize, yTile*bkgPatternSize, tileW, tileH};

            painter.fillRect(targetArea, color);
        }
    }
}
