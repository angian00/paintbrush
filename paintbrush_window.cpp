#include "paintbrush_window.h"

#include <QApplication>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QColorDialog>

#include <iostream>



PaintbrushWindow::PaintbrushWindow() {
    setFixedSize(800, 600);

    m_canvas = new PaintbrushCanvas { this };

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



    auto toolColorChooserAction = new QAction("Choose Color", this);
    //newAction->setIcon();
    toolColorChooserAction->setToolTip("Choose color");

    auto toolDrawAction = new QAction("Draw", this);
    //newAction->setIcon();
    toolDrawAction->setToolTip("Draw tool");
    toolDrawAction->setShortcut(QKeySequence("D"));

    auto toolEraseAction = new QAction("Erase", this);
    //newAction->setIcon();
    toolEraseAction->setToolTip("Erase tool");
    toolEraseAction->setShortcut(QKeySequence("E"));


    auto menuBar = new QMenuBar {this};

    auto fileMenu = new QMenu { "File", this};
    menuBar->addMenu(fileMenu);

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);    
    fileMenu->addAction(saveAction);    
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    setMenuBar(menuBar);


    auto toolBar = new QToolBar(this);
    toolBar->addAction(toolColorChooserAction);
    toolBar->addAction(toolDrawAction);
    toolBar->addAction(toolEraseAction);
    toolBar->addSeparator();

    // Aggiunta della toolbar alla finestra
    addToolBar(Qt::LeftToolBarArea, toolBar);


    m_colorChooser = new QColorDialog(this);


    connect(newAction, &QAction::triggered, this, &PaintbrushWindow::onNew);
    connect(openAction, &QAction::triggered, this, &PaintbrushWindow::onOpen);
    connect(saveAction, &QAction::triggered, this, &PaintbrushWindow::onSave);
    connect(exitAction, &QAction::triggered, this, &PaintbrushWindow::onExit);

    connect(toolColorChooserAction, &QAction::triggered, this, &PaintbrushWindow::onToolColorChooser);
    connect(toolDrawAction, &QAction::triggered, m_canvas, &PaintbrushCanvas::onToolDraw);
    connect(toolEraseAction, &QAction::triggered, m_canvas, &PaintbrushCanvas::onToolErase);


    connect(m_colorChooser, SIGNAL(colorSelected(const QColor &)), m_canvas, SLOT(setActiveColor(QColor)));
}
