#ifndef PAINTBRUSH_SCROLL_AREA_H
#define PAINTBRUSH_SCROLL_AREA_H

#include "editor.h"
#include "qnamespace.h"

#include <QWidget>
#include <QScrollArea>
#include <QWheelEvent>


class PaintbrushScrollArea : public QScrollArea {
public:
    explicit PaintbrushScrollArea(QWidget *parent = nullptr) : QScrollArea(parent) {}

protected:
    // disable automatic scroll on mouse wheel events
    void wheelEvent(QWheelEvent *event) override { event->ignore(); }
};


#endif // PAINTBRUSH_SCROLL_AREA_H
