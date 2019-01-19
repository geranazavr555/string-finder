#include "gtest/gtest.h"

#include "generator.h"
#include "string_finder/index.h"
#include "string_finder/text_deteminer.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <QObject>

using namespace std;

extern FilesGenerator* fg;

class fileComparatorTests : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        text_checker = new TextFileDeterminer();
    }

    static void TearDownTestCase()
    {
        delete text_checker;
    }

    static TextFileDeterminer* text_checker;
};
TextFileDeterminer* fileComparatorTests::text_checker = nullptr;

TEST_F(fileComparatorTests, empty)
{
    ASSERT_TRUE(text_checker->check(fg->get_map_iterator("empty")));
}

TEST_F(fileComparatorTests, smallTexts)
{
    ASSERT_TRUE(text_checker->check(fg->get_map_iterator("small1")));
    ASSERT_TRUE(text_checker->check(fg->get_map_iterator("small2")));
    ASSERT_TRUE(text_checker->check(fg->get_map_iterator("small256a")));
}

class indexTests : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        index = new IndexEngine(fg->generated_root());
        index->build_index();
    }

    static void TearDownTestCase()
    {
        delete index;
    }

    static IndexEngine* index;
};
IndexEngine* indexTests::index = nullptr;

TEST_F(indexTests, updateDirectoryNoThrow)
{
    index->update_directory(fg->generated_root());
    ASSERT_NO_THROW();
}

TEST_F(indexTests, updateFileNoThrow)
{
    ASSERT_NO_THROW(index->update_file(fg->path("empty")));
    ASSERT_NO_THROW(index->update_file(fg->path("small1")));
}