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


enum CommandType {
    Select,
    Draw,
    Erase,
};


class Command {

public:
    virtual ~Command() = default;
    virtual std::unique_ptr<Command> clone() const = 0;

    virtual CommandType type() const = 0;

    virtual void addDrag(const QPoint from, const QPoint to) = 0;
    virtual void perform(QPainter &painter) const = 0;
    virtual QCursor getCursor() const { return QCursor(Qt::ArrowCursor); }
    virtual void paintCustomCursor(QPainter &painter, QPoint pos) const {};
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

    QColor color() const { return m_color; }

    void addDrag(const QPoint from, const QPoint to) override;
    void perform(QPainter &painter) const override;
    QCursor getCursor() const override { return QCursor(Qt::BlankCursor); }
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
    void addDrag(const QPoint from, const QPoint to) override;
    void perform(QPainter &painter) const override;
    QCursor getCursor() const override { return QCursor(Qt::BlankCursor); }
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
    void addDrag(const QPoint from, const QPoint to) override;
    void perform(QPainter &painter) const override;
    QCursor getCursor() const override { return QCursor(Qt::CrossCursor); }

    //void paintCustomCursor(QPainter &painter, QPoint pos) override;

protected:
    QPoint m_from;
    QPoint m_to;
};



#endif // COMMAND_H
