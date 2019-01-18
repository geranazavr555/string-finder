#ifndef DUPLICATE_SCANNER_TEXT_DETEMINER_H
#define DUPLICATE_SCANNER_TEXT_DETEMINER_H

#include "general_declarations.h"
#include <cstddef>
#include <set>
#include <unordered_map>
#include <QFile>

class TextFileDeterminer
{
public:
    TextFileDeterminer();
    TextFileDeterminer(size_t trigram_limit);

    bool check(std::unordered_map<QString, std::set<Trigram> >::const_iterator index_iter);

private:
    size_t count_trigrams(std::unordered_map<QString, std::set<Trigram> >::const_iterator const& index_iter);

private:
    size_t trigram_limit;
};

#endif //DUPLICATE_SCANNER_TEXT_DETEMINER_H
