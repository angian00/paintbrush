#include "editor.h"
#include "command.h"

#include <QPainter>
#include <QPen>

#include <iostream>


const QColor& bkgColor { QColorConstants::LightGray };


Editor::Editor(int width, int height): m_width(width), m_height(height) {
    m_initialBuffer = QPixmap(m_width, m_height);
    m_initialBuffer.fill(bkgColor);

    m_currBuffer = QPixmap(m_width, m_height);
    m_currBuffer.fill(bkgColor);

    m_currLines = new std::vector<QPair<QPoint, QPoint>>;
}

bool Editor::loadFile(const QString filename) {
    //TODO: set m_initialBuffer
    return m_currBuffer.load(filename);
}

bool Editor::saveFile(const QString filename) {
    //TODO: set m_initialBuffer
    return m_currBuffer.save(filename);
}


void Editor::startDrag() {
    struct Command *newCommand;
    switch (m_activeTool) {
        case ToolDraw:
            newCommand = new struct CommandDraw(CommandType::CommandDraw, m_activeColor, m_currLines);
            break;
        case ToolErase:
            newCommand = new struct CommandErase(CommandType::CommandErase, m_currLines);
            break;
    }

    m_currCommand = newCommand;
}

void Editor::endDrag() {
    pushCurrentCommand();
}

void Editor::dragLine(const QPoint start, const QPoint end) {
    assert(m_currCommand != nullptr);

    QPair<QPoint,QPoint> newLine {start, end};
    performPartialCommand(m_currCommand, newLine);
    pushPartialCommand(newLine);
}


void Editor::undo() {
    assert(m_cmdStackPos >= 0);

    //std::cout << "before undo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    if (m_cmdStackPos == 0)
        return;

    m_cmdStackPos --;
    restoreCommands();
    //std::cout << "after undo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
}

void Editor::redo() {
    //std::cout << "before redo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
    assert(m_cmdStackPos <= (int)m_commandStack.size());

    if (m_cmdStackPos == (int)m_commandStack.size())
        return;

    m_cmdStackPos ++;
    restoreCommands();
    //std::cout << "after redo; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;
}





void Editor::pushCurrentCommand() {
    //std::cout << "pushCurrentCommand" << std::endl;
    //std::cout << "before; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;

    assert(m_cmdStackPos <= (int)m_commandStack.size());

    for (int cmdDiscardPos = (int)m_commandStack.size()-1; cmdDiscardPos >= m_cmdStackPos; cmdDiscardPos--) {
        // previously undoed commands are discarded when a new command is requested
        struct Command *discardedCmd = m_commandStack[cmdDiscardPos];
        delete discardedCmd;
        m_commandStack.erase(m_commandStack.begin() + cmdDiscardPos);
    }
    
    m_commandStack.push_back(m_currCommand);
    m_cmdStackPos ++;

    //std::cout << "after push; m_commandStack.size()=" << m_commandStack.size() << "; m_cmdStackPos=" << m_cmdStackPos << std::endl;

    m_currLines = new std::vector<QPair<QPoint, QPoint>>;
    m_currCommand = nullptr;
}


void Editor::restoreCommands() {
    m_currBuffer = m_initialBuffer.copy();

    for (int i=0; i < m_cmdStackPos && i < (int)m_commandStack.size(); ++i) {
        std::cout << "Restoring command #" << i << std::endl;
        performCommand(m_commandStack[i]);
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

void Editor::pushPartialCommand(const QPair<QPoint, QPoint> line) {
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

