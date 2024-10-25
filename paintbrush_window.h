#ifndef PAINTBRUSH_WINDOW_H
#define PAINTBRUSH_WINDOW_H

#include "paintbrush_canvas.h"

#include <QColorDialog>
#include <QMainWindow>
#include <QMessageBox>


class PaintbrushWindow : public QMainWindow
{
public:
    explicit PaintbrushWindow();
        
private:
    PaintbrushCanvas *m_canvas;
    QColorDialog *m_colorChooser;

private slots:
    void onNew() {
        QMessageBox::information(this, "New", "TODO: onNew");
    }

    void onOpen() {
        m_canvas->openFile();
    }

    void onSave() {
        m_canvas->saveFile();
    }

    void onExit() {
        close();
    }

    void onToolColorChooser() {
        m_colorChooser->show();
    }
};


#endif // PAINTBRUSH_WINDOW_H
