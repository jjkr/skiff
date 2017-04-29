#include "source.hpp"
#include <stdexcept>
#include <utility>

using std::move;
using std::runtime_error;
using std::string;
using std::vector;

namespace sk
{
std::vector<char>& SourceBuffer::makeBlock(size_t size)
{
    m_blocks.emplace_back(size);
    m_totalSize += size;
    return m_blocks.back();
}

void SourceBuffer::addBlock(vector<char>&& block)
{
    m_totalSize += block.size();
    m_blocks.push_back(move(block));
}

void SourceBuffer::addBlock(const std::string& s)
{
    vector<char> block(s.begin(), s.end());
    m_totalSize += block.size();
    m_blocks.push_back(move(block));
}

char SourceBuffer::getChar(size_t byteOffset)
{
    auto blockOffset = byteToBlockOffset(byteOffset);
    return m_blocks[blockOffset.block][blockOffset.offset];
}

string_view SourceBuffer::getString(size_t byteOffset, size_t size)
{
    if (byteOffset + size > getTotalSize())
    {
        return string_view();
    }
    auto firstBlock = byteToBlockOffset(byteOffset);
    auto lastBlock = byteToBlockOffset(byteOffset + size);
    if (firstBlock.block == lastBlock.block)
    {
        return string_view(m_blocks[firstBlock.block].data() + firstBlock.offset, size);
    }
}

SourceBuffer::BlockAndOffset SourceBuffer::byteToBlockOffset(size_t byteOffset)
{
    for (auto i = 0ul; i < m_blocks.size(); ++i) {
        auto& b = m_blocks[i];
        if (byteOffset < b.size())
        {
            return SourceBuffer::BlockAndOffset{i, byteOffset};
        }
    }
    throw runtime_error("SourceBuffer offset out of range: " + byteOffset);
}
}