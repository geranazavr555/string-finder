#ifndef DUPLICATE_SCANNER_INDEX_H
#define DUPLICATE_SCANNER_INDEX_H

#include "text_deteminer.h"
#include "search.h"
#include "general_declarations.h"
#include <QString>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>

class IndexEngine : public QObject
{
    Q_OBJECT
    friend SearchEngine;

public:
    IndexEngine(QString directory);

    std::unique_ptr<SearchEngine> get_searcher(QString const& pattern);
    void build_index();
    void update_file(QString const& path);
    void update_directory(QString const& path);
    void delete_file(QString const& path);

private:
    void index_file(QFile file, bool reindex = false);

public slots:
    void stop();

signals:
    void files_processed(size_t);

private:
    bool stop_required;
    std::mutex mutex;
    QString directory;
    std::unordered_map<QString, std::set<Trigram> > file_trigrams;
    TextFileDeterminer text_checker;
};

#endif //DUPLICATE_SCANNER_INDEX_H
