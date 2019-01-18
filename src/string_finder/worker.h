#ifndef STRING_FINDER_WORKER_H
#define STRING_FINDER_WORKER_H

#include "index.h"
#include <QObject>
#include <QFileSystemWatcher>
#include <memory>

class IndexWorker : public QObject
{
    Q_OBJECT

public:
    IndexWorker(QObject *parent = nullptr);
    std::unique_ptr<SearchEngine> get_searcher(QString const& pattern);

private:
    size_t recursive_subscribe(QString const& path);
    void reset_watcher();

public slots:
    void set_directory(QString const& path);

private slots:
    void directory_changed(QString const& path);
    void file_changed(QString const& path);

    void files_processed_slot(size_t);

signals:
    void files_counted(size_t);
    void files_processed(size_t);
    void started();
    void finished();

private:
    QFileSystemWatcher watcher;
    std::unique_ptr<IndexEngine> index;
};

#endif //STRING_FINDER_WORKER_H
