#include "paintbrush_canvas.h"
#include "command.h"
#include "editor.h"
#include "constants.h"
#include "qtransform.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QColor>
#include <QSize>

#include <math.h>

void paintBackgroundPattern(QWidget * target);

PaintbrushCanvas::PaintbrushCanvas(QWidget *parent, Editor *editor) : 
    QWidget(parent), m_editor(editor), m_documentSize(QSize()), m_scaleFactor(1.0) {

    setFixedSize(m_documentSize);
    setMouseTracking(true);
}

void PaintbrushCanvas::onDocumentSizeChanged(QSize size) {
    m_documentSize = size;
    updateCanvasSize();

}

void PaintbrushCanvas::onZoomLevelChanged(int zoomLevel) {
    std::cout << "onZoomLevelChanged: " << zoomLevel << std::endl;
    m_scaleFactor = zoomLevel / 100.0;
    updateCanvasSize();
}

void PaintbrushCanvas::updateCanvasSize() {
    auto canvasSize = m_documentSize * m_scaleFactor;
    std::cout << "setting PaintbrushCanvas size to: " << canvasSize.width() << ", " << canvasSize.height() << std::endl;
    setFixedSize(canvasSize);

    update();
}

void PaintbrushCanvas::paintEvent(QPaintEvent * _) {
    paintBackgroundPattern(this);

    QPainter painter { this };
    painter.setRenderHints(QPainter::Antialiasing);

    QTransform transform = QTransform::fromScale(m_scaleFactor, m_scaleFactor);
    painter.setTransform(transform);

    m_editor->paintCurrentBuffer(&painter);
    m_editor->performPartialCommand(&painter);

    // m_editor->paintCurrentBuffer(this);
    // m_editor->performCurrentCommand(this);

    transform.reset();
    painter.setTransform(transform);

    m_editor->paintCustomCursor(m_currMousePos, this);
    m_editor->paintCurrentSelection(this);
}


void PaintbrushCanvas::mouseMoveEvent(QMouseEvent *event) {
    m_currMousePos = event->pos();
    update();
    
    if (!m_isDragging)
        return;

    QPoint dragEnd = scalePoint(m_currMousePos);

    emit dragContinued(m_dragStart, dragEnd);

    m_dragStart = dragEnd;
}


void PaintbrushCanvas::mousePressEvent(QMouseEvent *event) {
    if ((!m_isDragging) && (event->button() & Qt::LeftButton)) {
        m_isDragging = true;
        m_dragStart = scalePoint(event->pos());
        emit dragStarted(event->pos());
    }
}

void PaintbrushCanvas::mouseReleaseEvent(QMouseEvent *event) {
    emit clicked(scalePoint(event->pos()), event->button());

    if ((m_isDragging) && (event->button() & Qt::LeftButton)) {
        m_isDragging = false;
        emit dragEnded(scalePoint(event->pos()));
    }
}

void PaintbrushCanvas::wheelEvent(QWheelEvent *event) {
    QPoint intPos = QPoint { static_cast<int>(event->position().x()), static_cast<int>(event->position().y()) };
    emit wheelRolled(intPos, event->angleDelta().y(), event->modifiers());
}

QPoint PaintbrushCanvas::scalePoint(const QPoint &p) const {
    return QPoint {
        static_cast<int>(floor(p.x()/m_scaleFactor)), 
        static_cast<int>(floor(p.y()/m_scaleFactor))
    };
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

