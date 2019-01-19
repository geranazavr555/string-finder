#include "text_deteminer.h"
#include "general_declarations.h"
#include "exceptions.h"
#include <QFileInfo>
#include <set>

static constexpr size_t DEFAULT_TRIGRAM_LIMIT_COUNT = 10000;

TextFileDeterminer::TextFileDeterminer(size_t trigram_limit) :
    trigram_limit(trigram_limit)
{}

TextFileDeterminer::TextFileDeterminer() : TextFileDeterminer(DEFAULT_TRIGRAM_LIMIT_COUNT)
{}

bool TextFileDeterminer::check(std::unordered_map<QString, Trigrams>::const_iterator index_iter) const
{
    return TextFileDeterminer::count_trigrams(index_iter) <= trigram_limit &&
           index_iter->second.find(0) == index_iter->second.end();
}

size_t
TextFileDeterminer::count_trigrams(std::unordered_map<QString, Trigrams>::const_iterator const &index_iter)
{
    return index_iter->second.size();
}

