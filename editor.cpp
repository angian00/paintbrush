#include "editor.h"

#include "constants.h"
#include "command.h"
#include "qpoint.h"
#include "tool_config.h"
#include "paintbrush_window.h"

#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QGuiApplication>
#include <QClipboard>

#include <iostream>
#include <memory>




Editor::Editor(int width, int height): 
    m_width(width), m_height(height), m_zoomLevel(1.0) {
    
    m_initialBuffer = QPixmap(m_width, m_height);
    m_initialBuffer.fill(bkgColor);

    m_currBuffer = QPixmap(m_width, m_height);
    m_currBuffer.fill(bkgColor);

    m_isModified = false;
}


void Editor::setCurrentSelection(QRect selection) {
    m_currSelection = selection;
    emit somethingDrawn();
    emit selectionChanged(!m_currSelection.isEmpty());
}

bool isClipboardValid() {
    auto clipboardData = QGuiApplication::clipboard()->image();
    return (!clipboardData.isNull());
}


void Editor::newFile() {
    m_initialBuffer = QPixmap(m_width, m_height);
    m_initialBuffer.fill(bkgColor);

    resetDocument();
}

bool Editor::loadFile(const QString filename) {
    bool isLoadOk = m_initialBuffer.load(filename);
    if (!isLoadOk)
        return false;

    resetDocument();

    emit somethingDrawn();

    return true;
}

bool Editor::saveFile(const QString filename) {
    bool isSaveOk = m_currBuffer.save(filename);
    if (!isSaveOk)
        return false;

    reset(m_initialBuffer, m_currBuffer);
    setModified(false);
    emit somethingDrawn();

    return true;
}

void Editor::zoom(double zoomFactor, const QPoint & zoomPos) {
    std::cout << "Editor::zoom" << std::endl;

    m_zoomLevel *= zoomFactor;
    m_zoomLevel = std::min(maxZoomLevel, m_zoomLevel);
    m_zoomLevel = std::max(minZoomLevel, m_zoomLevel);
    emit zoomLevelChanged(m_zoomLevel, zoomPos);
    std::cout << "after Editor::zoom" << std::endl;
}


void Editor::onUndo() {
    assert(m_cmdStackPos >= 0);

    //std::cout << "before undo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    if (m_cmdStackPos == 0)
        return;

    m_cmdStackPos --;

    restoreCommandsFromStack();
    //std::cout << "after undo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    emit commandStackChanged(m_cmdStack, m_cmdStackPos);
}

void Editor::onRedo() {
    //std::cout << "before redo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    assert(m_cmdStackPos <= (int)m_cmdStack.size());

    if (m_cmdStackPos == (int)m_cmdStack.size())
        return;

    m_cmdStackPos ++;

    restoreCommandsFromStack();
    //std::cout << "after redo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    emit commandStackChanged(m_cmdStack, m_cmdStackPos);
}

void Editor::onCut() {
    if (m_currSelection.isEmpty())
        return;

    assert(m_currCommand == nullptr);

    QPixmap clipboardData = m_currBuffer.copy(m_currSelection);
    QGuiApplication::clipboard()->setPixmap(clipboardData);

    m_currCommand = std::unique_ptr<Command>(new CommandCut(m_currSelection));
    performCompleteCommand();
    pushCurrentCommand();
    emit commandStackChanged(m_cmdStack, m_cmdStackPos);
}

void Editor::onCopy() {
    if (m_currSelection.isEmpty())
        return;

    //TODO: incapsulate in a Command?
    //TODO: check on native Linux
    QPixmap clipboardData = m_currBuffer.copy(m_currSelection);
    QGuiApplication::clipboard()->setPixmap(clipboardData);
}

void Editor::onPaste() {
    assert(m_currCommand == nullptr);

    auto clipboardData = QGuiApplication::clipboard()->image();
    if (clipboardData.isNull())
        return;

    auto rawData = QPixmap::fromImage(clipboardData);
    m_currCommand = std::unique_ptr<Command>(new CommandPaste(m_currSelection, rawData));
    performCompleteCommand();
    pushCurrentCommand();
    emit commandStackChanged(m_cmdStack, m_cmdStackPos);
}


