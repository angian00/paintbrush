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
#include <QTransform>
#include <QColor>
#include <QSize>
#include <QScrollBar>

#include <math.h>


void paintBackgroundPattern(QWidget * target);


PaintbrushCanvas::PaintbrushCanvas(QWidget *parent, QScrollArea *scrollArea, Editor *editor) : 
    QWidget(parent), m_scrollArea(scrollArea), m_editor(editor), m_documentSize(QSize()), m_zoomLevel(1.0) {

    setFixedSize(m_documentSize);
    setMouseTracking(true);
}

void PaintbrushCanvas::onDocumentSizeChanged(QSize size) {
    std::cout << "onDocumentSizeChanged " << std::endl;
    m_documentSize = size;
    updateSizeAndPos(QPoint { -1, -1 });

}

void PaintbrushCanvas::onZoomLevelChanged(double zoomLevel, const QPoint & zoomPos) {
    std::cout << "onZoomLevelChanged: " << zoomLevel << std::endl;
    m_zoomLevel = zoomLevel;
    updateSizeAndPos(zoomPos);
}

void PaintbrushCanvas::paintEvent(QPaintEvent * _) {
    paintBackgroundPattern(this);

    QPainter painter { this };
    painter.setRenderHints(QPainter::Antialiasing);

    QTransform transform = QTransform::fromScale(m_zoomLevel, m_zoomLevel);
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
        static_cast<int>(floor(p.x()/m_zoomLevel)), 
        static_cast<int>(floor(p.y()/m_zoomLevel))
    };
}


QRect PaintbrushCanvas::getVisibleArea() const {
    auto viewportRect = m_scrollArea->viewport()->rect();

    //QRect visibleRect = mapFromParent(viewportRect.translated(m_scrollArea->viewport()->mapTo(this, QPoint(0, 0))));
    auto topLeft = mapFromParent(viewportRect.topLeft());
    auto bottomRight = mapFromParent(viewportRect.bottomRight());
    auto visibleRect = QRect { topLeft, bottomRight };

    return visibleRect;
}

void PaintbrushCanvas::updateSizeAndPos(const QPoint & zoomPos) {
    auto canvasSize = m_documentSize * m_zoomLevel;
    std::cout << "PaintbrushCanvas::updateSizeAndPos " << std::endl;
    std::cout << "setting PaintbrushCanvas size to: " << canvasSize.width() << ", " << canvasSize.height() << std::endl;
    setFixedSize(canvasSize);

    update();
    
    QPoint newCenter;

    if (zoomPos.x() < 0) {
        newCenter = QPoint { canvasSize.width() / 2, canvasSize.height() / 2 };
    } else {
        newCenter = zoomPos + m_zoomLevel * (m_viewCenter - zoomPos);
        //newCenter = zoomPos / m_zoomLevel + (m_viewCenter - zoomPos);
    }

    std::cout << "setting newCenter to: " << newCenter.x() << ", " << newCenter.y() << std::endl;
    moveViewToCenter(newCenter);
}

void PaintbrushCanvas::moveViewToCenter(const QPoint &newCenter) {
    auto viewport = m_scrollArea->viewport();

    auto hBar = m_scrollArea->horizontalScrollBar();
    std::cout << "setting m_scrollArea; hBar->minimum: " << hBar->minimum() << ", hBar->maximum: " << hBar->maximum() << std::endl;
    auto newHorValue = newCenter.x() - viewport->width()  / 2;
    newHorValue = std::max(hBar->minimum(), newHorValue);
    newHorValue = std::min(hBar->maximum(), newHorValue);
    hBar->setValue(newHorValue);

    auto vBar = m_scrollArea->verticalScrollBar();
    auto newVerValue = newCenter.y() - viewport->height() / 2;
    newVerValue = std::max(vBar->minimum(), newVerValue);
    newVerValue = std::min(vBar->maximum(), newVerValue);

    hBar->setValue(newHorValue);
    vBar->setValue(newVerValue);
    std::cout << "newHorValue: " << newHorValue << ", newVerValue: " << newVerValue << std::endl;

    m_viewCenter = newCenter;
}


void PaintbrushCanvas::onViewMovedBy(QPoint delta) {
    auto hBar = m_scrollArea->horizontalScrollBar();
    auto newHorValue = hBar->value() + delta.x();
    newHorValue = std::max(hBar->minimum(), newHorValue);
    newHorValue = std::min(hBar->maximum(), newHorValue);
    hBar->setValue(newHorValue);

    auto vBar = m_scrollArea->verticalScrollBar();
    auto newVerValue = vBar->value() + delta.y();
    newVerValue = std::max(vBar->minimum(), newVerValue);
    newVerValue = std::min(vBar->maximum(), newVerValue);
    vBar->setValue(newVerValue);
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

