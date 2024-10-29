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


void PaintbrushCanvas::onToolChosen(ToolType newTool) {
    switch (newTool) {
        case ToolDraw:
        case ToolErase:
            //cursor is custom-drawn in paintEvent
            setCursor(Qt::BlankCursor);
            break;
        case ToolSelect:
            setCursor(Qt::CrossCursor);
    }
}


void PaintbrushCanvas::paintEvent(QPaintEvent * _) {
    QPainter painter { this };
    painter.drawPixmap(0, 0, m_editor->buffer());
    paintToolCursor(painter, m_editor->activeTool());
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

void drawCursorCircle(QPainter &painter, QPoint center, int radius);

void PaintbrushCanvas::paintToolCursor(QPainter &painter, const ToolData *toolData) {
    const ToolType toolType = toolData->type();
    
    switch (toolType) {
        case ToolDraw:
        case ToolErase: 
            {
                auto radius = toolData->cursorSize()/2;
                drawCursorCircle(painter, m_currMousePos, radius);
            }
            break;
        
        default:
            ; //no custom painting, rely on QCursor
    }
}


void drawCursorCircle(QPainter &painter, QPoint center, int radius) {
    const QColor cursorColor = Qt::black;

    //std::cout << "drawCursorCircle; radius=" << radius << std::endl;

    painter.setBrush(cursorColor);
    painter.drawEllipse(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
}
