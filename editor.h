#ifndef EDITOR_H
#define EDITOR_H


#include "command.h"

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QPixmap>


const int defaultDrawWidth  = 2;
const int defaultEraseWidth = 10;


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

    const ToolData * activeTool() { return m_activeTool; }

    void setActiveColor(const QColor & color) {
        m_activeColor = color;
    }

    void onStartDrag();
    void onEndDrag();
    void onDrag(const QPoint start, const QPoint end);


public slots:
    void onToolChosen(ToolType newToolType);


protected:
    int m_width;
    int m_height;

    QPixmap m_initialBuffer;
    QPixmap m_currBuffer;

    bool m_isModified;

    QColor m_activeColor { Qt::black };
    //ToolType m_activeTool { ToolType::ToolDraw };
    ToolData * m_activeTool = new ToolDrawData { m_activeColor, defaultDrawWidth };
    
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
