#ifndef EDITOR_H
#define EDITOR_H

#include <QColor>
#include <QPoint>
#include <QPixmap>


enum ToolType {
    Draw,
    Erase,
};


class Editor
{
public:
    Editor(int width, int height);

    const QPixmap & buffer() { return m_buffer; };

    bool loadFile(const QString filename);
    bool saveFile(const QString filename);

    void setActiveColor(const QColor & color) {
        m_activeColor = color;
    }

    void setActiveTool(const ToolType newTool) {
        m_activeTool = newTool;
    }

    void dragLine(const QPoint start, const QPoint end);

protected:
    int m_width;
    int m_height;

    QColor m_activeColor { Qt::black };
    ToolType m_activeTool { ToolType::Draw };
    QPixmap m_buffer;

    std::vector<QPair<QPoint, QPoint>> m_lines {};
};

#endif // EDITOR_H
