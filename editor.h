#ifndef EDITOR_H
#define EDITOR_H


#include "command.h"

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QPixmap>
#include <QCursor>

#include <memory>



class Editor : public QObject
{
Q_OBJECT
public:
    Editor(int width, int height);

    const QPixmap & buffer() { return m_currBuffer; };

    const Command * activeCommand() const;

    void newFile();
    bool loadFile(const QString filename);
    bool saveFile(const QString filename);


    void performCurrentCommand();
    void performCurrentCommand(QPainter &painter);
    void paintToolCursor(QPainter &painter, QPoint &pos);

public slots:
    void onUndo();
    void onRedo();

    void onToolChosen(CommandType newCommandType);
    void onToolColorChosen(const QColor & color);
    void onToolWidthChosen(int width);

    void onDragStarted();
    void onDragEnded();
    void onDragContinued(const QPoint start, const QPoint end);


protected:
    int m_width;
    int m_height;

    QPixmap m_initialBuffer;
    QPixmap m_currBuffer;

    bool m_isModified;

    CommandType m_activeTool;
    
    int m_cmdStackPos = 0;
    std::unique_ptr<Command> m_currCommand = nullptr;
    std::vector<std::unique_ptr<Command>> m_cmdStack {};


    void setModified(bool edited=true);

    void reset(QPixmap &destBuffer, const QPixmap &srcBuffer);
    void pushCurrentCommand();
    void restoreCommandsFromStack();

signals:
    void somethingDrawn();
    
    void modifiedStatusChanged(bool isDocumentModified);
    void commandStackChanged(std::vector<std::unique_ptr<Command>> &stack, int currStackPos);
    void cursorChanged(const QCursor &cursor);
};

#endif // EDITOR_H
