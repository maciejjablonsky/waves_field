module;
#include <print>
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQuickView>
#include <windows.h>

export module wf.ui;

namespace wf
{
export struct window_handle
{
    unsigned long long id;
};

void traverseAndPrint(QObject* obj, int depth = 0)
{
    std::string indent(depth * 2, ' ');
    std::string objectName = obj->objectName().toStdString();
    std::string className  = obj->metaObject()->className();
    std::println("{}{} ({})", indent, objectName, className);

    for (QObject* child : obj->children())
    {
        traverseAndPrint(child, depth + 1);
    }
}

export class ui
{
  private:
    QGuiApplication gui_application_;
    QWindow main_window_;
    QQmlApplicationEngine qml_engine_;


  public:
    ui(int argc, char** argv)
        : gui_application_(argc, argv)
    {
        qml_engine_.load(QUrl("qrc:/qt/qml/WfUI/qml/Root.qml"));

        if (qml_engine_.rootObjects().isEmpty()) {
            throw std::runtime_error("Failed to load QML!");
        }

        // Get the QML window
        // QObject* rootObject = qml_engine_.rootObjects().first();
        // QQuickWindow* qml_window = qobject_cast<QQuickWindow*>(rootObject);
        //
        // if (!qml_window) {
        //     throw std::runtime_error("Root object is not a QQuickWindow!");
        // }
        //
        // // Set the QML window properties
        // qml_window->setColor(Qt::transparent);
        // // qml_window->setFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
        // qml_window->setFlags( Qt::FramelessWindowHint);
        // qml_window->show();
        //
        // // Synchronize Vulkan window size with QML window
        // QObject::connect(qml_window, &QQuickWindow::widthChanged, [this, qml_window] {
        //     main_window_.resize(qml_window->size());
        // });
        // QObject::connect(qml_window, &QQuickWindow::heightChanged, [this, qml_window] {
        //     main_window_.resize(qml_window->size());
        // });
        // QObject::connect(main_window_, &QWindow::xChanged, [this]{
        //     QObject* rootObject = qml_engine_.rootObjects().first();
        //     QQuickWindow* qml_window = qobject_cast<QQuickWindow*>(rootObject);
        // }, );
        // main_window_.show();
        //
        // // Show Vulkan window
        // main_window_.show();

// #ifdef _WIN32
//         // Ensure QML overlays Vulkan on Windows
//         HWND qmlHwnd = reinterpret_cast<HWND>(qml_window->winId());
//         HWND vulkanHwnd = reinterpret_cast<HWND>(main_window_.winId());
//         SetWindowPos(qmlHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
// #endif
    }

    auto run() -> int
    {
        return gui_application_.exec();
    }

    [[nodiscard]] auto handle() const -> window_handle
    {
        return {main_window_.winId()};
    }
};
} // namespace wf
