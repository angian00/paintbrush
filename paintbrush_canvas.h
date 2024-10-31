#ifndef PAINTBRUSH_CANVAS_H
#define PAINTBRUSH_CANVAS_H

#include "editor.h"

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
    void onSomethingDrawn() { update(); }

protected:
    Editor *m_editor;

    bool m_isDragging { false };
    QPoint m_dragStart;
    QPoint m_currMousePos;


    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    //void paintToolCursor(QPainter &painter, const Command *currCommand);

signals:
    void dragStarted(QPoint pos);
    void dragEnded(QPoint pos);
    void dragContinued(const QPoint start, const QPoint end);
};

#endif // PAINTBRUSH_CANVAS_H
