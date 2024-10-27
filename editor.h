#ifndef EDITOR_H
#define EDITOR_H


#include "command.h"

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QPixmap>


enum ToolType {
    ToolDraw,
    ToolErase,
};


class Editor : public QObject
{
Q_OBJECT
public:
    Editor(int width, int height);

    const QPixmap & buffer() { return m_currBuffer; };

    void newFile();
    bool loadFile(const QString filename);
    bool saveFile(const QString filename);

    void undo();
    void redo();

    void setActiveColor(const QColor & color) {
        m_activeColor = color;
        //when you choose a color draw tool is also selected
        m_activeTool = ToolDraw;
    }

    void setActiveTool(const ToolType newTool) {
        m_activeTool = newTool;
    }

    void onStartDrag();
    void onEndDrag();
    void onDragLine(const QPoint start, const QPoint end);


protected:
    int m_width;
    int m_height;

    QPixmap m_initialBuffer;
    QPixmap m_currBuffer;

    bool m_isModified;

    QColor m_activeColor { Qt::black };
    ToolType m_activeTool { ToolType::ToolDraw };
    
    int m_cmdStackPos = 0;
    Command *m_currCommand = nullptr;
    std::vector<Command *> m_cmdStack {};


    void setModified(bool edited=true);

    void reset(QPixmap &destBuffer, const QPixmap &srcBuffer);
    void pushCurrentCommand();
    void restoreCommandsFromStack();
    void performCommand(struct Command *cmd);
    void performPartialCommand(struct Command *cmd, const QPair<QPoint, QPoint> line);
    void updateCurrentCommand(const QPair<QPoint, QPoint> line);

signals:
    void modifiedStatusChanged(bool isDocumentModified);
    void commandStackChanged(std::vector<Command *> stack, int currStackPos);
};

#endif // EDITOR_H
