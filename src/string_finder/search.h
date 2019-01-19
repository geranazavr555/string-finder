#ifndef STRING_FINDER_SEARCH_H
#define STRING_FINDER_SEARCH_H

#include "general_declarations.h"
#include <QString>
#include <QFile>
#include <vector>
#include <set>

class IndexEngine;

class SearchEngine : public QObject
{
    Q_OBJECT

public:
    SearchEngine(QString const &pattern, IndexEngine* index);

private:
    std::list<QFile> potential_files();
    static std::set<Trigram> get_trigrams(QString const& str);
    std::optional<size_t> first_occurrence(QFile& file) const;
    QString slice(QFile file, size_t index, size_t pre_size = 10, size_t post_size = 10) const;

public slots:
    void start();
    void stop();

signals:
    void started();
    void finished();
    void found(QString const& file, QString const& slice);
    void files_count(size_t);
    void files_processed(size_t);

private:
    bool stop_required;
    IndexEngine* index;
    QString pattern;
};

#endif //STRING_FINDER_SEARCH_H
