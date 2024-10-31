#ifndef COMMAND_H
#define COMMAND_H


#include "qnamespace.h"
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
    Erase,
};


class Command {

public:
    void setEditor(Editor *editor) { m_editor = editor; }

    virtual ~Command() = default;
    virtual std::unique_ptr<Command> clone() const = 0;

    virtual CommandType type() const = 0;
    virtual bool isModifying() const { return false; };

    virtual void startDrag(const QPoint pos) {};
    virtual void continueDrag(const QPoint from, const QPoint to) {};
    virtual void perform(QPainter &painter) const = 0; //TODO: change perform signature to use buffer
    virtual const Qt::CursorShape getCursor() const { return Qt::ArrowCursor; }
    virtual void paintCustomCursor(QPainter &painter, QPoint pos) const {};

protected:
    Editor *m_editor;
};


class CommandDraw: public Command {

public:
    CommandDraw(QColor color, int width): m_width(width), m_color(color) {
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

    void continueDrag(const QPoint from, const QPoint to) override;
    void perform(QPainter &painter) const override;
    virtual const Qt::CursorShape getCursor() const override { return Qt::BlankCursor; }
    void paintCustomCursor(QPainter &painter, QPoint pos) const override;

protected:
    int m_width;
    QColor m_color;
    std::unique_ptr<std::vector<QPair<QPoint, QPoint>>> m_lines;
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

    void continueDrag(const QPoint from, const QPoint to) override;
    void perform(QPainter &painter) const override;
    
    virtual const Qt::CursorShape getCursor() const override { return Qt::BlankCursor; }
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

    void startDrag(const QPoint pos) override;
    void continueDrag(const QPoint from, const QPoint to) override;

    void perform(QPainter &painter) const override;
    virtual const Qt::CursorShape getCursor() const override { return Qt::CrossCursor; }

    //void paintCustomCursor(QPainter &painter, QPoint pos) override;

protected:
    QPoint m_from;
    QPoint m_to;
};



#endif // COMMAND_H
