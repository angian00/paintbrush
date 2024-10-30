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


void PaintbrushCanvas::onToolChosen(CommandType newTool) {
    std::cout << "canvas::onToolChosen" << std::endl;

    //TODO: use hash instead
    switch (newTool) {
        case Draw:
        case Erase:
            //cursor is custom-drawn in paintEvent
            setCursor(Qt::BlankCursor);
            break;
        case Select:
            setCursor(Qt::CrossCursor);
    }
}


void PaintbrushCanvas::paintEvent(QPaintEvent * _) {
    QPainter painter { this };
    painter.drawPixmap(0, 0, m_editor->buffer());
    m_editor->performCurrentCommand(painter);

    QPainter painter2 { this };
    m_editor->paintToolCursor(painter2, m_currMousePos);
}


void PaintbrushCanvas::mouseMoveEvent(QMouseEvent *event) {
    m_currMousePos = event->pos();

    if (!(event->buttons() & Qt::LeftButton)) {
        if (m_isDragging) {
            m_editor->onEndDrag();
            m_isDragging = false;
        }
    
        update();
        return;
    }
    
    if (!m_isDragging) {
        m_isDragging = true;
        m_editor->onStartDrag();
        m_dragStart = { event->x(), event->y() };
        return;
    }

    QPoint dragEnd { event->x(), event->y() };

    m_editor->onDrag(m_dragStart, dragEnd);

    m_dragStart = dragEnd;

    update();
}
