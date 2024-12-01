#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <cstdlib>
#include <entt/entt.hpp>
#include <filesystem>
#include <format>
#include <print>

#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QString>

// import utils;

void traverseQmlDirectory(const QString& directoryPath)
{
    QDirIterator it(directoryPath, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filePath = it.next();
        qDebug() << filePath;
    }
}
int main(int argc, char** argv)
{
    try
    {
        QGuiApplication app(argc, argv);

        traverseQmlDirectory(":/");

        QQmlApplicationEngine engine;

        engine.loadFromModule("app", "Main");

        if (engine.rootObjects().isEmpty())
        {
            return -1;
        }

        return app.exec();
    }
    catch (const std::exception& e)
    {
        std::print("{}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
