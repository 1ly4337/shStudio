#include "FlntRows.h"
#include <cstring>
#include <unordered_map>
#include <vector>

namespace {

struct Row
{
    std::vector<BYTE> original;
    std::vector<BYTE> canon;
};

std::unordered_map<const void*, Row>& rows()
{
    static std::unordered_map<const void*, Row> map;
    return map;
}

bool g_capturing = false;

}

namespace FlntRows {

void remember(const void* record, const BYTE* data, int size, const BYTE* canon, int canonSize)
{
    if (size != canonSize || size <= 0 || (size & 7) != 0) {
        rows().erase(record);
        return;
    }
    if (memcmp(data, canon, size) == 0) {
        rows().erase(record);
        return;
    }
    Row& row = rows()[record];
    row.original.assign(data, data + size);
    row.canon.assign(canon, canon + size);
}

void restore(const void* record, BYTE* data, int size)
{
    auto it = rows().find(record);
    if (it == rows().end())
        return;
    const Row& row = it->second;
    if ((int)row.canon.size() != size)
        return;
    for (int cell = 0; cell < size; cell += 8)
        if (memcmp(data + cell, &row.canon[cell], 8) == 0)
            memcpy(data + cell, &row.original[cell], 8);
}

void forget(const void* record)
{
    if (!rows().empty())
        rows().erase(record);
}

static std::unordered_map<const void*, std::vector<BYTE>>& columnBlocks()
{
    static std::unordered_map<const void*, std::vector<BYTE>> map;
    return map;
}

void rememberColumns(const void* owner, const BYTE* data, int size)
{
    if (size > 0)
        columnBlocks()[owner].assign(data, data + size);
    else
        columnBlocks().erase(owner);
}

const BYTE* columns(const void* owner, int* size)
{
    auto it = columnBlocks().find(owner);
    if (it == columnBlocks().end() || it->second.empty()) {
        *size = 0;
        return 0;
    }
    *size = (int)it->second.size();
    return it->second.data();
}

bool capturing()
{
    return g_capturing;
}

void setCapturing(bool on)
{
    g_capturing = on;
}

}
