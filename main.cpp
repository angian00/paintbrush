#include "paintbrush_window.h"

#include <QApplication>


int main(int argc, char **argv)
{
    QApplication app (argc, argv);
    PaintbrushWindow window;
    window.show();

    return app.exec();
}
