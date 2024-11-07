#ifndef PAINTBRUSH_CANVAS_H
#define PAINTBRUSH_CANVAS_H

#include "editor.h"
#include "qnamespace.h"

#include <QPixmap>
#include <QWidget>

#include <vector>
#include <iostream>



class PaintbrushCanvas : public QWidget
{
Q_OBJECT
public:
    explicit PaintbrushCanvas(QWidget *parent, Editor *editor);
    
public slots:
    void onDocumentSizeChanged(QSize size);
    void onZoomLevelChanged(int zoomLevel);
    void onSomethingDrawn() { update(); }

protected:
    Editor *m_editor;

    QSize m_documentSize;


    bool m_isDragging { false };
    QPoint m_dragStart;
    QPoint m_currMousePos;
    double m_scaleFactor;


    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void updateCanvasSize();
    QPoint scalePoint(const QPoint & p) const;

signals:
    void clicked(QPoint pos, Qt::MouseButton button);
    void dragStarted(QPoint pos);
    void dragEnded(QPoint pos);
    void dragContinued(const QPoint start, const QPoint end);
};

#endif // PAINTBRUSH_CANVAS_H
