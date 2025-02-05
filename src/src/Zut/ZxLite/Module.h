#pragma once
#include <Zut/ZxLite/StrView.h>


namespace ZQF::Zut::ZxLite
{
    class OpenModule
    {
    private:
        PLDR_DATA_TABLE_ENTRY m_pLDRDataEntry;

    public:
        OpenModule(const ZxLite::WStrView wsModuleName) : m_pLDRDataEntry(this->FindLDRDataEntry(wsModuleName)) {}

    private:
        static auto FindLDRDataEntry(const ZxLite::WStrView wsModuleName) -> PLDR_DATA_TABLE_ENTRY;

    public:
        auto ImageBase() const -> void*
        {
            return m_pLDRDataEntry->DllBase;
        }

        auto ModuleName() const -> ZxLite::WStrView
        {
            return ZxLite::WStrView::FromRtlStr(m_pLDRDataEntry->BaseDllName);
        }

        auto ModuleFullPath() const -> ZxLite::WStrView
        {
            return ZxLite::WStrView::FromRtlStr(m_pLDRDataEntry->FullDllName);
        }

        operator bool() const
        {
            return m_pLDRDataEntry != nullptr;
        }
    };
}