void Editor::onSelectAll() {
    m_currSelection = QRect {0, 0, m_width, m_height};
    emit somethingDrawn();
    emit selectionChanged(true);
}

void Editor::onSelectNone() {
    m_currSelection = QRect {0, 0, 0, 0};
    emit somethingDrawn();
    emit selectionChanged(false);
}


void Editor::onClicked(const QPoint pos, Qt::MouseButton button) {
    Command * maybeCmd = ToolConfig::instance().getConfig(m_activeTool);
    if (!maybeCmd->isClickable()) {
        //FIXME: use a ToolSetting class instead
        return;
    }

    std::cout << "Editor::onClicked; m_activeTool: " << m_activeTool << std::endl;

    m_currCommand = ToolConfig::instance().createCommand(m_activeTool);
    m_currCommand->setMode((button == Qt::LeftButton) ? CommandMode::Primary : CommandMode::Alternate );
    m_currCommand->setEditor(this);
    m_currCommand->setTargetPos(pos);

    performCompleteCommand();
    if (m_currCommand->isModifying()) {
        pushCurrentCommand();
        emit commandStackChanged(m_cmdStack, m_cmdStackPos);
    } else {
        m_currCommand.reset();
    }

    assert(m_currCommand == nullptr);
}

void Editor::onDragStarted(const QPoint pos) {
    Command * maybeCmd = ToolConfig::instance().getConfig(m_activeTool);
    if (!maybeCmd->isDraggable()) {
        //FIXME: use a ToolSetting class instead
        return;
    }

    m_currCommand = ToolConfig::instance().createCommand(m_activeTool);
    m_currCommand->setEditor(this);
    m_currCommand->startDrag(pos);
}

void Editor::onDragContinued(const QPoint start, const QPoint end) {
    if ((m_currCommand == nullptr) || (!m_currCommand->isDraggable()))
        return;

    m_currCommand->continueDrag(start, end);
}

void Editor::onDragEnded(const QPoint pos) {
    if ((m_currCommand == nullptr) || (!m_currCommand->isDraggable()))
        return;

    performCompleteCommand();
    if (m_currCommand->isModifying()) {
        pushCurrentCommand();
        emit commandStackChanged(m_cmdStack, m_cmdStackPos);
    } else {
        m_currCommand.reset();
    }

    assert(m_currCommand == nullptr);
}


void Editor::onWheelRolled(const QPoint pos, int delta, QFlags<Qt::KeyboardModifier> modifiers) {
    static constexpr double wheelZoomFactor = 50.0;
    static constexpr double wheelScrollFactor = 1.0;
    assert(m_currCommand == nullptr);
    std::cout << "Editor::onWheelRolled; delta: " << delta << std::endl;

    if (modifiers &  Qt::ControlModifier) {
        //zoom command
        m_currCommand = ToolConfig::instance().createCommand(CommandType::Zoom);
        m_currCommand->setEditor(this);
        m_currCommand->setTargetPos(pos);
        m_currCommand->setMode(delta >= 0 ? Primary : Alternate);
        static_cast<CommandZoom *>(m_currCommand.get())->setZoomFactor(std::abs(delta)/wheelZoomFactor);
        performCompleteCommand();
        m_currCommand.reset();

    } else {
        std::cout << "Editor::onWheelRolled; creating scroll command" << std::endl;

        // scroll command
        m_currCommand = ToolConfig::instance().createCommand(CommandType::Scroll);
        m_currCommand->setEditor(this);
        m_currCommand->setTargetPos(pos); //not used
        m_currCommand->setMode((modifiers &  Qt::ShiftModifier) ? Alternate : Primary);
        static_cast<CommandScroll *>(m_currCommand.get())->setScrollAmount(-delta/wheelScrollFactor);
        performCompleteCommand();
        m_currCommand.reset();
    }

    assert(m_currCommand == nullptr);
}

void Editor::onToolChosen(CommandType newToolType) {
    std::cout << "onToolChosen" << std::endl;

    m_activeTool = newToolType;
    auto currCmd = ToolConfig::instance().getConfig(m_activeTool);
    emit cursorChanged(QCursor(currCmd->getCursor()));
}

