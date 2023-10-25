#include "datamodel.h"
#include "modbusmodel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    DataModel *dataModel = new DataModel(&app);
    qmlRegisterSingletonInstance("com.cmp.DataModel", 1, 0, "DataModel", dataModel);

    ModbusModel *modbusModel = new ModbusModel(&app);
    qmlRegisterSingletonInstance("com.cmp.ModbusModel", 1, 0, "ModbusModel", modbusModel);

    const QUrl url(u"qrc:/EMpro/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
