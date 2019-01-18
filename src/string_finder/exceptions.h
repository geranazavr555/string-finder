#ifndef HW1_EXCEPTIONS_H
#define HW1_EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include <QString>

class FilesystemException : public std::runtime_error
{
private:
    static constexpr auto message_template = "Filesystem exception on file ";

    std::string path;

public:
    explicit FilesystemException(std::string const& path) : runtime_error(message_template + path) {}
    explicit FilesystemException(const char* message, std::string const& path) :
            runtime_error(message + path)
    {}

    std::string const& get_path() const
    {
        return path;
    }
};

class UnableToOpenFileException : public FilesystemException
{
private:
    static constexpr auto message_template = "Unable to open file ";

public:
    explicit UnableToOpenFileException(std::string const& path):
            FilesystemException(message_template, path)
    {}
};

class FileNotFoundException : public FilesystemException
{
private:
    static constexpr auto message_template = "Could not find file ";

public:
    explicit FileNotFoundException(std::string const& path):
            FilesystemException(message_template, path)
    {}
};
#endif //HW1_EXCEPTIONS_H
