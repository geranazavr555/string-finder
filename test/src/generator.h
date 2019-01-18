#ifndef DUPLICATE_SCANNER_TESTS_GENERATOR_H
#define DUPLICATE_SCANNER_TESTS_GENERATOR_H

#include <QString>
#include <QDir>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <vector>

namespace std
{
    template <> struct hash<QString>
    {
        size_t operator()(const QString & x) const
        {
            return qHash(x);
        }
    };
}

class FilesGenerator
{
private:
    char const * ERROR_MSG = "Can't generete files to tests. "
                             "Check if test executable have permissions to write in working directory";

    int files_cnt;
    int copy_buckets;
    QString working_dir;
    std::unordered_map<QString, QString> aliases;
    std::unordered_map<QString, QString> pathes;
    std::unordered_map<QString, std::function<QByteArray()> > generators;
    std::unordered_map<QString, QString> copyof;
    std::unordered_map<QString, std::vector<QString> > copies;

private:
    QString rel_path(QString const& alias)
    {
        return "tests_gen/" + aliases[alias];
    }

    void dfs(const QString &alias, std::vector<QString> &result)
    {
        result.push_back(alias);
        if (copies[alias].empty())
            return;
        for (auto const& copy : copies[alias])
            dfs(copy, result);
    }

public:
    FilesGenerator(QString working_dir) : files_cnt(0), copy_buckets(-1), working_dir(std::move(working_dir)) {}
    FilesGenerator() : FilesGenerator(QDir::currentPath()) {}

    ~FilesGenerator()
    {
        if (!aliases.empty())
            clean_up();
    }

    QString path(QString const &alias)
    {
        return QDir(working_dir + "/tests_gen").absoluteFilePath(aliases[alias]);
    }

    QString alias(QString const& path)
    {
        return pathes[path];
    }

    int count_buckets()
    {
        if (copy_buckets != -1)
            return copy_buckets;

        std::unordered_map<QString, int> identity_index;
        int ind = 0;
        for (auto const& alias_path : aliases)
        {
            auto files = get_all_same_files(alias_path.first);
            bool flag = true;
            for (auto const& file : files)
            {
                if (identity_index.find(file) != identity_index.end())
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                for (auto const& file : files)
                {
                    identity_index[file] = ind;
                }
                ++ind;
            }
        }
        return copy_buckets = ind;
    }

    void declare(QString alias, std::function<QByteArray()> const &data_generator, QString file_name = "")
    {
        if (file_name.isEmpty())
            file_name = alias;
        if (!data_generator)
            throw std::logic_error("data_generator have to be specified");
        aliases[alias] = file_name;
        generators[alias] = data_generator;
        pathes[path(alias)] = alias;
    }

    void copy(QString new_alias, QString alias, QString file_name = "")
    {
        if (file_name.isEmpty())
            file_name = aliases[alias];
        aliases[new_alias] = file_name;
        generators[new_alias] = generators[alias];
        pathes[path(alias)] = new_alias;
        copyof[new_alias] = alias;
        copies[alias].push_back(new_alias);
    }

    std::vector<QString> get_all_same_files(const QString &alias)
    {
        auto iter = copyof.find(alias);
        while (iter != copyof.end())
        {
            iter = copyof.find(iter->second);
        }
        std::vector<QString> result;
        dfs(iter->second, result);
        return result;
    }

    void generate_files()
    {
        QDir dir(working_dir);
        if (!dir.exists("tests_gen"))
            if (!dir.mkdir("tests_gen"))
                throw std::runtime_error(ERROR_MSG);

        for (auto const& alias_path_pair : aliases)
        {
            QString path = rel_path(alias_path_pair.first);
            QFileInfo file_info(path);
            if (!dir.mkpath(file_info.path()))
                throw std::runtime_error(ERROR_MSG);

            QFile file(file_info.filePath());
            if (!file.open(QIODevice::WriteOnly))
                throw std::runtime_error(ERROR_MSG);

            if (file.write(generators[alias_path_pair.first]()) == -1)
                throw std::runtime_error(ERROR_MSG);

            ++files_cnt;
        }
    }

    void clean_up()
    {
        QDir dir(working_dir);
        if (!dir.exists("tests_gen"))
            return;

        if (!dir.cd("tests_gen"))
            return;

        for (auto const& alias_path_pair : aliases)
        {
            QString path = QFileInfo(rel_path(alias_path_pair.first)).filePath();
            QFile file(path);
            if (!file.exists())
                continue;

            if (!file.remove())
                std::cerr << "Can't delete file: " + path.toStdString() << std::endl;
        }

        aliases.clear();
    }

    int files_count()
    {
        return files_cnt;
    }

    QString generated_root()
    {
        return working_dir + "/tests_gen";
    }

    static std::function<QByteArray()> wrap(QString const &s)
    {
        return [s](){
            return s.toUtf8();
        };
    }
};

#endif //DUPLICATE_SCANNER_TESTS_GENERATOR_H
