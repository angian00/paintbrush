#include "paintbrush_canvas.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QColor>

#include <iostream>


const Qt::GlobalColor& bkgColor { Qt::lightGray };
const Qt::GlobalColor& drawColor { Qt::red };


PaintbrushCanvas::PaintbrushCanvas(QWidget *parent) : QWidget(parent) {
    setFixedSize(parent->width(), parent->height());
    setMouseTracking(true);

    m_buffer = QPixmap(width(), height());
    m_buffer.fill(bkgColor);
}

void PaintbrushCanvas::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open file");
    if (fileName.isEmpty())
        return;

    if (!m_buffer.load(fileName)) {
        QMessageBox::warning(this, "Warning", "Cannot open file " + fileName);
        return;
    }
    setWindowTitle(fileName);

    update();
}

void PaintbrushCanvas::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save file");
    if (fileName.isEmpty())
        return;

    if (!m_buffer.save(fileName)) {
        QMessageBox::warning(this, "Warning", "Cannot save file " + fileName);
        return;
    }
}


void PaintbrushCanvas::paintEvent(QPaintEvent * _)
{
    QPainter painter { this };

    painter.drawPixmap(0, 0, m_buffer);
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
    painter.drawLine(m_lineStart, lineEnd);

    m_lineStart = lineEnd;

    update();
}
