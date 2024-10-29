#include "editor.h"
#include "command.h"
#include "paintbrush_window.h"
#include "qpixmap.h"

#include <QPainter>
#include <QPen>

#include <iostream>


const QColor& bkgColor { QColorConstants::LightGray };


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

    return true;
}

bool Editor::saveFile(const QString filename) {
    bool isSaveOk = m_currBuffer.save(filename);
    if (!isSaveOk)
        return false;

    reset(m_initialBuffer, m_currBuffer);
    setModified(false);

    return true;
}

void Editor::reset(QPixmap &destBuffer, const QPixmap &srcBuffer) {
    destBuffer = srcBuffer.copy();

    m_cmdStack.clear();
    m_cmdStackPos = 0;

    if (m_currCommand) {
        delete m_currCommand;
        m_currCommand = nullptr;
    }

    commandStackChanged(m_cmdStack, m_cmdStackPos);
}

void Editor::setModified(bool isModified) {
    m_isModified = isModified;
    modifiedStatusChanged(isModified);
}


void Editor::onStartDrag() {
    struct Command *newCommand;

    switch (m_activeTool->type()) {
        case ToolDraw: {
                auto lines =  new std::vector<QPair<QPoint, QPoint>>;
                newCommand = new struct CommandDraw(m_activeColor, lines);
            }
            break;
        case ToolErase: {
                auto lines =  new std::vector<QPair<QPoint, QPoint>>;
                newCommand = new struct CommandErase(lines);
            }
            break;

        default:
            return;
    }

    m_currCommand = newCommand;
}

void Editor::onEndDrag() {
    switch (m_activeTool->type()) {
        case ToolDraw:
        case ToolErase:
            pushCurrentCommand();
            break;
        
        default:
            ;
    }
}

void Editor::onDrag(const QPoint start, const QPoint end) {
    assert(m_currCommand != nullptr);

    QPair<QPoint,QPoint> newLine {start, end};
    performPartialCommand(m_currCommand, newLine);
    updateCurrentCommand(newLine);
}


void Editor::onToolChosen(ToolType newToolType) {
    if (m_activeTool->type() == newToolType)
        return;
    
    delete m_activeTool;

    switch (newToolType) {
        case ToolDraw:
            m_activeTool = new ToolDrawData {m_activeColor, defaultDrawWidth};
            break;
        case ToolErase:
            m_activeTool = new ToolEraseData {defaultEraseWidth};
            break;
        default:
            m_activeTool = new ToolSelectData();
    }
}

void Editor::undo() {
    assert(m_cmdStackPos >= 0);

    //std::cout << "before undo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    if (m_cmdStackPos == 0)
        return;

    m_cmdStackPos --;

    restoreCommandsFromStack();
    //std::cout << "after undo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    commandStackChanged(m_cmdStack, m_cmdStackPos);
}

void Editor::redo() {
    //std::cout << "before redo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    assert(m_cmdStackPos <= (int)m_cmdStack.size());

    if (m_cmdStackPos == (int)m_cmdStack.size())
        return;

    m_cmdStackPos ++;

    restoreCommandsFromStack();
    //std::cout << "after redo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    commandStackChanged(m_cmdStack, m_cmdStackPos);
}


void Editor::pushCurrentCommand() {
    //std::cout << "pushCurrentCommand" << std::endl;
    //std::cout << "before; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;

    assert(m_cmdStackPos <= (int)m_cmdStack.size());

    for (int cmdDiscardPos = (int)m_cmdStack.size()-1; cmdDiscardPos >= m_cmdStackPos; cmdDiscardPos--) {
        // previously undoed commands are discarded when a new command is requested
        struct Command *discardedCmd = m_cmdStack[cmdDiscardPos];
        delete discardedCmd;
        m_cmdStack.erase(m_cmdStack.begin() + cmdDiscardPos);
    }
    
    m_cmdStack.push_back(m_currCommand);
    m_cmdStackPos ++;

    //std::cout << "after push; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;

    m_currCommand = nullptr;

    setModified();
    commandStackChanged(m_cmdStack, m_cmdStackPos);
}


void Editor::restoreCommandsFromStack() {
    m_currBuffer = m_initialBuffer.copy();
    setModified(false);

    for (int i=0; i < m_cmdStackPos && i < (int)m_cmdStack.size(); ++i) {
        m_isModified = true;
        //std::cout << "Restoring command #" << i << std::endl;
        performCommand(m_cmdStack[i]);
    }
}

void Editor::performCommand(struct Command *cmd) {
    //std::cout << "performCommand; cmd=" << cmd << std::endl;

    switch (cmd->type()) {
        case CommandDraw:
            {
                struct CommandDraw *cmdDraw = static_cast<struct CommandDraw *> (cmd);
                auto lines = *(cmdDraw->lines());
                for (auto line: lines) {
                    performPartialCommand(cmd, line);
                }
            }
            break;
        
        case CommandErase:
            {
                struct CommandErase *cmdErase = static_cast<struct CommandErase *> (cmd);
                auto lines = *(cmdErase->lines());
                for (auto line: lines) {
                    performPartialCommand(cmd, line);
                }
            }
            break;
    }

    setModified();
}

void Editor::performPartialCommand(struct Command *cmd, const QPair<QPoint, QPoint> line) {
    QColor drawColor;
    int drawWidth;
    switch (cmd->type()) {
        case CommandDraw:
            {
                struct CommandDraw *cmdDraw = static_cast<struct CommandDraw *> (cmd);
                drawColor = cmdDraw->color();
                drawWidth = 2;
            }
            break;

        case CommandErase:
            {
                drawColor = bkgColor;
                drawWidth = 10;
            }
            break;
    }

    QPainter painter {&m_currBuffer};
    painter.setPen(QPen(drawColor, drawWidth));
    painter.drawLine(line.first, line.second);
}

void Editor::updateCurrentCommand(const QPair<QPoint, QPoint> line) {
    switch (m_currCommand->type()) {
        case CommandDraw:
            {
                struct CommandDraw *cmdDraw = static_cast<struct CommandDraw *> (m_currCommand);
                cmdDraw->lines()->push_back(line);
            }
            break;
        
        case CommandErase:
            {
                struct CommandErase *cmdErase = static_cast<struct CommandErase *> (m_currCommand);
                cmdErase->lines()->push_back(line);
            }
            break;
    }
}

