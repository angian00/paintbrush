#ifndef PAINTBRUSH_CANVAS_H
#define PAINTBRUSH_CANVAS_H

#include "qnamespace.h"
#include <QWidget>

#include <vector>
#include <iostream>


enum ToolType {
    Draw,
    Erase,
};


class PaintbrushCanvas : public QWidget
{
Q_OBJECT
public:
    explicit PaintbrushCanvas(QWidget *parent = 0);
    
    void openFile();
    void saveFile();


public slots:
    void onToolDraw() { setActiveTool(ToolType::Draw); }
    void onToolErase() { setActiveTool(ToolType::Erase); }

    void setActiveColor(QColor color) {
        //std::cout << "Now using color: " << color << std::endl;
        m_activeColor = color;
    }

    void setActiveTool(ToolType newTool) {
        std::cout << "Now using tool: " << newTool << std::endl;
        m_activeTool = newTool;
    }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    std::vector<std::tuple<QPoint, QPoint>>m_lines {};
    bool m_isDragging { false };
    QPoint m_lineStart;

    QColor m_activeColor { Qt::black };
    ToolType m_activeTool { ToolType::Draw };
    QPixmap m_buffer;
};

#endif // PAINTBRUSH_CANVAS_H
