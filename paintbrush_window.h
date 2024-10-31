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
#include <QPushButton>
#include <QSpinBox>


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
    QWidget *m_toolSettingsPanel;
    QPushButton *m_chooseColorButton;
    QSpinBox *m_chooseWidthControl;
    QLabel *m_widthThumbnail;

    QAction *m_toolColorChooserAction;
    QAction *m_toolWidthAction;
    QAction *m_undoAction;
    QAction *m_redoAction;

    QString m_windowTitle;

    void initActions();
    void initToolSettingsPanel();
    void initLayout();
    
    void openFile(QString filepath);
    void updateColorThumbnail(const QColor & color);
    void updateWidthThumbnail(int width);

private slots:
    //-------- from gui --------
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileExit();

    void onColorChosen(const QColor & color);
    void onWidthChosen(int width);

    //-------- from editor --------
    void onModifiedStatusChanged(bool isDocumentModified);
    void onCommandStackChanged(std::vector<std::unique_ptr<Command>> &stack, int currStackPos);

signals:
    void chooseTool(CommandType newCommandType);
};


#endif // PAINTBRUSH_WINDOW_H
