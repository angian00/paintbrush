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
    void onFileNew() {
        QMessageBox::information(this, "New", "TODO: onNew");
    }

    void onFileOpen();
    void onFileSave();
    void onFileExit();

    void onToolColorChooser(const QColor & color) { m_colorChooser->show(); }
    void onToolDraw() { m_editor->setActiveTool(ToolType::Draw); }
    void onToolErase() { m_editor->setActiveTool(ToolType::Erase); }

    void onColorChosen(const QColor &color) {
        m_editor->setActiveColor(color);
    }
};


#endif // PAINTBRUSH_WINDOW_H
