#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setFocusPolicy(Qt::StrongFocus);
    w.show();
    return a.exec();
}
