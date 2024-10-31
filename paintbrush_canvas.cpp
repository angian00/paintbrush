#include "paintbrush_canvas.h"
#include "command.h"
#include "editor.h"
#include "qnamespace.h"

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



void PaintbrushCanvas::paintEvent(QPaintEvent * _) {
    m_editor->paintCurrentBuffer(this);
    m_editor->performCurrentCommand(this);

    m_editor->paintToolCursor(m_currMousePos, this);
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
    if ((m_isDragging) && !(event->buttons() & Qt::LeftButton)) {
        m_isDragging = false;
        dragEnded(event->pos());
    }
}
