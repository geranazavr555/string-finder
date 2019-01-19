#include "index.h"
#include "exceptions.h"
#include <QDirIterator>
#include <QDebug>
#include <cmath>
#include <QThread>
#include <QDebug>

IndexEngine::IndexEngine(QString directory):
        stop_required(false),
        mutex(),
        directory(std::move(directory)),
        file_trigrams(),
        text_checker()
{}

std::unique_ptr<SearchEngine> IndexEngine::get_searcher(QString const &pattern)
{
    return std::make_unique<SearchEngine>(pattern, this);
}

void IndexEngine::build_index()
{
    QDirIterator it(directory, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    size_t counter = 0;
    while (it.hasNext())
    {
        if (stop_required)
            break;

        QString path = it.next();
        //qDebug() << path;
        if (QFileInfo(path).isFile())
        {
            try
            {
                index_file(path);
            }
            catch (std::exception const &err)
            {
                qWarning() << err.what();
            }
            emit files_processed(++counter);
        }
    }
}

void IndexEngine::index_file(QFile file, bool reindex)
{
    constexpr size_t BUFFER_SIZE = 1024 * 1024 * 4;
    QFileInfo file_info(file);
    QString file_path = file_info.absoluteFilePath();

    if (!file_info.isFile())
    {
        if (reindex)
            delete_file(file_path);
        throw FileNotFoundException(file_path.toStdString());
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        if (reindex)
            delete_file(file_path);
        throw UnableToOpenFileException(file_path.toStdString());
    }

    char* buffer = new char[BUFFER_SIZE];

    auto get_trigram = [&buffer](size_t index){
        return (static_cast<uint32_t>(buffer[index]) << (2 * sizeof(char))) |
               (static_cast<uint32_t>(buffer[index + 1]) << (sizeof(char))) |
               (static_cast<uint32_t>(buffer[index + 2]));
    };

    if (reindex)
        delete_file(file_path);
    qint64 read_bytes = file.read(buffer, BUFFER_SIZE);
    auto iter = file_trigrams.end();
    bool first_read = true;
    mutex.lock();
    while (true)
    {
        //qDebug() << "pack";
        if ((!reindex && stop_required) || read_bytes == 0 || QThread::currentThread()->isInterruptionRequested())
            break;
        if (read_bytes == -1)
        {
            mutex.unlock();
            delete_file(file_path);
            delete[] buffer;
            throw FilesystemException(file_path.toStdString());
        }
        if (first_read && read_bytes < 3)
        {
            file_trigrams[file_path] = {};
            iter = file_trigrams.find(file_path);
            break;
        }

        for (size_t i = 0; i < std::min(BUFFER_SIZE, static_cast<size_t>(read_bytes)) - 2; ++i)
        {
            //qDebug() << i;
            if ((!reindex && stop_required) || QThread::currentThread()->isInterruptionRequested())
                break;
            file_trigrams[file_path].insert(get_trigram(i));
        }

        if (iter == file_trigrams.end())
            iter = file_trigrams.find(file_path);
        if (!text_checker.check(iter))
            break;

        if ((!reindex && stop_required) || QThread::currentThread()->isInterruptionRequested())
            break;

        memmove(buffer, buffer + BUFFER_SIZE - 2, 2);
        read_bytes = file.read(buffer + 2, BUFFER_SIZE - 2);
        first_read = false;
    }
    mutex.unlock();

    if (iter == file_trigrams.end() || !text_checker.check(iter))
        delete_file(file_path);

    delete[] buffer;
}

void IndexEngine::update_file(QString const &path)
{
    try
    {
        index_file(path, true);
    }
    catch (std::exception const& err)
    {
        file_trigrams[path].clear();
        file_trigrams.erase(path);
        qWarning() << err.what();
    }
}

void IndexEngine::delete_file(QString const &path)
{
    mutex.lock();
    file_trigrams.erase(path);
    mutex.unlock();
}

void IndexEngine::update_directory(QString const &path)
{
    QDir dir(path);
    if (dir.exists())
    {
        for (auto const& file_path : dir.entryList(QDir::Files | QDir::Readable))
        {
            if (QThread::currentThread()->isInterruptionRequested())
                break;
            update_file(QFileInfo(dir, file_path).absoluteFilePath());
        }
    }
}

void IndexEngine::stop()
{
    qDebug() << "Stop flag set";
    stop_required = true;
}
