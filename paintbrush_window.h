#ifndef PAINTBRUSH_WINDOW_H
#define PAINTBRUSH_WINDOW_H

#include "command.h"
#include "editor.h"
#include "paintbrush_canvas.h"

#include <QColorDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>


class PaintbrushWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit PaintbrushWindow();
    void start(const char *filename=nullptr);

private:
    Editor *m_editor;
    PaintbrushCanvas *m_canvas;
    QColorDialog *m_colorChooser;

    QAction *m_undoAction;
    QAction *m_redoAction;

    QString m_windowTitle;

    void openFile(QString filepath);

private slots:
    //-------- from gui --------
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileExit();

    void onEditUndo() {
        //std::cout << "onEditUndo" << std::endl;
        m_editor->undo();
        m_canvas->update();
    }

    void onEditRedo() {
        //std::cout << "onEditRedo" << std::endl;
        m_editor->redo();
        m_canvas->update();
    }


    void onColorChosen(const QColor &color) {
        m_editor->onToolColorChosen(color);

        //when you choose a color, draw tool is also selected
        chooseTool(CommandType::Draw);
    }


    //-------- from editor --------
    void onModifiedStatusChanged(bool isDocumentModified);
    void onCommandStackChanged(std::vector<std::unique_ptr<Command>> &stack, int currStackPos);

signals:
    void chooseTool(CommandType newCommandType);
};


#endif // PAINTBRUSH_WINDOW_H
