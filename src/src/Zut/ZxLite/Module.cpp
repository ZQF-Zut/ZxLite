#include "Module.h"
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
            const auto compare_status = ::RtlCompareUnicodeString(&ldr_entry_ptr->BaseDllName, &target_module_name_ustr, TRUE);
            if (compare_status == 0) { return ldr_entry_ptr; }
        }

        return nullptr;
    }
}