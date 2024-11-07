#ifndef EDITOR_H
#define EDITOR_H


#include "command.h"
#include "qpaintdevice.h"

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


    const QRect currentSelection() const { return m_currSelection; }
    void setCurrentSelection(QRect selection);

    void newFile();
    bool loadFile(const QString filename);
    bool saveFile(const QString filename);

    void zoomIn(int zoomFactor);
    void zoomOut(int zoomFactor);

    // void paintCurrentBuffer(QPaintDevice * target=nullptr);
    // void performCurrentCommand(QPaintDevice * target=nullptr);
    // void paintCustomCursor(QPoint &pos, QPaintDevice * target=nullptr);
    // void paintCurrentSelection(QPaintDevice * target=nullptr);
    void paintCurrentBuffer(QPainter * canvasPainter);
    void performPartialCommand(QPainter * canvasPainter);
    void paintCustomCursor(QPoint &pos, QWidget * canvas);
    void paintCurrentSelection(QWidget * canvas);

public slots:
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();

    void onSelectAll();
    void onSelectNone();

    void onToolChosen(CommandType newCommandType);
    void onToolColorChosen(const QColor & color);
    void onToolWidthChosen(int width);

    void onClicked(const QPoint pos, Qt::MouseButton button);
    void onDragStarted(const QPoint pos);
    void onDragEnded(const QPoint pos);
    void onDragContinued(const QPoint start, const QPoint end);
    void onWheelRolled(const QPoint pos, int delta, QFlags<Qt::KeyboardModifier> modifiers);


protected:
    int m_width;
    int m_height;

    QPixmap m_initialBuffer;
    QPixmap m_currBuffer;

    bool m_isModified;
    int m_zoomLevel;

    CommandType m_activeTool;
    QRect m_currSelection;
    
    int m_cmdStackPos = 0;
    std::unique_ptr<Command> m_currCommand = nullptr;
    std::vector<std::unique_ptr<Command>> m_cmdStack {};


    void setModified(bool edited=true);

    void resetDocument();
    void reset(QPixmap &destBuffer, const QPixmap &srcBuffer);
    void restoreCommandsFromStack();
    void pushCurrentCommand();
    void performCompleteCommand();
    void performCurrentCommand(QPainter * painter);

signals:
    void documentSizeChanged(QSize size);
    void zoomLevelChanged(int zoomLevel);
    void viewMoved(QPoint delta);
    void somethingDrawn();
    
    void modifiedStatusChanged(bool isDocumentModified);
    void commandStackChanged(std::vector<std::unique_ptr<Command>> &stack, int currStackPos);
    void cursorChanged(const QCursor &cursor);
    void selectionChanged(bool isSomethingSelected);
};


bool isClipboardValid();

#endif // EDITOR_H

