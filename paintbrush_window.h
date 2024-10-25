#ifndef PAINTBRUSH_WINDOW_H
#define PAINTBRUSH_WINDOW_H

#include "editor.h"
#include "paintbrush_canvas.h"

#include <QColorDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>


class PaintbrushWindow : public QMainWindow
{
public:
    explicit PaintbrushWindow();
        
private:
    Editor *m_editor;
    PaintbrushCanvas *m_canvas;
    QColorDialog *m_colorChooser;

private slots:
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
};


#endif // PAINTBRUSH_WINDOW_H
