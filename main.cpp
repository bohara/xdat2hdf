#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#include "converter.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<Converter>("XDAT", 1, 0, "Converter");
    qmlRegisterType<SpeciesInfo>("XDAT", 1, 0, "SpeciesInfo");
    qmlRegisterType<AtomicSystem>("XDAT", 1, 0, "AtomicSystem");

    // Set applicatio dir to current directory
    QDir dir(QApplication::applicationDirPath());
    QDir::setCurrent(dir.absolutePath());

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
