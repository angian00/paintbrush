#ifndef EDITOR_H
#define EDITOR_H


#include "command.h"

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QPixmap>
#include <memory>


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

    const Command * activeCommand() const;

    //FIXME: transform to slots
    void onStartDrag();
    void onEndDrag();
    void onDrag(const QPoint start, const QPoint end);
    //

    void performCurrentCommand();
    void performCurrentCommand(QPainter &painter);
    void paintToolCursor(QPainter &painter, QPoint &pos);

public slots:
    void onToolChosen(CommandType newCommandType);
    void onToolColorChosen(const QColor & color);
    void onToolWidthChosen(int width);



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
    void modifiedStatusChanged(bool isDocumentModified);
    void commandStackChanged(std::vector<std::unique_ptr<Command>> &stack, int currStackPos);
};

#endif // EDITOR_H
