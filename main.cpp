#include "widget.h"
#include "socket.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Socket* vnaClient = new Socket();
    Widget widget(vnaClient);
    widget.show();

    return app.exec();
}
