#include "source.hpp"
#include <algorithm>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <utility>

using std::copy;
using std::make_unique;
using std::move;
using std::make_pair;
using std::ostringstream;
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

SourceBuffer SourceBuffer::readFile(const char* filename)
{
    SourceBuffer buffer;
    auto f = fopen(filename, "r");
    if (!f)
    {
        ostringstream ss;
        ss << "Failed to open file " << filename;
        throw runtime_error(ss.str());
    }
    while (true)
    {
        auto& block = buffer.makeBlock();
        auto bytesRead = fread(block.data(), 1, block.size(), f);
        if (bytesRead != block.size())
        {
            block.resize(bytesRead);
            break;
        }
    }

    return buffer;
}

std::vector<char>& SourceBuffer::makeBlock(size_t size)
{
    if (size == 0)
    {
        throw runtime_error("Cannot make empty block");
    }
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

string_view SourceBuffer::getString(size_t byteOffset, size_t sz)
{
    if (sz == 0u)
    {
        return string_view();
    }
    if (byteOffset + sz > size())
    {
        return string_view();
    }
    auto firstOffset = byteToBlockOffset(byteOffset);
    auto lastOffset = byteToBlockOffset(byteOffset + sz - 1);
    if (firstOffset.block == lastOffset.block)
    {
        return string_view(m_blocks[firstOffset.block].data() + firstOffset.offset, sz);
    }
    else
    {
        auto key = make_pair(byteOffset, sz);
        auto cached = m_coalesceSet.find(key);
        if (cached != m_coalesceSet.end())
        {
            return cached->second;
        }
        else
        {
            m_coalesceOwners.emplace_back(sz);
            auto& coalesced = m_coalesceOwners.back();

            const auto& firstBlock = m_blocks[firstOffset.block];
            const auto firstBase = firstBlock.cbegin();
            auto coalescedIter = coalesced.begin();
            copy(firstBase + firstOffset.offset, firstBase + firstBlock.size(), coalescedIter);
            coalescedIter += firstBlock.size() - firstOffset.offset;
            ++firstOffset.block;

            while (firstOffset.block != lastOffset.block)
            {
                auto fullBlock = m_blocks[firstOffset.block];
                copy(fullBlock.begin(), fullBlock.end(), coalescedIter);
                coalescedIter += fullBlock.size();
                ++firstOffset.block;
            }

            const auto& lastBlock = m_blocks[lastOffset.block];
            const auto lastBase = lastBlock.cbegin();
            copy(lastBase, lastBase + lastBlock.size(), coalescedIter);

            string_view newString(coalesced.data(), coalesced.size());
            m_coalesceSet.insert(make_pair(key, newString));
            return newString;
        }
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
        byteOffset -= b.size();
    }
    ostringstream ss;
    ss << "SourceBuffer offset out of range: " <<  byteOffset;
    throw runtime_error(ss.str());
}
}