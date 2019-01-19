//
// Created by georgiy on 17.01.19.
//

#include "search.h"
#include "index.h"
#include <QFileInfo>
#include <QThread>

SearchEngine::SearchEngine(QString const &pattern, IndexEngine *index) :
    stop_required(false),
    running(false),
    index(index),
    pattern(pattern)
{}


std::list<QFile> SearchEngine::potential_files()
{
    std::list<QFile> result;
    auto trigrams = SearchEngine::get_trigrams(pattern);
    index->mutex.lock();
    for (auto const& file_trigrams_pair : index->file_trigrams)
    {
        if (stop_required)
            break;

        bool flag = true;
        for (auto const& trigram : trigrams)
            if (file_trigrams_pair.second.find(trigram) == file_trigrams_pair.second.end())
            {
                flag = false;
                break;
            }
        if (flag)
        {
            result.emplace_back(file_trigrams_pair.first);
        }
    }
    index->mutex.unlock();
    return result;
}

std::set<Trigram> SearchEngine::get_trigrams(QString const &str)
{
    std::set<Trigram> result;
    QByteArray const utf8data = str.toUtf8();
    if (utf8data.size() < 3)
        return {};

    auto get_trigram = [&utf8data](size_t index){
        return (static_cast<uint32_t>(utf8data[static_cast<int>(index)]) << (2 * sizeof(char))) |
               (static_cast<uint32_t>(utf8data[static_cast<int>(index) + 1]) << (sizeof(char))) |
               (static_cast<uint32_t>(utf8data[static_cast<int>(index) + 2]));
    };

    for (size_t i = 0; i < str.size() - 2; ++i)
    {
        result.insert(get_trigram(i));
    }

    return result;
}

std::optional<size_t> SearchEngine::first_occurrence(QFile& file) const
{
    constexpr size_t BUFFER_SIZE = 1024 * 1024 * 4;
    // TODO: REWRITE THIS
    if (!QFileInfo(file).isFile() || !file.open(QIODevice::ReadOnly))
        return {};

    char* buffer = new char[BUFFER_SIZE + 1];
    qint64 read_bytes = file.read(buffer, BUFFER_SIZE);
    buffer[read_bytes] = '\0';
    auto utf8 = pattern.toUtf8();
    char* pattern_bytes = new char[utf8.size() + 1];
    strcpy(pattern_bytes, utf8.data());
    size_t result = 0;

    while (true)
    {
        if (read_bytes == 0)
            break;
        if (stop_required || read_bytes == -1)
        {
            delete[] buffer;
            delete[] pattern_bytes;
            return {};
        }

        auto ptr = strstr(buffer, pattern_bytes);
        if (ptr)
        {
            size_t diff = ptr - buffer;
            delete[] buffer;
            delete[] pattern_bytes;
            return result + diff;
        }
        result += read_bytes;

        memmove(buffer, buffer + BUFFER_SIZE - utf8.size() + 1, static_cast<size_t>(utf8.size() - 1));
        read_bytes = file.read(buffer + utf8.size() - 1, static_cast<qint64>(BUFFER_SIZE - utf8.size() + 1));
        buffer[read_bytes + utf8.size()] = '\0';
    }

    delete[] buffer;
    delete[] pattern_bytes;
    return {};
}

void SearchEngine::start()
{
    running = true;
    emit started();
    auto files = potential_files();
    if (stop_required)
    {
        emit finished();
        return;
    }
    emit files_count(files.size());

    size_t counter = 0;
    for (auto& file : files)
    {
        if (stop_required)
            break;

        auto first_index = first_occurrence(file);
        if (first_index)
        {
            auto path = QFileInfo(file).absoluteFilePath();
            emit found(path, slice(path, first_index.value()));
        }
        emit files_processed(++counter);
    }

    emit finished();
    running = false;
}

void SearchEngine::stop()
{
    stop_required = true;
}

QString SearchEngine::slice(QFile file, size_t index, size_t pre_size, size_t post_size) const
{
    if (!QFileInfo(file).isFile() || !file.open(QIODevice::ReadOnly))
        return "<Unable to open file>";
    auto begin = static_cast<qint64>(index >= pre_size ? index - pre_size : 0);
    auto end = static_cast<qint64>(
            index + pattern.toUtf8().size() + post_size < file.size() ? index + pattern.toUtf8().size() +
            post_size : file.size());
    qint64 length = end - begin;
    if (length < 0 || !file.seek(begin))
        return "<Unable to get slice of file>";
    return QString::fromUtf8(file.read(length));
}
