#include "source.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>
#include <vector>

using sk::SourceBuffer;
using std::equal;
using std::move;
using std::string;
using std::vector;

TEST(SourceBuffer, constructs)
{
    SourceBuffer buffer;
}

TEST(SourceBuffer, makesBlock)
{
    SourceBuffer buffer;
    auto size = 782ul;
    vector<char>& block = buffer.makeBlock(size);
    EXPECT_EQ(size, block.size());
}

TEST(SourceBuffer, addsBlock)
{
    SourceBuffer buffer;
    auto size = 12ul;
    vector<char> newBlock(size, '!');
    buffer.addBlock(move(newBlock));
    EXPECT_EQ(size, buffer.size());
}

TEST(SourceBuffer, addsString)
{
    SourceBuffer buffer;
    string s("foobar");
    buffer.addBlock(s);
    EXPECT_EQ(s.size(), buffer.size());
}

TEST(SourceBuffer, iterates)
{
    SourceBuffer buffer;
    string s("foobar");
    buffer.addBlock(s);
    EXPECT_TRUE(equal(buffer.begin(), buffer.end(), s.begin()));
    EXPECT_TRUE(equal(buffer.cbegin(), buffer.cend(), s.cbegin()));
}

TEST(SourceBuffer, getChar)
{
    SourceBuffer buffer;
    string s("foobar");
    buffer.addBlock(s);
    EXPECT_EQ('o', buffer.getChar(2));
}

TEST(SourceBuffer, getString)
{
    SourceBuffer buffer;
    string s("The quick brown fox jumped over the lazy dogs");
    buffer.addBlock(s);
    EXPECT_EQ("quick", buffer.getString(4, 5));
}
