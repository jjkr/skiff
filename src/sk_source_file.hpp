#pragma once
#include "util/string_view.hpp"
#include <thread>

namespace sk
{
template <int BlockSize>
class SourceFile
{
public:
    SourceFile(string_view path) { }

private:
    std::thread m_readThread;
    std::vector<uint8_t[BlockSize]> m_blockList;
};
}