#include "Module.h"
#include "String.h"
#include "Hasher.h"
#include <phnt.h>


namespace ZQF::Zut::ZxLite
{
    auto OpenModule::FindLDRDataEntry(const ZxLite::WStrView wsModuleName) -> PLDR_DATA_TABLE_ENTRY
    {
        auto target_module_name_ustr{ wsModuleName.GetRtlStr() };

        const auto module_list_ptr = &NtCurrentPeb()->Ldr->InLoadOrderModuleList;
        for (auto ldr_entry_node_ptr = module_list_ptr->Flink; module_list_ptr != ldr_entry_node_ptr; ldr_entry_node_ptr = ldr_entry_node_ptr->Flink)
        {
            const auto ldr_entry_ptr = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(ldr_entry_node_ptr);
            const auto compare_status = ZxLite::StrCmp(ldr_entry_ptr->BaseDllName, target_module_name_ustr, true);
            if (compare_status != 0) 
            { 
                continue;
            }

            return ldr_entry_ptr;
        }

        return nullptr;
    }

    auto OpenModule::FindLDRDataEntry(const std::size_t nModuleNameHashUpperCase) -> PLDR_DATA_TABLE_ENTRY
    {
        const auto module_list_ptr = &NtCurrentPeb()->Ldr->InLoadOrderModuleList;
        for (auto ldr_entry_node_ptr = module_list_ptr->Flink; module_list_ptr != ldr_entry_node_ptr; ldr_entry_node_ptr = ldr_entry_node_ptr->Flink)
        {
            const auto ldr_entry_ptr = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(ldr_entry_node_ptr);
            const auto dll_name_hash = ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCase(ldr_entry_ptr->BaseDllName.Buffer, ldr_entry_ptr->BaseDllName.Length / sizeof(WCHAR));
            if (dll_name_hash != nModuleNameHashUpperCase) 
            {
                continue;
            }

            return ldr_entry_ptr;
        }

        return nullptr;
    }

    auto OpenModule::ImageBase() const -> void*
    {
        return m_pLDRDataEntry->DllBase;
    }

    auto OpenModule::ModuleName() const -> ZxLite::WStrView
    {
        return ZxLite::WStrView::FromRtlStr(m_pLDRDataEntry->BaseDllName);
    }

    auto OpenModule::ModuleFullPath() const -> ZxLite::WStrView
    {
        return ZxLite::WStrView::FromRtlStr(m_pLDRDataEntry->FullDllName);
    }

    auto OpenModule::GetProcedure(const std::size_t nHash) const -> void*
    {
        const auto image_ptr = reinterpret_cast<PBYTE>(this->ImageBase());
        const auto dos_hdr_ptr = reinterpret_cast<PIMAGE_DOS_HEADER>(image_ptr);
        const auto nt_hdr_ptr = reinterpret_cast<PIMAGE_NT_HEADERS>(image_ptr + dos_hdr_ptr->e_lfanew);

        const auto& export_entry = nt_hdr_ptr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if ((export_entry.VirtualAddress == 0) || (export_entry.Size == 0))
        { 
            return nullptr;
        }

        const auto export_dir_ptr = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(image_ptr + export_entry.VirtualAddress);
        if ((export_dir_ptr == nullptr) || (export_dir_ptr->NumberOfNames == 0) || (export_dir_ptr->NumberOfFunctions == 0)) 
        { 
            return nullptr;
        }

        const auto name_table_ptr = reinterpret_cast<PDWORD>(image_ptr + export_dir_ptr->AddressOfNames);
        const auto ordinal_table_ptr = reinterpret_cast<PWORD>(image_ptr + export_dir_ptr->AddressOfNameOrdinals);
        const auto func_table_ptr = reinterpret_cast<PDWORD>(image_ptr + export_dir_ptr->AddressOfFunctions);

        for (std::size_t idx = 0; idx < export_dir_ptr->NumberOfNames; idx++)
        {
            const auto func_name_cstr = reinterpret_cast<PCSTR>(image_ptr + name_table_ptr[idx]);
            const auto func_name_hash = ZxLite::FNV1a<std::size_t>::HashCStr(func_name_cstr);
            if (nHash != func_name_hash) 
            { 
                continue;
            }

            return image_ptr + func_table_ptr[ordinal_table_ptr[idx]];
        }

        return nullptr;
    }

    auto OpenModule::GetProcedure(const ZxLite::StrView msFnName) const -> void*
    {
        const auto image_ptr = reinterpret_cast<PBYTE>(this->ImageBase());
        const auto dos_hdr_ptr = reinterpret_cast<PIMAGE_DOS_HEADER>(image_ptr);
        const auto nt_hdr_ptr = reinterpret_cast<PIMAGE_NT_HEADERS>(image_ptr + dos_hdr_ptr->e_lfanew);

        const auto& export_entry = nt_hdr_ptr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if ((export_entry.VirtualAddress == 0) || (export_entry.Size == 0))
        {
            return nullptr;
        }

        const auto export_dir_ptr = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(image_ptr + export_entry.VirtualAddress);
        if ((export_dir_ptr == nullptr) || (export_dir_ptr->NumberOfNames == 0) || (export_dir_ptr->NumberOfFunctions == 0)) { return nullptr; }

        const auto name_table_ptr = reinterpret_cast<PDWORD>(image_ptr + export_dir_ptr->AddressOfNames);
        const auto ordinal_table_ptr = reinterpret_cast<PWORD>(image_ptr + export_dir_ptr->AddressOfNameOrdinals);
        const auto func_table_ptr = reinterpret_cast<PDWORD>(image_ptr + export_dir_ptr->AddressOfFunctions);

        for (std::size_t idx = 0; idx < export_dir_ptr->NumberOfNames; idx++)
        {
            const auto func_name_cstr = reinterpret_cast<PCSTR>(image_ptr + name_table_ptr[idx]);
            const auto func_name_len = ZxLite::StrLen(func_name_cstr);
            if (func_name_len != msFnName.SizeBytes()) 
            { 
                continue;
            }

            const auto compare_status = ZxLite::StrCmp(msFnName.Data(), msFnName.Size(), func_name_cstr, func_name_len, false);
            if (compare_status != 0) 
            {
                continue;
            }

            return image_ptr + func_table_ptr[ordinal_table_ptr[idx]];
        }

        return nullptr;
    }

    OpenModule::operator bool() const
    {
        return m_pLDRDataEntry != nullptr;
    }
}