#include <phnt_windows.h>
#include <phnt.h>
#include <Zut/ZxLite/Hasher.h>
#include <Zut/ZxLite/Module.h>
#include <detours.h>

extern "C" auto __stdcall Start(PCONTEXT ContextRecord, PVOID Parameter) -> void
{
  static decltype(&::NtTestAlert) sg_NtTestAlert; // NOLINT
  sg_NtTestAlert = ::NtTestAlert;

  DetourTransactionBegin();
  DetourUpdateThread(NtCurrentThread());
  DetourAttach(reinterpret_cast<PVOID*>(&sg_NtTestAlert), reinterpret_cast<PVOID>(+[]() -> NTSTATUS { return 0x1337; }));
  DetourTransactionCommit();

  using namespace ZQF::Zut;
  {
    ZQF::Zut::ZxLite::OpenModule module_ntdll{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"ntdll.dll") };
    module_ntdll.GetProcedure<decltype(&::LdrInitializeThunk)>(ZxLite::FNV1a<std::size_t>::HashCStrCompileTime("LdrInitializeThunk"))(ContextRecord, Parameter);
  }
}
