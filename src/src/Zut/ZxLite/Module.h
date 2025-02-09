#pragma once
#include <Zut/ZxLite/Hasher.h>
#include <Zut/ZxLite/String.h>


namespace ZQF::Zut::ZxLite
{
    class OpenModule
    {
    private:
        PLDR_DATA_TABLE_ENTRY m_pLDRDataEntry;

    public:
        OpenModule(const ZxLite::WStrView wsModuleName) : m_pLDRDataEntry(this->FindLDRDataEntry(wsModuleName)) {}
        OpenModule(const std::size_t nModuleNameHashUpperCase) : m_pLDRDataEntry(this->FindLDRDataEntry(nModuleNameHashUpperCase)) {}

    private:
        static auto FindLDRDataEntry(const ZxLite::WStrView wsModuleName) -> PLDR_DATA_TABLE_ENTRY;
        static auto FindLDRDataEntry(const std::size_t nModuleNameHashUpperCase) -> PLDR_DATA_TABLE_ENTRY;

    public:
        auto ImageBase() const -> void*;
        auto ModuleName() const->ZxLite::WStrView;
        auto ModuleFullPath() const->ZxLite::WStrView;
        auto GetProcedure(const std::size_t nHash) const -> void*;
        auto GetProcedure(const ZxLite::StrView msFnName) const -> void*;

    public:
        operator bool() const;
    };
}