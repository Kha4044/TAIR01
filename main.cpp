#include "widget.h"
#include "socket.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Socket* vnaClient = new Socket();
    Widget w(vnaClient);
    w.show();
    return app.exec();
}
