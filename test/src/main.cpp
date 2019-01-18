#include <stdio.h>
#include <gtest/gtest.h>
#include "generator.h"
#include <functional>

FilesGenerator* fg = nullptr;

using FG = FilesGenerator;

class Environment : public ::testing::Environment
{
public:
    virtual ~Environment()
    {
        delete fg;
    };

    virtual void SetUp()
    {
        fg = new FilesGenerator();

        std::function<QByteArray(char, size_t)> repeat = [](char x, size_t count) -> QByteArray {
            QByteArray a;
            for (size_t i = 0; i < count; ++i)
                a.append(x);
            return a;
        };

        fg->declare("empty", FG::wrap(""));
        fg->declare("small1", FG::wrap("abcabcabc"));
        fg->declare("small2", FG::wrap("qwertyuiopasd"));
        fg->copy("empty2", "empty", "x/empty");
        fg->copy("small1a", "small1", "x/smalla");
        fg->copy("small2a", "small2", "x/smallb");

        fg->declare("small256a", std::bind(repeat, 'a', 256));
        fg->declare("small256b", std::bind(repeat, 'b', 256));
        fg->copy("small256ac", "small256a", "x/y/s256a");
        fg->copy("small256bc", "small256b", "x/y/s256b");

        fg->declare("1Ma", std::bind(repeat, 'a', 1024 * 1024));
        fg->declare("1Mb", std::bind(repeat, 'b', 1024 * 1024));
        fg->copy("1Mac", "1Ma", "x/z/1mac");
        fg->copy("1Mbc", "1Mb", "x/z/1mbc");

        fg->declare("biga", std::bind(repeat, 'a', 1024 * 1024 * 15));
        fg->declare("bigb", std::bind(repeat, 'b', 1024 * 1024 * 15));
        fg->copy("bigac", "biga", "bigs/bigac");
        fg->copy("bigbc", "bigb", "bigs/bigbc");

        fg->declare("trash1", FG::wrap("qerqerreqerrewqqerqwerqer"));
        fg->declare("trash2", FG::wrap("sfg1569196sf1dg61sdf5g1654sf5g451"), "x/trash");
        fg->declare("trash3", FG::wrap("2561fdgsgserg45"), "x/y/trash");

        fg->generate_files();
    }

    virtual void TearDown()
    {
        fg->clean_up();
    }
};

GTEST_API_ int main(int argc, char **argv)
{
    testing::AddGlobalTestEnvironment(new Environment);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
