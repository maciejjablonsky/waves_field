#pragma once
#include <QtGui/QWindow>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>

namespace wf
{

class SynchronizedWindow : public QWindow {
    Q_OBJECT

  public:
    explicit SynchronizedWindow(QQmlApplicationEngine& qmlEngine)
        : qml_engine_(qmlEngine)
    {
        // Connect signals to handle window resizing
        connect(this, &QWindow::widthChanged, this, &SynchronizedWindow::syncQmlWindowGeometry);
        connect(this, &QWindow::heightChanged, this, &SynchronizedWindow::syncQmlWindowGeometry);
    }

    void initializeQmlOverlay(const QUrl& qmlSource)
    {
        show();
        // Load QML
        qml_engine_.load(qmlSource);
        if (qml_engine_.rootObjects().isEmpty()) {
            qFatal("Failed to load QML root object");
        }

        // Cast the root QML object to QQuickWindow
        qml_window_ = qobject_cast<QQuickWindow*>(qml_engine_.rootObjects().first());
        if (!qml_window_) {
            qFatal("Failed to cast QML root object to QQuickWindow");
        }

        // Configure QML window for transparency and overlay
        qml_window_->setColor(Qt::transparent);
        qml_window_->setFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput);
        qml_window_->setParent(this);
        syncQmlWindowGeometry(); // Set initial geometry
        qml_window_->show();
    }

  private slots:
    void syncQmlWindowGeometry()
    {
        if (qml_window_) {
            qml_window_->setGeometry(geometry());
        }
    }

  private:
    QQmlApplicationEngine& qml_engine_;
    QQuickWindow* qml_window_ = nullptr;
};
}