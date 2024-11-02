#include "constants.h"
#include "paintbrush_window.h"
#include "command.h"
#include "editor.h"
#include "qnamespace.h"
#include "tool_config.h"
#include "paintbrush_canvas.h"

#include <QApplication>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QColorDialog>
#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QClipboard>

#include <iostream>


const int screenWidth = 800;
const int screenHeight = 600;

const int documentWidth = 800;
const int documentHeight = 600;


PaintbrushWindow::PaintbrushWindow() {
    setFixedSize(screenWidth, screenHeight);

    m_editor = new Editor { documentWidth, documentHeight };
    m_canvas = new PaintbrushCanvas { this, m_editor };

    m_colorChooser = new QColorDialog(this);
    initActions();
    initToolSettingsPanel();
    initLayout();
}


void PaintbrushWindow::initActions() {
    auto newAction = new QAction("New", this);
    newAction->setToolTip("New file");
    newAction->setShortcut(QKeySequence("Ctrl+N"));

    auto openAction = new QAction("Open", this);
    openAction->setShortcut(QKeySequence("Ctrl+O"));

    m_saveAction = new QAction("Save", this);
    m_saveAction->setShortcut(QKeySequence("Ctrl+S"));

    m_saveAsAction = new QAction("Save as...", this);
    m_saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));

    auto exitAction = new QAction("Exit", this);
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));


    m_undoAction = new QAction("Undo", this);
    m_undoAction->setShortcut(QKeySequence("Ctrl+Z"));

    m_redoAction = new QAction("Redo", this);
    m_redoAction->setShortcut(QKeySequence("Ctrl+Y"));

    m_cutAction = new QAction("Cut", this);
    m_cutAction->setShortcut(QKeySequence("Ctrl+X"));

    m_copyAction = new QAction("Copy", this);
    m_copyAction->setShortcut(QKeySequence("Ctrl+C"));

    m_pasteAction = new QAction("Paste", this);
    m_pasteAction->setShortcut(QKeySequence("Ctrl+V"));


    auto m_selectAllAction = new QAction("Select All", this);
    m_selectAllAction->setShortcut(QKeySequence("Ctrl+A"));

    auto m_selectNoneAction = new QAction("Select None", this);
    m_selectNoneAction->setShortcut(QKeySequence("Shift+Ctrl+A"));


    m_toolColorChooserAction = new QAction("Choose Color", this);
    m_toolColorChooserAction->setToolTip("Choose color");

    m_toolWidthAction = new QAction("Choose Width", this);
    m_toolWidthAction->setToolTip("Choose width");

    auto toolSelectAction = new QAction("Select", this);
    toolSelectAction->setIcon(QIcon("images/square3d-corner-to-corner.svg"));
    toolSelectAction->setShortcut(QKeySequence("S"));

    auto toolDrawAction = new QAction("Draw", this);
    toolDrawAction->setIcon(QIcon("images/design-pencil.svg"));
    toolDrawAction->setShortcut(QKeySequence("D"));

    auto toolEraseAction = new QAction("Erase", this);
    toolEraseAction->setIcon(QIcon("images/erase.svg"));
    toolEraseAction->setShortcut(QKeySequence("E"));


    //--------------------------- menu bar ---------------------------

    auto menuBar = new QMenuBar {this};
    setMenuBar(menuBar);

    auto fileMenu = new QMenu {"File", this};
    menuBar->addMenu(fileMenu);

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(m_saveAction);
    fileMenu->addAction(m_saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);


    auto editMenu = new QMenu {"Edit", this};
    menuBar->addMenu(editMenu);

    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
    editMenu->addAction(m_cutAction);
    editMenu->addAction(m_copyAction);
    editMenu->addAction(m_pasteAction);

    auto selectMenu = new QMenu {"Select", this};
    menuBar->addMenu(selectMenu);

    selectMenu->addAction(m_selectAllAction);
    selectMenu->addAction(m_selectNoneAction);    

    //--------------------------- tool bar ---------------------------

    auto toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(toolThumbnailSize, toolThumbnailSize));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolBar);

    toolBar->addAction(toolSelectAction);
    toolBar->addAction(toolDrawAction);
    toolBar->addAction(toolEraseAction);

    toolBar->addSeparator();

    //--------------------- connect slots from ui input ---------------------
    connect(newAction,  &QAction::triggered, this, &PaintbrushWindow::onFileNew);
    connect(openAction, &QAction::triggered, this, &PaintbrushWindow::onFileOpen);
    connect(m_saveAction, &QAction::triggered, this, &PaintbrushWindow::onFileSave);
    connect(m_saveAsAction, &QAction::triggered, this, &PaintbrushWindow::onFileSaveAs);
    connect(exitAction, &QAction::triggered, this, &PaintbrushWindow::onFileExit);

    connect(m_undoAction, &QAction::triggered, m_editor, &Editor::onUndo);
    connect(m_redoAction, &QAction::triggered, m_editor, &Editor::onRedo);
    connect(m_cutAction, &QAction::triggered, m_editor, &Editor::onCut);
    connect(m_copyAction, &QAction::triggered, m_editor, &Editor::onCopy);
    connect(m_pasteAction, &QAction::triggered, m_editor, &Editor::onPaste);
    
    connect(m_selectAllAction, &QAction::triggered, m_editor, &Editor::onSelectAll);
    connect(m_selectNoneAction, &QAction::triggered, m_editor, &Editor::onSelectNone);
    
    connect(toolSelectAction,       &QAction::triggered, this, [=]() { chooseTool(CommandType::Select); });
    connect(toolDrawAction,         &QAction::triggered, this, [=]() { chooseTool(CommandType::Draw); });
    connect(toolEraseAction,        &QAction::triggered, this, [=]() { chooseTool(CommandType::Erase); });

    connect(m_toolColorChooserAction, &QAction::triggered, this, [=]() { m_colorChooser->show(); });
    connect(m_colorChooser, &QColorDialog::colorSelected, this, &PaintbrushWindow::onColorChosen);
    connect(m_toolWidthAction, &QAction::triggered, this, &PaintbrushWindow::onWidthChosen);

    //--------------------- connect slots from external signals ---------------------
    connect(QGuiApplication::clipboard(), &QClipboard::changed, this, &PaintbrushWindow::onClipboardChanged);

    //--------------------- connect slots from editor ---------------------
    connect(m_editor, &Editor::modifiedStatusChanged, this, &PaintbrushWindow::onModifiedStatusChanged);
    connect(m_editor, &Editor::commandStackChanged, this, &PaintbrushWindow::onCommandStackChanged);
    connect(m_editor, &Editor::selectionChanged, this, &PaintbrushWindow::onSelectionChanged);

    //--------------------- connect signals from this ---------------------
    connect(this, &PaintbrushWindow::chooseTool, m_editor, &Editor::onToolChosen);
    connect(m_editor, &Editor::cursorChanged, m_canvas, &PaintbrushCanvas::setCursor);


    //--------------------- connect signals/slots between other components ---------------------
    connect(m_editor, &Editor::somethingDrawn, m_canvas, &PaintbrushCanvas::onSomethingDrawn);
    connect(m_canvas, &PaintbrushCanvas::dragStarted, m_editor, &Editor::onDragStarted);
    connect(m_canvas, &PaintbrushCanvas::dragEnded, m_editor, &Editor::onDragEnded);
    connect(m_canvas, &PaintbrushCanvas::dragContinued, m_editor, &Editor::onDragContinued);
}