void Editor::onToolColorChosen(const QColor & color) {
    Command * cmdDraw = ToolConfig::instance().getConfig(CommandType::Draw);
    (static_cast<CommandDraw *>(cmdDraw))->setColor(color);

    Command * cmdFill = ToolConfig::instance().getConfig(CommandType::Fill);
    (static_cast<CommandFill *>(cmdFill))->setColor(color);
}

void Editor::onToolWidthChosen(int width) {
    Command *currCmd = ToolConfig::instance().getConfig(m_activeTool);
    switch (m_activeTool) {
        case Draw:
            (static_cast<CommandDraw *>(currCmd))->setWidth(width);
            break;
        case Erase:
            (static_cast<CommandErase *>(currCmd))->setWidth(width);
            break;
        default:
            ; //ignore
    }
}

void Editor::resetDocument() {
    m_width = m_initialBuffer.width();
    m_height = m_initialBuffer.height();
    m_zoomLevel = 1.0;

    reset(m_currBuffer, m_initialBuffer);
    setModified(false);
    emit documentSizeChanged(m_initialBuffer.size());
}

void Editor::reset(QPixmap &destBuffer, const QPixmap &srcBuffer) {
    destBuffer = srcBuffer.copy();

    m_cmdStack.clear();
    m_cmdStackPos = 0;

    m_currCommand = nullptr;

    emit commandStackChanged(m_cmdStack, m_cmdStackPos);
}


void Editor::setModified(bool isModified) {
    m_isModified = isModified;
    emit modifiedStatusChanged(isModified);
}

void Editor::performCompleteCommand() {
    QPainter bufferPainter {&m_currBuffer};
    performCurrentCommand(&bufferPainter);
}

void Editor::performPartialCommand(QPainter * canvasPainter) {
    performCurrentCommand(canvasPainter);
}

void Editor::performCurrentCommand(QPainter * painter) {
    assert(painter != nullptr);
    if (m_currCommand == nullptr)
        return;
    
    if (m_currCommand->isModifying()) {
        m_currCommand->perform(*painter);

    } else {
        m_currCommand->perform();
    }

    emit somethingDrawn();
}

void Editor::paintCurrentBuffer(QPainter * painter) {
    assert(painter != nullptr);
    painter->drawPixmap(0, 0, m_currBuffer);
}

void Editor::paintCustomCursor(QPoint &pos, QWidget * target) {
    auto currToolConfig = ToolConfig::instance().getConfig(m_activeTool);
    if (!currToolConfig->usesCustomCursor())
        return;

    QPainter painter {target};
    currToolConfig->paintCustomCursor(painter, pos);
}

void Editor::paintCurrentSelection(QWidget * target) {
    if (m_currSelection.size().isEmpty())
        return;

    QPainter painter {target};

    painter.setPen(QPen(Qt::DashLine));
    painter.drawRect(m_currSelection);
}


void Editor::pushCurrentCommand() {
    //std::cout << "pushCurrentCommand" << std::endl;
    //std::cout << "before; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;

    assert(m_cmdStackPos <= (int)m_cmdStack.size());

    for (int cmdDiscardPos = (int)m_cmdStack.size()-1; cmdDiscardPos >= m_cmdStackPos; cmdDiscardPos--) {
        // previously undoed commands are discarded when a new command is requested
        m_cmdStack.erase(m_cmdStack.begin() + cmdDiscardPos);
    }
    
    //m_cmdStack.push_back(m_currCommand);
    m_cmdStack.push_back(std::move(m_currCommand));
    m_cmdStackPos ++;

    //std::cout << "after push; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;

    m_currCommand = nullptr;

    setModified();
    emit commandStackChanged(m_cmdStack, m_cmdStackPos);
}


void Editor::restoreCommandsFromStack() {
    m_currBuffer = m_initialBuffer.copy();
    QPainter painter {&m_currBuffer};
    setModified(false);

    for (int i=0; i < m_cmdStackPos && i < (int)m_cmdStack.size(); ++i) {
        m_isModified = true;
        //std::cout << "Restoring command #" << i << std::endl;
        m_cmdStack[i]->perform(painter);
    }

    setModified(m_isModified);
    emit somethingDrawn();
}

