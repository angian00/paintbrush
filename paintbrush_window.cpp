#include "paintbrush_window.h"
#include "editor.h"
#include "paintbrush_canvas.h"
#include "qcolordialog.h"

#include <QApplication>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QColorDialog>

#include <iostream>


const int screenWidth = 800;
const int screenHeight = 600;

const int documentWidth = 800;
const int documentHeight = 600;


PaintbrushWindow::PaintbrushWindow() {
    setFixedSize(screenWidth, screenHeight);

    m_editor = new Editor { documentWidth, documentHeight };
    m_canvas = new PaintbrushCanvas { this, m_editor };


    auto newAction = new QAction("New", this);
    newAction->setToolTip("New file");
    newAction->setShortcut(QKeySequence("Ctrl+N"));

    auto openAction = new QAction("Open", this);
    openAction->setToolTip("Open file");
    openAction->setShortcut(QKeySequence("Ctrl+O"));

    auto saveAction = new QAction("Save", this);
    saveAction->setToolTip("Save file");
    saveAction->setShortcut(QKeySequence("Ctrl+S"));

    auto exitAction = new QAction("Exit", this);
    exitAction->setToolTip("Exit application");
    exitAction->setShortcut(QKeySequence("Ctrl+X"));


    m_undoAction = new QAction("Undo", this);
    m_undoAction->setToolTip("Undo previous action");
    m_undoAction->setShortcut(QKeySequence("Ctrl+Z"));

    m_redoAction = new QAction("Redo", this);
    m_redoAction->setToolTip("Redo previous action");
    m_redoAction->setShortcut(QKeySequence("Ctrl+Y"));


    auto toolColorChooserAction = new QAction("Choose Color", this);
    toolColorChooserAction->setIcon(QIcon("images/color-filter.svg"));
    toolColorChooserAction->setToolTip("Choose color");

    auto toolSelectAction = new QAction("Select", this);
    toolSelectAction->setIcon(QIcon("images/square3d-corner-to-corner.svg"));
    toolSelectAction->setToolTip("Select tool");
    toolSelectAction->setShortcut(QKeySequence("S"));

    auto toolDrawAction = new QAction("Draw", this);
    toolDrawAction->setIcon(QIcon("images/design-pencil.svg"));
    toolDrawAction->setToolTip("Draw tool");
    toolDrawAction->setShortcut(QKeySequence("D"));

    auto toolEraseAction = new QAction("Erase", this);
    toolEraseAction->setIcon(QIcon("images/erase.svg"));
    toolEraseAction->setToolTip("Erase tool");
    toolEraseAction->setShortcut(QKeySequence("E"));


    //--------------------------- menu bar ---------------------------

    auto menuBar = new QMenuBar {this};
    setMenuBar(menuBar);

    auto fileMenu = new QMenu { "File", this};
    menuBar->addMenu(fileMenu);

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);    
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);


    auto editMenu = new QMenu { "Edit", this};
    menuBar->addMenu(editMenu);

    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);    

    //--------------------------- tool bar ---------------------------

    auto toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(32, 32));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolBar);

    toolBar->addAction(toolSelectAction);
    toolBar->addAction(toolColorChooserAction);
    toolBar->addAction(toolDrawAction);
    toolBar->addAction(toolEraseAction);
    toolBar->addSeparator();


    m_colorChooser = new QColorDialog(this);

    //--------------------- connect slots from ui input ---------------------

    connect(newAction,  &QAction::triggered, this, &PaintbrushWindow::onFileNew);
    connect(openAction, &QAction::triggered, this, &PaintbrushWindow::onFileOpen);
    connect(saveAction, &QAction::triggered, this, &PaintbrushWindow::onFileSave);
    connect(exitAction, &QAction::triggered, this, &PaintbrushWindow::onFileExit);

    connect(m_undoAction, &QAction::triggered, this, &PaintbrushWindow::onEditUndo);
    connect(m_redoAction, &QAction::triggered, this, &PaintbrushWindow::onEditRedo);
    
    connect(toolSelectAction,       &QAction::triggered, this, [=]() { chooseTool(ToolSelect); });
    connect(toolDrawAction,         &QAction::triggered, this, [=]() { chooseTool(ToolDraw); });
    connect(toolEraseAction,        &QAction::triggered, this, [=]() { chooseTool(ToolErase); });

    connect(toolColorChooserAction, &QAction::triggered, this, [=]() { m_colorChooser->show(); });
    connect(m_colorChooser, &QColorDialog::colorSelected, this, &PaintbrushWindow::onColorChosen);

    //--------------------- connect slots from editor ---------------------
    connect(m_editor, &Editor::modifiedStatusChanged, this, &PaintbrushWindow::onModifiedStatusChanged);
    connect(m_editor, &Editor::commandStackChanged, this, &PaintbrushWindow::onCommandStackChanged);

    //--------------------- connect signals from this ---------------------
    connect(this, &PaintbrushWindow::chooseTool, m_editor, &Editor::onToolChosen);
    connect(this, &PaintbrushWindow::chooseTool, m_canvas, &PaintbrushCanvas::onToolChosen);

    //--------------------- startup ---------------------
}

void PaintbrushWindow::start(const char *cmdLineArg) {
    onFileNew();
    
    if (cmdLineArg == nullptr) {
        show();
        return;
    }

    openFile(cmdLineArg);
    show();
}

void PaintbrushWindow::onFileNew() {
    m_editor->newFile();
    m_windowTitle = "Untitled";
    setWindowTitle(m_windowTitle);

    chooseTool(ToolDraw);
    m_canvas->update();
}

void PaintbrushWindow::onFileOpen() {
    openFile("");
}

void PaintbrushWindow::openFile(QString filepath) {
    if (filepath.isEmpty())
        filepath = QFileDialog::getOpenFileName(this, "Open file");

    if (!m_editor->loadFile(filepath)) {
        QMessageBox::warning(this, "Warning", "Cannot open file " + filepath);
        return;
    }

    m_windowTitle = QFileInfo(filepath).fileName();
    setWindowTitle(m_windowTitle);

    chooseTool(ToolDraw);
    m_canvas->update();
}

void PaintbrushWindow::onFileSave() {
    QString filepath = QFileDialog::getSaveFileName(this, "Save file");
    if (filepath.isEmpty())
        return;

    if (!m_editor->saveFile(filepath)) {
        QMessageBox::warning(this, "Warning", "Cannot save file " + filepath);
        return;
    }

    m_windowTitle = QFileInfo(filepath).fileName();
    setWindowTitle(m_windowTitle);

    m_canvas->update();
}

void PaintbrushWindow::onFileExit() {
    close();
}


void PaintbrushWindow::onModifiedStatusChanged(bool isDocumentModified) {
    auto fullWindowTitle = m_windowTitle;
    if (isDocumentModified)
        fullWindowTitle += "*";

    setWindowTitle(fullWindowTitle);
}

void PaintbrushWindow::onCommandStackChanged(std::vector<Command *> stack, int currStackPos) {
    m_undoAction->setEnabled( (currStackPos > 0) );
    m_redoAction->setEnabled( (currStackPos < (int)stack.size()) );
}
