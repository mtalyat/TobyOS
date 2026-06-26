#include "types/types.hpp"

extern "C" void* memcpy(void* dest, const void* src, uint n)
{
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);

    for (uint i = 0; i < n; ++i)
    {
        d[i] = s[i];
    }

    return dest;
}