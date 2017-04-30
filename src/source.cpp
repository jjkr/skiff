#include "source.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

using std::copy;
using std::make_unique;
using std::move;
using std::runtime_error;
using std::string;
using std::vector;

namespace sk
{
std::unique_ptr<SourceBuffer> SourceBuffer::fromSourceStr(string_view src)
{
    auto buffer = make_unique<SourceBuffer>();
    buffer->addBlock(src);
    return buffer;
}

std::vector<char>& SourceBuffer::makeBlock(size_t size)
{
    m_blocks.emplace_back(size);
    m_totalSize += size;
    return m_blocks.back();
}

void SourceBuffer::addBlock(vector<char>&& block)
{
    if (block.empty())
    {
        return;
    }
    m_totalSize += block.size();
    m_blocks.push_back(move(block));
}

void SourceBuffer::addBlock(string_view s)
{
    if (s.empty())
    {
        return;
    }
    vector<char> block(s.size());
    copy(s.cbegin(), s.cend(), block.begin());
    m_totalSize += block.size();
    m_blocks.push_back(move(block));
}

char SourceBuffer::getChar(size_t byteOffset) const
{
    auto blockOffset = byteToBlockOffset(byteOffset);
    return m_blocks[blockOffset.block][blockOffset.offset];
}

string_view SourceBuffer::getString(size_t byteOffset, size_t sz) const
{
    if (sz == 0u)
    {
        return string_view();
    }
    if (byteOffset + sz > size())
    {
        return string_view();
    }
    auto firstBlock = byteToBlockOffset(byteOffset);
    auto lastBlock = byteToBlockOffset(byteOffset + sz - 1);
    if (firstBlock.block == lastBlock.block)
    {
        return string_view(m_blocks[firstBlock.block].data() + firstBlock.offset, sz);
    }
}

SourceBuffer::BlockAndOffset SourceBuffer::byteToBlockOffset(size_t byteOffset) const
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