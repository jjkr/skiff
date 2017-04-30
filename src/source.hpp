#pragma once
#include "util/string_view.hpp"
#include <cassert>
#include <map>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace sk
{
/**
 * SourceBuffer
 *
 * Owns utf8 in-memory source data. Maintains a sequence of memory blocks of variable size.
 */
class SourceBuffer
{
public:
    /**
     * Constructs a SourceBuffer with a copy of the input string
     */
    static std::unique_ptr<SourceBuffer> fromSourceStr(string_view src);
    static std::unique_ptr<SourceBuffer> readFile(const char* filename);

    static constexpr auto DEFAULT_BLOCK_SIZE = 4 << 10;
    std::vector<char>& makeBlock(size_t size = DEFAULT_BLOCK_SIZE);
    void addBlock(std::vector<char>&& block);
    void addBlock(string_view s);

    std::vector<std::vector<char>>& getBlocks() { return m_blocks; }

    size_t size() const { return m_totalSize; }

    char getChar(size_t byteOffset) const;
    string_view getString(size_t byteOffset, size_t size);


    // Iterator
    template <typename Char>
    class Iterator {
    public:
        Iterator(const std::vector<std::vector<char>>& blocks, size_t blockOffset = 0)
            : m_blocks(&blocks), m_blockOffset(blockOffset)
        {
        }

        Iterator& operator++()
        {
            ++m_byteOffset;
            auto currentBlockSize = (*m_blocks)[m_blockOffset].size();
            if (m_byteOffset == currentBlockSize)
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
            return m_blocks == other.m_blocks && m_blockOffset == other.m_blockOffset &&
                   m_byteOffset == other.m_byteOffset;
        }
        bool operator!=(Iterator other) const { return !(*this == other); }
        char operator*() {return (*m_blocks)[m_blockOffset][m_byteOffset];}
        // iterator traits
        using difference_type = std::ptrdiff_t;
        using value_type = Char;
        using pointer = Char*;
        using reference = Char&;
        using iterator_category = std::forward_iterator_tag;

    private:
        const std::vector<std::vector<char>>* m_blocks;
        size_t m_blockOffset;
        size_t m_byteOffset = 0;
    };
    using iterator = Iterator<char>;
    iterator begin() { return iterator(m_blocks); }
    iterator end() { return iterator(m_blocks, m_blocks.size()); }
    using const_iterator = Iterator<const char>;
    const_iterator cbegin() const { return const_iterator(m_blocks); }
    const_iterator cend() const { return const_iterator(m_blocks, m_blocks.size()); }

private:
    struct BlockAndOffset
    {
        size_t block;
        size_t offset;
    };
    BlockAndOffset byteToBlockOffset(size_t byteOffset) const;

    struct PairHash
    {
    public:
        template <typename T, typename U>
        std::size_t operator()(const std::pair<T, U> &x) const
        {
            return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
        }
    };

    size_t m_totalSize = 0;
    //std::vector<size_t> m_newLineOffsets;
    std::vector<std::vector<char>> m_blocks;
    std::unordered_map<std::pair<size_t, size_t>, string_view, PairHash> m_coalesceSet;
    std::vector<std::vector<char>> m_coalesceOwners;
};


}