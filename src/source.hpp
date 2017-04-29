#pragma once
#include "util/string_view.hpp"
#include <cassert>
#include <string>
#include <vector>

namespace sk
{
/**
 * SourceBuffer
 *
 * Owns all of the in memory source information
 */
class SourceBuffer
{
public:
    template <typename Char>
    class Iterator {
    public:
        Iterator(std::vector<std::vector<char>>& blocks, int blockOffset = 0)
            : m_blocks(&blocks), m_blockOffset(blockOffset)
        {
        }

        Iterator& operator++()
        {
            ++m_byteOffset;
            if (m_byteOffset == m_blocks[m_blockOffset].size())
            {
                ++m_blockOffset;
                m_byteOffset = 0;
            };
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator retval = *this;
            ++(*this);
            return retval;
        }
        bool operator==(Iterator other) const
        {
            return m_blocks == other.m_blocks == m_blockOffset == other.m_blockOffset &&
                   m_byteOffset == other.m_byteOffset;
        }
        bool operator!=(Iterator other) const { return !(*this == other); }
        Char operator*() {return (*m_blocks)[m_blockOffset][m_byteOffset];}
        // iterator traits
        using difference_type = std::ptrdiff_t;
        using value_type = Char;
        using pointer = Char*;
        using reference = Char&;
        using iterator_category = std::forward_iterator_tag;

    private:
        std::vector<std::vector<char>>* m_blocks = nullptr;
        size_t m_blockOffset;
        size_t m_byteOffset = 0;
    };
    using iterator = Iterator<char>;
    iterator begin() { return iterator(m_blocks); }
    iterator end() { return iterator(m_blocks); }
    using const_iterator = Iterator<const char>;
    const_iterator cbegin() { return const_iterator(m_blocks); }
    const_iterator cend() { return const_iterator(m_blocks); }

    static constexpr auto DEFAULT_BLOCK_SIZE = 4 << 10;
    std::vector<char>& makeBlock(size_t size = DEFAULT_BLOCK_SIZE);
    void addBlock(std::vector<char>&& block);
    void addBlock(const std::string& s);

    std::vector<std::vector<char>>& getBlocks() { return m_blocks; }

    size_t getTotalSize() { return m_totalSize; }

    char getChar(size_t byteOffset);
    string_view getString(size_t byteOffset, size_t size);

private:
    struct BlockAndOffset
    {
        size_t block;
        size_t offset;
    };
    BlockAndOffset byteToBlockOffset(size_t byteOffset);

    size_t m_totalSize = 0;
    std::vector<size_t> m_newLineOffsets;
    std::vector<std::vector<char>> m_blocks;
    std::vector<std::vector<char>> m_coalesceBlocks;
};
}