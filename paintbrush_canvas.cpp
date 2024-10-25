#include "paintbrush_canvas.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QColor>



PaintbrushCanvas::PaintbrushCanvas(QWidget *parent, Editor *editor) : QWidget(parent), m_editor(editor) {
    setFixedSize(parent->width(), parent->height());
    setMouseTracking(true);
}


void PaintbrushCanvas::paintEvent(QPaintEvent * _)
{
    QPainter painter { this };
    painter.drawPixmap(0, 0, m_editor->buffer());
}


void PaintbrushCanvas::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        m_isDragging = false;
        return;
    }
    
    if (!m_isDragging) {
        m_isDragging = true;
        m_lineStart = { event->x(), event->y() };
        return;
    }

    QPoint lineEnd { event->x(), event->y() };

    m_editor->dragLine(m_lineStart, lineEnd);

    m_lineStart = lineEnd;
    update();
}
