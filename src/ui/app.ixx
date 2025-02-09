module;
#include <print>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlExtensionPlugin>
#include <QQuickView>
#include <QtPlugin>
#include <ranges>
#include <set>
export module wf.ui.app;

namespace wf::ui
{
};

void traverseQmlDirectory(const QString& directoryPath)
{
    QDirIterator it(directoryPath, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filePath = it.next();
        qDebug() << filePath;
    }
}

class qml_url_interceptor : public QQmlAbstractUrlInterceptor
{
  public:
    using watched_modules = std::unordered_map<QString, QDir>;
    using watched_dirs    = std::set<QString>;

  private:
    watched_modules modules_to_be_replaced_;
    watched_dirs watched_dirs_;

  public:
    static qml_url_interceptor& instance()
    {
        static qml_url_interceptor instance;
        return instance;
    }

    QUrl intercept(const QUrl& url, DataType type) override
    {
        if (type != QmlFile and type != UrlString)
        {
            return url;
        }
        QString qrc_path = url.path();

        if (qrc_path.startsWith('/'))
        {
            qrc_path.remove(0, 1);
        }

        for (const auto& [module, module_dir] : modules_to_be_replaced_)
        {
            if (qrc_path.startsWith(module))
            {
                qrc_path.remove(module + '/');
                const QString new_path = module_dir.absoluteFilePath(qrc_path);
                QString enclosing_folder = QFileInfo(new_path).dir().path();
                watched_dirs_.insert(enclosing_folder);
                return QUrl::fromLocalFile(new_path);
            }
        }
        return url;
    }

    void reset_watched_dirs()
    {
        watched_dirs_.clear();
    }

    [[nodiscard]] watched_dirs get_watched_dirs() const
    {
        return watched_dirs_;
    }
};

class qml_hotreload : public QObject
{
    Q_OBJECT
  public:
    explicit qml_hotreload(QQmlEngine& engine,
                           QQuickView& view,
                           QUrl root_qml,
                           QObject* parent = nullptr)
        : QObject(parent), engine_(engine), view_(view), watcher_(this),
          root_qml_(std::move(root_qml))
    {
        connect(&watcher_,
                &QFileSystemWatcher::directoryChanged,
                this,
                [this](const QString&) { load_(); });
        engine_.addUrlInterceptor(&qml_url_interceptor::instance());
        load_();
    }

  private:
    void load_()
    {
        engine_.clearComponentCache();
        auto& interceptor = qml_url_interceptor::instance();
        interceptor.reset_watched_dirs();
        view_.setSource(root_qml_);
        std::ranges::for_each(
            interceptor.get_watched_dirs(),
            [this](const auto& dir) { watcher_.addPath(dir); });

        if (view_.errors().isEmpty())
        {
            return;
        }

        auto errors_as_strings =
            view_.errors() | std::views::transform([](const QQmlError& error) {
                return error.toString().toStdString();
            });
        std::ranges::for_each(errors_as_strings, [](const std::string& error) {
            std::print(error);
        });
    }

    QQmlEngine& engine_;
    QQuickView& view_;
    QFileSystemWatcher watcher_;
    QUrl root_qml_;
    QDir watched_dir_;
};

void app::run()
{
    // traverseQmlDirectory(":/");
    const QUrl url(QStringLiteral("qrc:/qt/qml/WfUI/qml/Root.qml"));
    QObject::connect(
        view_.engine(),
        &QQmlApplicationEngine::objectCreated,
        &gui_app_,
        [url](QObject* obj, const QUrl& objUrl) {
            if (obj == nullptr and url == objUrl)
            {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);
    view_.engine()->load(url);

    if (engine_.rootObjects().isEmpty())
    {
        std::println("failed to run qml views");
    }

    gui_app_.gui_app_.exec();
}
} // namespace wf::ui