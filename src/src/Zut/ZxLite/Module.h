#pragma once
#include <Zut/ZxLite/String.h>


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

        auto GetProcedure(const ZxLite::StrView wsFnName) const -> void*
        {
            auto fn_name_rtlstr{ wsFnName.GetRtlStr() };

            PVOID fn_addr;
            const auto status = ::LdrGetProcedureAddress(this->ImageBase(), &fn_name_rtlstr, 0, &fn_addr);
            return status == STATUS_SUCCESS ? fn_addr : nullptr;
        }

        operator bool() const
        {
            return m_pLDRDataEntry != nullptr;
        }
    };
}