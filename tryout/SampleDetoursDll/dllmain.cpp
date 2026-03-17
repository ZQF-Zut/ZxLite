#include <phnt_windows.h>
#include <phnt.h>
#include <Zut/ZxLite/Hasher.h>
#include <Zut/ZxLite/Module.h>
#include <detours.h>

using fn_RtlInitCodePageTable_t = decltype(&::RtlInitCodePageTable);
static fn_RtlInitCodePageTable_t sg_fnRtlInitCodePageTable; // NOLINT

static VOID NTAPI RtlInitCodePageTable_Hook(PUSHORT /*TableBase*/, PCPTABLEINFO CodePageTable)
{
  NtCurrentPeb()->ActiveCodePage = 0x3A4;
  NtCurrentPeb()->OemCodePage = 0x3A4;
  PVOID code_page_table_ptr{};
  ULONG code_page_table_size{};
  ::NtGetNlsSectionPtr(0xB, 0x3A4, nullptr, &code_page_table_ptr, &code_page_table_size);
  sg_fnRtlInitCodePageTable(reinterpret_cast<PUSHORT>(code_page_table_ptr), CodePageTable);

  ::DetourTransactionBegin();
  ::DetourUpdateThread(NtCurrentThread());
  ::DetourDetach(reinterpret_cast<PVOID*>(&sg_fnRtlInitCodePageTable), reinterpret_cast<PVOID>(::RtlInitCodePageTable_Hook));
  ::DetourTransactionCommit();
}

extern "C" auto NTAPI Start(PTHREAD_START_ROUTINE Function, PVOID Parameter) -> void
{
  sg_fnRtlInitCodePageTable = ::RtlInitCodePageTable;

  ::DetourTransactionBegin();
  ::DetourUpdateThread(NtCurrentThread());
  ::DetourAttach(reinterpret_cast<PVOID*>(&sg_fnRtlInitCodePageTable), reinterpret_cast<PVOID>(::RtlInitCodePageTable_Hook));
  ::DetourTransactionCommit();

  using fn_RtlUserThreadStart_t = decltype(&::RtlUserThreadStart);
  static fn_RtlUserThreadStart_t sg_fnRtlUserThreadStart; // NOLINT

  sg_fnRtlUserThreadStart = reinterpret_cast<fn_RtlUserThreadStart_t>(0x77D958B0);
  sg_fnRtlUserThreadStart(Function, Parameter);

  // using namespace ZQF::Zut;
  // {
  //   ZQF::Zut::ZxLite::OpenModule module_ntdll{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"ntdll.dll") };
  //   if (module_ntdll)
  //   {
  //     module_ntdll.GetProcedure<decltype(&::LdrInitializeThunk)>(ZxLite::FNV1a<std::size_t>::HashCStrCompileTime("LdrInitializeThunk"))(ContextRecord, Parameter);
  //   }
  // }
}
