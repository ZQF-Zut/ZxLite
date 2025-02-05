#pragma once
#include <Zut/ZxLite/StrView.h>


namespace ZQF::Zut::ZxLite
{
    auto PutConsole(void* pData, const size_t nBytes) -> void;

    template<typename T>
    static auto PutConsole(const BaseStrView<T> tStr) -> void
    {
        ZxLite::PutConsole((void*)(tStr.Data()), tStr.SizeBytes());
    }
}