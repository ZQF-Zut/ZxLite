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

        auto GetProcedure(const std::size_t nHash) const -> void*
        {
            const auto image_ptr = reinterpret_cast<PBYTE>(this->ImageBase());
            const auto dos_hdr_ptr = reinterpret_cast<PIMAGE_DOS_HEADER>(image_ptr);
            const auto nt_hdr_ptr = reinterpret_cast<PIMAGE_NT_HEADERS>(image_ptr + dos_hdr_ptr->e_lfanew);

            const auto& export_entry = nt_hdr_ptr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
            if (!export_entry.VirtualAddress || !export_entry.Size) { return nullptr; }

            const auto export_dir_ptr = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(image_ptr + export_entry.VirtualAddress);
            if ((export_dir_ptr == nullptr) || (export_dir_ptr->NumberOfNames == 0) || (export_dir_ptr->NumberOfFunctions == 0)) { return nullptr; }

            const auto name_table_ptr = reinterpret_cast<PDWORD>(image_ptr + export_dir_ptr->AddressOfNames);
            const auto ordinal_table_ptr = reinterpret_cast<PWORD>(image_ptr + export_dir_ptr->AddressOfNameOrdinals);
            const auto func_table_ptr = reinterpret_cast<PDWORD>(image_ptr + export_dir_ptr->AddressOfFunctions);

            for (std::size_t idx = 0; idx < export_dir_ptr->NumberOfNames; idx++)
            {
                const auto func_name_cstr = reinterpret_cast<PCSTR>(image_ptr + name_table_ptr[idx]);
                const auto func_name_hash = ZxLite::FNV1a<std::size_t>::HashCStr(func_name_cstr);
                if (nHash != func_name_hash) { continue; }
                return image_ptr + func_table_ptr[ordinal_table_ptr[idx]];
            }

            return nullptr;
        }

        auto GetProcedure(const ZxLite::StrView msFnName) const -> void*
        {
            const auto image_ptr = reinterpret_cast<PBYTE>(this->ImageBase());
            const auto dos_hdr_ptr = reinterpret_cast<PIMAGE_DOS_HEADER>(image_ptr);
            const auto nt_hdr_ptr = reinterpret_cast<PIMAGE_NT_HEADERS>(image_ptr + dos_hdr_ptr->e_lfanew);

            const auto& export_entry = nt_hdr_ptr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
            if (!export_entry.VirtualAddress || !export_entry.Size) { return nullptr; }

            const auto export_dir_ptr = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(image_ptr + export_entry.VirtualAddress);
            if ((export_dir_ptr == nullptr) || (export_dir_ptr->NumberOfNames == 0) || (export_dir_ptr->NumberOfFunctions == 0)) { return nullptr; }

            const auto name_table_ptr = reinterpret_cast<PDWORD>(image_ptr + export_dir_ptr->AddressOfNames);
            const auto ordinal_table_ptr = reinterpret_cast<PWORD>(image_ptr + export_dir_ptr->AddressOfNameOrdinals);
            const auto func_table_ptr = reinterpret_cast<PDWORD>(image_ptr + export_dir_ptr->AddressOfFunctions);

            for (std::size_t idx = 0; idx < export_dir_ptr->NumberOfNames; idx++)
            {
                const auto func_name_cstr = reinterpret_cast<PCSTR>(image_ptr + name_table_ptr[idx]);
                const auto func_name_len = strlen(func_name_cstr);
                if (func_name_len != msFnName.SizeBytes()) { continue; }
                if (::strcmp(msFnName.Data(), func_name_cstr) == 0) { continue; }
                return image_ptr + func_table_ptr[ordinal_table_ptr[idx]];
            }

            return nullptr;
        }

        operator bool() const
        {
            return m_pLDRDataEntry != nullptr;
        }
    };
}