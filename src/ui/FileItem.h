//
// Created by georgiy on 12.01.19.
//

#ifndef DUPLICATE_SCANNER_FILEITEM_H
#define DUPLICATE_SCANNER_FILEITEM_H

#include <QTreeWidgetItem>

class FileItem : public QTreeWidgetItem
{
public:
    template <class... Args>
    FileItem(Args&& ...args) : QTreeWidgetItem(std::forward<Args>(args)...) {}

    virtual bool operator<(const QTreeWidgetItem &other) const
    {
        auto list_this = this->text(1).split(' ');
        auto list_other = other.text(1).split(' ');
        return list_this.first().toInt() < list_other.first().toInt();
    }
};


#endif //DUPLICATE_SCANNER_FILEITEM_H
