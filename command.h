#ifndef COMMAND_H
#define COMMAND_H


#include <QColor>
#include <QPoint>
#include <QPair>

#include <vector>
#include <variant>


enum CommandType {
    Draw,
    Erase,
};


struct Command {

public:
    explicit Command(CommandType type): m_type(type) {}
    
    const CommandType type() { return m_type; }

protected:
    CommandType m_type;
};


struct CommandDraw: Command {

public:
    explicit CommandDraw(CommandType type, QColor color, std::vector<QPair<QPoint, QPoint>> lines): 
        Command(type), m_color(color), m_lines(lines) {}

    const QColor color() { return m_color; }

    const std::vector<QPair<QPoint, QPoint>> lines() { return m_lines; }

protected:
    QColor m_color;
    std::vector<QPair<QPoint, QPoint>> m_lines;
};


struct CommandErase: Command {

public:
    explicit CommandErase(CommandType type, std::vector<QPair<QPoint, QPoint>> lines): Command(type), m_lines(lines) {}

    const std::vector<QPair<QPoint, QPoint>> lines() { return m_lines; }

protected:
    std::vector<QPair<QPoint, QPoint>> m_lines;
};

#endif // COMMAND_H
