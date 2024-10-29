#ifndef COMMAND_H
#define COMMAND_H


#include <QColor>
#include <QPoint>
#include <QPair>

#include <iostream>
#include <vector>
#include <variant>



//---- Command ------------------------------------------------------------

enum CommandType {
    CommandDraw,
    CommandErase,
};


struct Command {

public:
    explicit Command(CommandType type): m_type(type) {}
    
    CommandType type() const { return m_type; }

protected:
    CommandType m_type;
};


struct CommandDraw: Command {

public:
    explicit CommandDraw(QColor color, std::vector<QPair<QPoint, QPoint>> *lines): 
        Command(CommandType::CommandDraw), m_color(color), m_lines(lines) {}

    QColor color() const { return m_color; }

    std::vector<QPair<QPoint, QPoint>> *lines() { return m_lines; }

protected:
    QColor m_color;
    std::vector<QPair<QPoint, QPoint>> *m_lines;
};


struct CommandErase: Command {

public:
    explicit CommandErase(std::vector<QPair<QPoint, QPoint>> *lines): Command(CommandType::CommandErase), m_lines(lines) {}

    std::vector<QPair<QPoint, QPoint>> *lines() { return m_lines; }

protected:
    std::vector<QPair<QPoint, QPoint>> *m_lines;
};


//---- Tool ------------------------------------------------------------

enum ToolType {
    ToolDraw,
    ToolErase,
    ToolSelect,
};

struct ToolData {
public:
    explicit ToolData(ToolType type): m_type(type) {}
    virtual ~ToolData() = default;

    ToolType type() const { return m_type; }
    //virtual int cursorSize() const { std::cout << "ToolData.cursorSize()" <<std::endl; return 0; }
    virtual int cursorSize() const = 0;

protected:
    ToolType m_type;
};

struct ToolDrawData: ToolData {
public:
    explicit ToolDrawData(QColor color, int width): ToolData(ToolType::ToolDraw), m_color {color}, m_width(width) {}
    int width() const { return m_width; }
    QColor color() const { return m_color; }

    int cursorSize() const override { return m_width; }

protected:
    QColor m_color;
    int m_width;
};

struct ToolEraseData: ToolData {
public:
    explicit ToolEraseData(int width): ToolData(ToolType::ToolErase), m_width(width) {}
    int width() const { return m_width; }

    int cursorSize() const override { return m_width; }

protected:
    int m_width;
};


struct ToolSelectData: ToolData {
public:
    explicit ToolSelectData(): ToolData(ToolType::ToolSelect) {}

    int cursorSize() const override { return 0; }

};

#endif // COMMAND_H