void PaintbrushWindow::initToolSettingsPanel() {
    m_toolSettingsPanel = new QWidget(this);
    auto toolSettingsPanelLayout = new QHBoxLayout(m_toolSettingsPanel);

    m_chooseColorButton = new QPushButton(this);
    m_chooseColorButton->setFlat(true);
    m_chooseColorButton->setIconSize(QSize {toolThumbnailSize, toolThumbnailSize});
    connect(m_chooseColorButton, &QPushButton::clicked, this, [=](bool _) { m_colorChooser->show(); });

    m_widthThumbnail = new QLabel(this);

    m_chooseWidthControl = new QSpinBox(this);
    m_chooseWidthControl->setMinimum(1);
    m_chooseWidthControl->setMaximum(maxDrawWidth);
    connect(m_chooseWidthControl, QOverload<int>::of(&QSpinBox::valueChanged), this, &PaintbrushWindow::onWidthChosen);
    
    toolSettingsPanelLayout->addWidget(m_chooseColorButton);
    toolSettingsPanelLayout->addSpacing(1);
    toolSettingsPanelLayout->addWidget(m_widthThumbnail);
    toolSettingsPanelLayout->addWidget(m_chooseWidthControl);
    toolSettingsPanelLayout->addStretch();
}

void PaintbrushWindow::initLayout() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(m_toolSettingsPanel);
    mainLayout->addWidget(m_canvas);
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

    onColorChosen(defaultDrawColor);
    onWidthChosen(defaultDrawWidth);
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

    m_filepath = filepath;
    m_windowTitle = QFileInfo(filepath).fileName();
    setWindowTitle(m_windowTitle);

    onColorChosen(defaultDrawColor);
    onWidthChosen(defaultDrawWidth);
}

