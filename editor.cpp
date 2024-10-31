#include "editor.h"

#include "constants.h"
#include "command.h"
#include "tool_config.h"
#include "paintbrush_window.h"

#include <QPainter>
#include <QPixmap>
#include <QPen>

#include <iostream>
#include <memory>




Editor::Editor(int width, int height): 
    m_width(width), m_height(height) {
    
    m_initialBuffer = QPixmap(m_width, m_height);
    m_initialBuffer.fill(bkgColor);

    m_currBuffer = QPixmap(m_width, m_height);
    m_currBuffer.fill(bkgColor);

    m_isModified = false;
}

void Editor::newFile() {
    m_initialBuffer = QPixmap(m_width, m_height);
    m_initialBuffer.fill(bkgColor);

    reset(m_currBuffer, m_initialBuffer);
    setModified(false);
}

bool Editor::loadFile(const QString filename) {
    bool isLoadOk = m_initialBuffer.load(filename);
    if (!isLoadOk)
        return false;

    reset(m_currBuffer, m_initialBuffer);
    setModified(false);
    somethingDrawn();

    return true;
}

bool Editor::saveFile(const QString filename) {
    bool isSaveOk = m_currBuffer.save(filename);
    if (!isSaveOk)
        return false;

    reset(m_initialBuffer, m_currBuffer);
    setModified(false);
    somethingDrawn();

    return true;
}


void Editor::onUndo() {
    assert(m_cmdStackPos >= 0);

    //std::cout << "before undo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    if (m_cmdStackPos == 0)
        return;

    m_cmdStackPos --;

    restoreCommandsFromStack();
    //std::cout << "after undo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    commandStackChanged(m_cmdStack, m_cmdStackPos);
}

void Editor::onRedo() {
    //std::cout << "before redo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    assert(m_cmdStackPos <= (int)m_cmdStack.size());

    if (m_cmdStackPos == (int)m_cmdStack.size())
        return;

    m_cmdStackPos ++;

    restoreCommandsFromStack();
    //std::cout << "after redo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    commandStackChanged(m_cmdStack, m_cmdStackPos);
}


void Editor::onDragStarted(const QPoint pos) {
    assert(m_currCommand == nullptr);

    m_currCommand = ToolConfig::instance().createCommand(m_activeTool);
    m_currCommand->setEditor(this);
    m_currCommand->startDrag(pos);
}

void Editor::onDragContinued(const QPoint start, const QPoint end) {
    assert(m_currCommand != nullptr);

    m_currCommand->continueDrag(start, end);
    // //DEBUG
    // if (m_currCommand->type() == CommandType::Select) {
    //     auto cmdSelect = static_cast<CommandSelect *>(m_currCommand.get());
    //     m_currSelection = cmdSelect->selectionArea();
    // }
    // //
}

void Editor::onDragEnded(const QPoint pos) {
    performCurrentCommand();
    if (m_currCommand->isModifying()) {
        pushCurrentCommand();
        commandStackChanged(m_cmdStack, m_cmdStackPos);
    } else {
        m_currCommand.reset();
    }

    assert(m_currCommand == nullptr);

}


void Editor::onToolChosen(CommandType newToolType) {
    m_activeTool = newToolType;
    auto currCmd = ToolConfig::instance().getConfig(m_activeTool);
    cursorChanged(QCursor(currCmd->getCursor()));
}

void Editor::onToolColorChosen(const QColor & color) {
    Command * cmdDraw = ToolConfig::instance().getConfig(CommandType::Draw);
    (static_cast<CommandDraw *>(cmdDraw))->setColor(color);
}

void Editor::onToolWidthChosen(int width) {
    Command *currCmd = ToolConfig::instance().getConfig(m_activeTool);
    //FIXME: improve pattern
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


void Editor::reset(QPixmap &destBuffer, const QPixmap &srcBuffer) {
    destBuffer = srcBuffer.copy();

    m_cmdStack.clear();
    m_cmdStackPos = 0;

    m_currCommand = nullptr;

    commandStackChanged(m_cmdStack, m_cmdStackPos);
}


void Editor::setModified(bool isModified) {
    m_isModified = isModified;
    modifiedStatusChanged(isModified);
}


void Editor::performCurrentCommand(QPaintDevice * target) {
    if (m_currCommand == nullptr) 
        return;
    
    if (target == nullptr)
        target = &m_currBuffer;

    QPainter painter {target};
    m_currCommand->perform(painter);
    somethingDrawn();
}

void Editor::paintCurrentBuffer(QPaintDevice * target) {
    if (target == nullptr)
        target = &m_currBuffer;

    QPainter painter { target };
    painter.drawPixmap(0, 0, m_currBuffer);
}

void Editor::paintToolCursor(QPoint &pos, QPaintDevice * target) {
    if (target == nullptr)
        target = &m_currBuffer;

    QPainter painter {target};

    auto currToolConfig = ToolConfig::instance().getConfig(m_activeTool);
    currToolConfig->paintCustomCursor(painter, pos);
}

void Editor::paintCurrentSelection(QPaintDevice * target) {
    if (m_currSelection.size().isEmpty())
        return;

    if (target == nullptr)
        target = &m_currBuffer;

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
    commandStackChanged(m_cmdStack, m_cmdStackPos);
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

    setModified();
    somethingDrawn();
}

