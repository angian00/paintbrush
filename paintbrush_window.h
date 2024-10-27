#ifndef PAINTBRUSH_WINDOW_H
#define PAINTBRUSH_WINDOW_H

#include "editor.h"
#include "paintbrush_canvas.h"

#include <QColorDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>


class PaintbrushWindow : public QMainWindow
{
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
        std::cout << "onEditUndo" << std::endl;
        m_editor->undo();
        m_canvas->update();
    }

    void onEditRedo() {
        std::cout << "onEditRedo" << std::endl;
        m_editor->redo();
        m_canvas->update();
    }


    void onToolColorChooser(const QColor & color) { m_colorChooser->show(); }
    void onToolDraw() { m_editor->setActiveTool(ToolType::ToolDraw); }
    void onToolErase() { m_editor->setActiveTool(ToolType::ToolErase); }

    void onColorChosen(const QColor &color) {
        m_editor->setActiveColor(color);
    }

    //-------- from editor --------
    void onModifiedStatusChanged(bool isDocumentModified) {
        auto fullWindowTitle = m_windowTitle;
        if (isDocumentModified)
            fullWindowTitle += "*";

        setWindowTitle(fullWindowTitle);
    }

    void onCommandStackChanged(std::vector<Command *> stack, int currStackPos) {
        m_undoAction->setEnabled( (currStackPos > 0) );
        m_redoAction->setEnabled( (currStackPos < (int)stack.size()) );
    }
};


#endif // PAINTBRUSH_WINDOW_H
