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
    
protected:
    Editor *m_editor;

    bool m_isDragging { false };
    QPoint m_lineStart;


    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

};

#endif // PAINTBRUSH_CANVAS_H
