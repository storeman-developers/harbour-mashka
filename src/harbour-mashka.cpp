#include <QtQuick>

#include <sailfishapp.h>

#include <mashka_version.h>
#include "mashka.h"
#include "mmodel.h"
#include "mproxymodel.h"


int main(int argc, char *argv[])
{
    qmlRegisterType<Mashka>     ("harbour.mashka", 1, 0, "Mashka");
    qmlRegisterType<MModel>     ("harbour.mashka", 1, 0, "MModel");
    qmlRegisterType<MProxyModel>("harbour.mashka", 1, 0, "MProxyModel");
    qRegisterMetaType<QVector<int>>();

    auto app = SailfishApp::application(argc, argv);
    app->setApplicationVersion(QStringLiteral(MASHKA_VERSION));

    auto view = SailfishApp::createView();
    view->setSource(SailfishApp::pathToMainQml());
    view->show();

    return app->exec();
}
