#ifndef PAINTBRUSH_WINDOW_H
#define PAINTBRUSH_WINDOW_H

#include "command.h"
#include "editor.h"
#include "paintbrush_canvas.h"
#include "paintbrush_scroll_area.h"

#include <QColorDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
#include <QPushButton>
#include <QSpinBox>
#include <QClipboard>

class PaintbrushWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit PaintbrushWindow();
    void start(const char *filename=nullptr);

private:
    Editor *m_editor;
    PaintbrushCanvas *m_canvas;
    PaintbrushScrollArea *m_scrollArea;

    QColorDialog *m_colorChooser;
    QWidget *m_toolSettingsPanel;
    QPushButton *m_chooseColorButton;
    QSpinBox *m_chooseWidthControl;
    QLabel *m_widthThumbnail;

    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_toolColorChooserAction;
    QAction *m_toolWidthAction;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;

    QString m_windowTitle;
    QString m_filepath;


    void initActions();
    void initToolSettingsPanel();
    void initLayout();
    
    void openFile(QString filepath);
    void saveFile(QString filepath);

    void updateColorThumbnail(const QColor & color);
    void updateWidthThumbnail(int width);

private slots:
    //-------- from gui --------
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileExit();

    void onColorChosen(const QColor & color);
    void onWidthChosen(int width);

    //-------- from editor --------
    void onModifiedStatusChanged(bool isDocumentModified);
    void onCommandStackChanged(std::vector<std::unique_ptr<Command>> &stack, int currStackPos);
    void onSelectionChanged(bool isSomethingSelected);
    void onClipboardChanged(QClipboard::Mode targetMode);

signals:
    void chooseTool(CommandType newCommandType);
};


#endif // PAINTBRUSH_WINDOW_H
