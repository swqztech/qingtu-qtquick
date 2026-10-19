#include "imagecontroller.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

int main(int argc, char* argv[]) {
  QGuiApplication app(argc, argv);
  QQuickStyle::setStyle(QStringLiteral("Basic"));
  app.setApplicationName(QStringLiteral("轻图"));
  app.setOrganizationName(QStringLiteral("swqztech"));

  ImageController controller;

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty(QStringLiteral("Qingtu"), &controller);
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule(QStringLiteral("QingtuApp"), QStringLiteral("Main"));

  return app.exec();
}
