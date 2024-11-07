#ifndef COMMAND_H
#define COMMAND_H


#include "qnamespace.h"
#include "qpainter.h"
#include <QPixmap>
#include <QColor>
#include <QPoint>
#include <QPair>
#include <QPainter>
#include <QCursor>

#include <iostream>
#include <vector>
#include <memory>


class Editor;

enum CommandType {
    Select,
    Draw,
    Fill,
    Erase,
    Cut,
    Copy,
    Paste,
    Zoom,
};

enum CommandMode {
    Primary,
    Alternate,
};

class Command {

public:
    void setMode(CommandMode mode) { m_mode = mode; };
    void setEditor(Editor *editor) { m_editor = editor; }

    virtual ~Command() = default;
    virtual std::unique_ptr<Command> clone() const = 0;

    virtual CommandType type() const = 0;
    virtual bool isModifying() const = 0;

    virtual bool isClickable() const { return false; };
    virtual void setTargetPos(const QPoint pos) {};

    virtual bool isDraggable() const { return false; };
    virtual void startDrag(const QPoint pos) {};
    virtual void continueDrag(const QPoint from, const QPoint to) {};
    
    virtual void perform() const {};
    virtual void perform(QPainter &painter) const {};
    virtual const Qt::CursorShape getCursor() const { return Qt::ArrowCursor; }
    virtual bool usesCustomCursor() const { return false; };
    virtual void paintCustomCursor(QPainter &painter, QPoint pos) const {};

protected:
    CommandMode m_mode;
    Editor *m_editor;
};

class CommandDraw: public Command {

public:
    CommandDraw(const QColor &color, int width): m_width(width), m_color(color) {
        m_lines = std::make_unique<std::vector<QPair<QPoint, QPoint>>>();
    }

    CommandDraw(const CommandDraw &other): m_width(other.m_width), m_color(other.m_color) {
        m_lines = std::make_unique<std::vector<QPair<QPoint, QPoint>>>();

        for (auto line: *other.m_lines) {
            m_lines->push_back(line);
        }
    }

    void setWidth(int width) {
        m_width = width;
    }

    void setColor(const QColor &color) {
        m_color = color;
    }

    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CommandDraw>(*this);
    }

    CommandType type() const override { return CommandType::Draw; }
    virtual bool isModifying() const override { return true; };

    QColor color() const { return m_color; }

    bool isDraggable() const override { return true; };
    void continueDrag(const QPoint from, const QPoint to) override;
    void perform(QPainter &painter) const override;
    virtual const Qt::CursorShape getCursor() const override { return Qt::BlankCursor; }
    bool usesCustomCursor() const override { return true; };
    void paintCustomCursor(QPainter &painter, QPoint pos) const override;

protected:
    int m_width;
    QColor m_color;
    std::unique_ptr<std::vector<QPair<QPoint, QPoint>>> m_lines;
};


class CommandFill: public Command {

public:
    CommandFill(const QColor &color, const QPoint &targetPos): m_color(color), m_targetPos(targetPos) {}

    void setColor(const QColor &color) {
        m_color = color;
    }

    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CommandFill>(*this);
    }

    CommandType type() const override { return CommandType::Fill; }
    bool isModifying() const override { return true; };
    bool isClickable() const override { return true; };
    void setTargetPos(const QPoint pos) override { m_targetPos = pos; };

    QColor color() const { return m_color; }

    void perform(QPainter &painter) const override;
    const Qt::CursorShape getCursor() const override { return Qt::ArrowCursor; }

protected:
    QColor m_color;
    QPoint m_targetPos;
};


class CommandErase: public Command {

public:
    CommandErase(int width): m_width(width) {
        m_lines = std::make_unique<std::vector<QPair<QPoint, QPoint>>>();
    }

    CommandErase(const CommandErase &other): m_width(other.m_width) {
        m_lines = std::make_unique<std::vector<QPair<QPoint, QPoint>>>();

        for (auto line: *other.m_lines) {
            m_lines->push_back(line);
        }
    }

    void setWidth(int width) {
        m_width = width;
    }

    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CommandErase>(*this);
    }

    CommandType type() const override { return CommandType::Erase; }
    virtual bool isModifying() const override { return true; };

    bool isDraggable() const override { return true; };
    void continueDrag(const QPoint from, const QPoint to) override;
    void perform(QPainter &painter) const override;
    
    virtual const Qt::CursorShape getCursor() const override { return Qt::BlankCursor; }
    bool usesCustomCursor() const override { return true; };
    void paintCustomCursor(QPainter &painter, QPoint pos) const override;

protected:
    int m_width;
    std::unique_ptr<std::vector<QPair<QPoint, QPoint>>> m_lines;
};


class CommandSelect: public Command {

public:
    CommandSelect() {}

    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CommandSelect>(*this);
    }

    CommandType type() const override { return CommandType::Select; }
    bool isModifying() const override { return false; };

    bool isDraggable() const override { return true; };
    void startDrag(const QPoint pos) override;
    void continueDrag(const QPoint from, const QPoint to) override;

    const Qt::CursorShape getCursor() const override { return Qt::CrossCursor; }

protected:
    QPoint m_from;
    QPoint m_to;
};



class CommandCut: public Command {

public:
    CommandCut(QRect &targetArea): m_targetArea(targetArea) {}

    void setTargetArea(QRect &targetArea) { m_targetArea = targetArea; }

    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CommandCut>(*this);
    }

    CommandType type() const override { return CommandType::Cut; }
    bool isModifying() const override { return true; };

    void perform(QPainter &painter) const override;

protected:
    QRect m_targetArea;
};


class CommandCopy: public Command {

public:
    CommandCopy(QPixmap & data) {
        m_data = std::make_unique<QPixmap>(data);
    }

    CommandCopy(const CommandCopy& other) {
        m_data = std::make_unique<QPixmap>(other.m_data->copy());
    }

    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CommandCopy>(*this);
    }

    CommandType type() const override { return CommandType::Copy; }
    bool isModifying() const override { return false; };

protected:
    std::unique_ptr<QPixmap> m_data;
};


class CommandPaste: public Command {

public:
    CommandPaste(QRect &targetArea, QPixmap & data): m_targetArea(targetArea) {
        m_data = std::make_unique<QPixmap>(data);
    }

    CommandPaste(const CommandPaste& other) {
        m_data = std::make_unique<QPixmap>(other.m_data->copy());
    }

    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CommandPaste>(*this);
    }

    CommandType type() const override { return CommandType::Paste; }
    bool isModifying() const override { return true; };

    void perform(QPainter &painter) const override;

protected:
    QRect m_targetArea;
    std::unique_ptr<QPixmap> m_data;
};


class CommandZoom: public Command {

public:
    CommandZoom() {}

    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CommandZoom>(*this);
    }

    CommandType type() const override { return CommandType::Zoom; }
    bool isModifying() const override { return false; };
    bool isClickable() const override { return true; };

    void setTargetPos(const QPoint pos) override { m_targetPos = pos; };

    const Qt::CursorShape getCursor() const override { return Qt::CrossCursor; }
    void perform() const override;

protected:
    QPoint m_targetPos;
};


#endif // COMMAND_H
