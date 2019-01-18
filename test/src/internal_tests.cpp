#include "gtest/gtest.h"

#include "generator.h"
#include "directory_scanner/worker.h"
#include <iostream>
#include <string>
#include <QObject>

using namespace std;

extern FilesGenerator* fg;

class fileComparatorTests : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        file_comparator = new FileComparator();
    }

    static void TearDownTestCase()
    {
        delete file_comparator;
    }

    static FileComparator* file_comparator;
};
FileComparator* fileComparatorTests::file_comparator = nullptr;


TEST_F(fileComparatorTests, emptyFilesEquality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("empty"), fg->path("empty2")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("empty2"), fg->path("empty")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("empty"), fg->path("empty")), true);
}

TEST_F(fileComparatorTests, smallFilesEquality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("small1"), fg->path("small1a")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("small2"), fg->path("small2a")), true);
}

TEST_F(fileComparatorTests, smallFilesInequality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("small1"), fg->path("small2a")), false);
    ASSERT_EQ(file_comparator->compare(fg->path("small2"), fg->path("small1a")), false);
}

TEST_F(fileComparatorTests, emptyAndNonEmpty)
{
    ASSERT_EQ(file_comparator->compare(fg->path("empty"), fg->path("small1")), false);
    ASSERT_EQ(file_comparator->compare(fg->path("small1"), fg->path("empty")), false);
}

TEST_F(fileComparatorTests, small256Equality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("small256a"), fg->path("small256ac")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("small256ac"), fg->path("small256a")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("small256b"), fg->path("small256bc")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("small256bc"), fg->path("small256b")), true);
}

TEST_F(fileComparatorTests, small256Inequality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("small256a"), fg->path("small256bc")), false);
    ASSERT_EQ(file_comparator->compare(fg->path("small256ac"), fg->path("small256b")), false);
    ASSERT_EQ(file_comparator->compare(fg->path("small256b"), fg->path("small256ac")), false);
    ASSERT_EQ(file_comparator->compare(fg->path("small256bc"), fg->path("small256a")), false);
}

TEST_F(fileComparatorTests, small1MbyteEquality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("1Ma"), fg->path("1Mac")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("1Mac"), fg->path("1Ma")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("1Mb"), fg->path("1Mbc")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("1Mbc"), fg->path("1Mb")), true);
}

TEST_F(fileComparatorTests, small1MbyteInequality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("1Ma"), fg->path("1Mbc")), false);
    ASSERT_EQ(file_comparator->compare(fg->path("1Mac"), fg->path("1Mb")), false);
}

TEST_F(fileComparatorTests, bigEquality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("biga"), fg->path("bigac")), true);
    ASSERT_EQ(file_comparator->compare(fg->path("bigb"), fg->path("bigbc")), true);
}

TEST_F(fileComparatorTests, bigInquality)
{
    ASSERT_EQ(file_comparator->compare(fg->path("biga"), fg->path("bigbc")), false);
    ASSERT_EQ(file_comparator->compare(fg->path("bigb"), fg->path("bigac")), false);
}

TEST_F(fileComparatorTests, differentFileSizes)
{
    ASSERT_EQ(file_comparator->compare(fg->path("small1"), fg->path("bigbc")), false);
    ASSERT_EQ(file_comparator->compare(fg->path("empty"), fg->path("1Ma")), false);
}

TEST_F(fileComparatorTests, invalidPath)
{
    try
    {
        file_comparator->compare(fg->path("small1"), "123");
        FAIL();
    }
    catch (std::exception const& err)
    {
        try
        {
            file_comparator->compare("321", fg->path("empty"));
            FAIL();
        }
        catch (std::exception const& err)
        {
            try
            {
                file_comparator->compare("321", "123");
                FAIL();
            }
            catch (std::exception const& err)
            {
                SUCCEED();
            }
            SUCCEED();
        }
        SUCCEED();
    }
}

TEST_F(fileComparatorTests, abortationReturnFalse)
{
    bool x = true;
    ASSERT_FALSE(file_comparator->compare(fg->path("biga"), fg->path("bigac"), &x));
}

class duplicateScannerTests : public ::testing::Test
{
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};

TEST_F(duplicateScannerTests, filesCount)
{
    DuplicateScanner scanner(fg->generated_root());
    ASSERT_EQ(fg->files_count(), scanner.count_files());
}

TEST_F(duplicateScannerTests, copiesGroupCount)
{
    DuplicateScanner scanner(fg->generated_root());
    ASSERT_EQ(fg->files_count(), scanner.count_files());
}

namespace
{
    class Checker : public QObject
    {
        Q_OBJECT

    };
}

TEST_F(duplicateScannerTests, correctness)
{

}