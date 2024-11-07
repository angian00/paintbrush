#include "paintbrush_scroll_area.h"


#include <QScrollArea>
#include <QScrollBar>


void PaintbrushScrollArea::onViewMoved(QPoint delta) {
    auto vBar = verticalScrollBar();
    auto newVerValue = vBar->value() + delta.y();
    newVerValue = std::max(vBar->minimum(), newVerValue);
    vBar->setValue(newVerValue);

    auto hBar = horizontalScrollBar();
    auto newHorValue = hBar->value() + delta.x();
    newHorValue = std::max(hBar->minimum(), newHorValue);
    hBar->setValue(newHorValue);
}
