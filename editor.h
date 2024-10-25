#ifndef EDITOR_H
#define EDITOR_H


#include "command.h"

#include <QColor>
#include <QPoint>
#include <QPixmap>


enum ToolType {
    ToolDraw,
    ToolErase,
};


class Editor
{
public:
    Editor(int width, int height);

    const QPixmap & buffer() { return m_currBuffer; };

    bool loadFile(const QString filename);
    bool saveFile(const QString filename);

    void undo();
    void redo();

    void setActiveColor(const QColor & color) {
        m_activeColor = color;
    }

    void setActiveTool(const ToolType newTool) {
        m_activeTool = newTool;
    }

    void startDrag();
    void endDrag();
    void dragLine(const QPoint start, const QPoint end);

protected:
    int m_width;
    int m_height;

    QPixmap m_initialBuffer;
    QPixmap m_currBuffer;

    QColor m_activeColor { Qt::black };
    ToolType m_activeTool { ToolType::ToolDraw };
    
    int m_cmdStackPos = 0;
    Command *m_currCommand = nullptr;
    std::vector<Command *> m_commandStack {};

    std::vector<QPair<QPoint, QPoint>> * m_currLines;

    void pushCurrentCommand();
    void restoreCommands();
    void performCommand(struct Command *cmd);
    void performPartialCommand(struct Command *cmd, const QPair<QPoint, QPoint> line);
    void pushPartialCommand(const QPair<QPoint, QPoint> line);
};

#endif // EDITOR_H
