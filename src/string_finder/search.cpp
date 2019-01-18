//
// Created by georgiy on 17.01.19.
//

#include "search.h"
#include "index.h"
#include <QFileInfo>

SearchEngine::SearchEngine(QString const &pattern, IndexEngine *index) :
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

std::optional<size_t> SearchEngine::first_occurrence(QFile& file)
{
    constexpr size_t BUFFER_SIZE = 1024 * 1024 * 4;
    // TODO: REWRITE THIS
    if (!QFileInfo(file).isFile() || !file.open(QIODevice::ReadOnly))
        return {};

    char* buffer = new char[BUFFER_SIZE];
    qint64 read_bytes = file.read(buffer, BUFFER_SIZE);
    char* pattern_bytes = pattern.toUtf8().data();
    size_t result = 0;

    while (true)
    {
        if (read_bytes == 0)
            break;
        if (stop_required || read_bytes == -1)
        {
            delete[] buffer;
            return {};
        }

        size_t j = 0;
        for (size_t i = 0; i < std::min(static_cast<qint64>(BUFFER_SIZE), read_bytes) - pattern.size() + 1; ++i)
        {
            if (stop_required)
            {
                delete[] buffer;
                return {};
            }

            if (pattern_bytes[j] == buffer[i])
            {
                ++j;
                if (j == pattern.size())
                {
                    delete[] buffer;
                    return result;
                }
            }
            else
            {
                j = 0;
            }
            ++result;
        }

        memmove(buffer, buffer + BUFFER_SIZE - pattern.size() + 1, static_cast<size_t>(pattern.size() - 1));
        read_bytes = file.read(buffer + pattern.size() - 1, static_cast<qint64>(BUFFER_SIZE - pattern.size() + 1));
    }

    delete[] buffer;
    return {};
}

void SearchEngine::start()
{
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
            emit found(QFileInfo(file).absoluteFilePath(), QString::number(first_index.value()));
        emit files_processed(++counter);
    }

    emit finished();
}

void SearchEngine::stop()
{
    stop_required = true;
}
