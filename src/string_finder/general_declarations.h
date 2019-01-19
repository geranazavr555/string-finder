#ifndef DUPLICATE_SCANNER_GENERAL_DECLARATIONS_H
#define DUPLICATE_SCANNER_GENERAL_DECLARATIONS_H

#include <unordered_set>
#include <QString>
#include <cstdint>
#include <QHash>

using Trigram = uint32_t;
using Trigrams = std::unordered_set<Trigram>;

namespace std {
    template <>
    struct hash<QString>
    {
        size_t operator()(const QString & x) const
        {
            return qHash(x);
        }
    };
}


#endif //DUPLICATE_SCANNER_GENERAL_DECLARATIONS_H
