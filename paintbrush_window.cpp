#include "paintbrush_window.h"
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


    auto undoAction = new QAction("Undo", this);
    undoAction->setToolTip("Undo previous action");
    undoAction->setShortcut(QKeySequence("Ctrl+Z"));

    auto redoAction = new QAction("Redo", this);
    redoAction->setToolTip("Redo previous action");
    redoAction->setShortcut(QKeySequence("Ctrl+Y"));


    auto toolColorChooserAction = new QAction("Choose Color", this);
    toolColorChooserAction->setIcon(QIcon("images/color-filter.svg"));
    toolColorChooserAction->setToolTip("Choose color");

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

    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);    

    //--------------------------- tool bar ---------------------------

    auto toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(32, 32));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolBar);

    toolBar->addAction(toolColorChooserAction);
    toolBar->addAction(toolDrawAction);
    toolBar->addAction(toolEraseAction);
    toolBar->addSeparator();


    m_colorChooser = new QColorDialog(this);

    //--------------------- connect signals/slots ---------------------

    connect(newAction,  &QAction::triggered, this, &PaintbrushWindow::onFileNew);
    connect(openAction, &QAction::triggered, this, &PaintbrushWindow::onFileOpen);
    connect(saveAction, &QAction::triggered, this, &PaintbrushWindow::onFileSave);
    connect(exitAction, &QAction::triggered, this, &PaintbrushWindow::onFileExit);

    connect(undoAction, &QAction::triggered, this, &PaintbrushWindow::onEditUndo);
    connect(redoAction, &QAction::triggered, this, &PaintbrushWindow::onEditRedo);
    
    connect(toolColorChooserAction, &QAction::triggered, this, &PaintbrushWindow::onToolColorChooser);
    connect(toolDrawAction,         &QAction::triggered, this, &PaintbrushWindow::onToolDraw);
    connect(toolEraseAction,        &QAction::triggered, this, &PaintbrushWindow::onToolErase);

    connect(m_colorChooser, &QColorDialog::colorSelected, this, &PaintbrushWindow::onColorChosen);
}


void PaintbrushWindow::onFileNew() {
    QMessageBox::information(this, "New", "TODO: onNew");
}

void PaintbrushWindow::onFileOpen() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open file");
    if (fileName.isEmpty())
        return;

    if (!m_editor->loadFile(fileName)) {
        QMessageBox::warning(this, "Warning", "Cannot open file " + fileName);
        return;
    }
    setWindowTitle(fileName);

    //update();
}

void PaintbrushWindow::onFileSave() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save file");
    if (fileName.isEmpty())
        return;

    if (!m_editor->saveFile(fileName)) {
        QMessageBox::warning(this, "Warning", "Cannot save file " + fileName);
        return;
    }
}

void PaintbrushWindow::onFileExit() {
    close();
}