void PaintbrushWindow::onFileSave() {
    if (m_filepath.isEmpty())
        onFileSaveAs();
    else
        saveFile(m_filepath);
}

void PaintbrushWindow::onFileSaveAs() {
    QString filepath = QFileDialog::getSaveFileName(this, "Save file");
    saveFile(filepath);

}

void PaintbrushWindow::saveFile(QString filepath) {
    if (filepath.isEmpty())
        return;

    if (!m_editor->saveFile(filepath)) {
        QMessageBox::warning(this, "Warning", "Cannot save file " + filepath);
        return;
    }

    m_filepath = filepath;
    m_windowTitle = QFileInfo(filepath).fileName();
    setWindowTitle(m_windowTitle);

    m_canvas->update();
}


void PaintbrushWindow::onFileExit() {
    close();
}


void PaintbrushWindow::onColorChosen(const QColor & color) {
    updateColorThumbnail(color);
    m_editor->onToolColorChosen(color);

    //when you choose a color, draw tool is also selected
    chooseTool(CommandType::Draw);
}

void PaintbrushWindow::onWidthChosen(int width) {
    if (m_chooseWidthControl->value() != width)
        m_chooseWidthControl->setValue(width);
    
    updateWidthThumbnail(width);
    m_editor->onToolWidthChosen(width);
}


void PaintbrushWindow::onModifiedStatusChanged(bool isDocumentModified) {
    auto fullWindowTitle = m_windowTitle;
    if (isDocumentModified)
        fullWindowTitle += "*";
    
    m_saveAction->setEnabled(isDocumentModified);
    m_saveAsAction->setEnabled(isDocumentModified);
    
    setWindowTitle(fullWindowTitle);
}

void PaintbrushWindow::onCommandStackChanged(std::vector<std::unique_ptr<Command>> &stack, int currStackPos) {
    m_undoAction->setEnabled( (currStackPos > 0) );
    m_redoAction->setEnabled( (currStackPos < (int)stack.size()) );
}


void PaintbrushWindow::onSelectionChanged(bool isSomethingSelected) {
    m_copyAction->setEnabled(isSomethingSelected);
    m_cutAction->setEnabled(isSomethingSelected);
}

void PaintbrushWindow::onClipboardChanged(QClipboard::Mode targetMode) {
    if (targetMode == QClipboard::Clipboard)
        m_pasteAction->setEnabled(isClipboardValid());
}


void PaintbrushWindow::updateColorThumbnail(const QColor & color) {
    QPixmap thumbnail {toolThumbnailSize, toolThumbnailSize};
    thumbnail.fill(color);

    m_toolColorChooserAction->setIcon(QIcon(thumbnail));
    m_chooseColorButton->setIcon(QIcon(thumbnail));
}

void PaintbrushWindow::updateWidthThumbnail(int width) {
    QPixmap thumbnail {toolThumbnailSize, toolThumbnailSize};
    thumbnail.fill(bkgColor);

    QPoint lineWidthFrom { toolThumbnailSize/4, toolThumbnailSize/2 };
    QPoint lineWidthTo { toolThumbnailSize*3/4, toolThumbnailSize/2 };

    QPainter painter{&thumbnail};
    painter.setPen(QPen(defaultDrawColor, width));
    painter.drawLine(lineWidthFrom, lineWidthTo);
    
    m_toolWidthAction->setIcon(QIcon(thumbnail));
    m_widthThumbnail->setPixmap(thumbnail);
}