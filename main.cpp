#include "paintbrush_window.h"

#include <QApplication>

#include <iostream>



int main(int argc, char **argv)
{
    std::cout << "Paintbrush application starting" << std::endl;

    QApplication app (argc, argv);
    PaintbrushWindow window;
    if (argc == 1)
        window.start();
    else
        window.start(argv[1]);

    return app.exec();
}
