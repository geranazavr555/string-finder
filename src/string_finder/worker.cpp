//
// Created by georgiy on 18.01.19.
//

#include "worker.h"
#include <QDirIterator>

IndexWorker::IndexWorker(QObject *parent) : QObject(parent)
{
    connect(&watcher, SIGNAL(fileChanged(QString const&)), this, SLOT(file_changed(QString const&)));
    connect(&watcher, SIGNAL(directoryChanged(QString const&)), this, SLOT(directory_changed(QString const&)));
}

void IndexWorker::set_directory(QString const &path)
{
    index = std::make_unique<IndexEngine>(path);
    connect(index.get(), &IndexEngine::files_processed, this, &IndexWorker::files_processed_slot);

    emit started();

    reset_watcher();
    recursive_subscribe(path);
    index->build_index();

    emit finished();
}

size_t IndexWorker::recursive_subscribe(QString const &path)
{
    QDirIterator it(path, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    QStringList files, dirs;
    dirs.push_back(QFileInfo(path).absoluteFilePath());
    while (it.hasNext())
    {
        QFileInfo info(it.next());
        if (info.isFile())
            files.push_back(info.absoluteFilePath());
        else if (info.isDir())
            dirs.push_back(info.absoluteFilePath());
    }
    emit files_counted(static_cast<size_t>(files.size()));
    return static_cast<size_t>(watcher.addPaths(files).size()) + static_cast<size_t>(watcher.addPaths(dirs).size());
}

void IndexWorker::file_changed(QString const &path)
{
    QFileInfo info(path);
    QString file_path = info.absoluteFilePath();
    if (!info.exists())
    {
        watcher.removePath(file_path);
        index->delete_file(file_path);
    }
    else
    {
        index->update_file(file_path);
    }
}

void IndexWorker::directory_changed(QString const &path)
{
    QFileInfo info(path);
    QString dir_path = info.absoluteFilePath();
    if (!info.exists())
        watcher.removePath(dir_path);
    else
        index->update_directory(dir_path);
}

void IndexWorker::reset_watcher()
{
    watcher.removePaths(watcher.files());
    watcher.removePaths(watcher.directories());
}

void IndexWorker::files_processed_slot(size_t count)
{
    emit files_processed(count);
}

std::unique_ptr<SearchEngine> IndexWorker::get_searcher(QString const &pattern)
{
    return index->get_searcher(pattern);
}
