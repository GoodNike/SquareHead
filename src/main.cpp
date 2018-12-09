#include <QtWidgets/QApplication>

#include "UI/MainWindow.h"

#include "GlobalOptions.h"

int main(int argc, char *argv[])
{
#if defined _DEBUG && defined _MSC_VER
    #include <crtdbg.h>
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    QApplication a(argc, argv);

    GlobalOptions::instance().initQApplication();

    MainWindow w;
    w.show();

    return a.exec();
}
