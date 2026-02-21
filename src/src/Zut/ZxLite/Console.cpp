#include "Console.h"
#include "Hasher.h"
#include "Module.h"
#include <phnt.h>

namespace ZQF::Zut::ZxLite
{
  Console::Console()
  {
    ZxLite::OpenModule ntdll_module{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"ntdll.dll") };
    if (!ntdll_module)
    {
      return;
    }

    m_fnNtWriteFile = ntdll_module.GetProcedure(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("NtWriteFile"));
    if (m_fnNtWriteFile == nullptr)
    {
      return;
    }
  }

  auto Console::Put(const void* pData, const std::size_t nBytes) -> void
  {
    IO_STATUS_BLOCK io_status_block;
    reinterpret_cast<decltype(&::NtWriteFile)>(m_fnNtWriteFile)(NtCurrentPeb()->ProcessParameters->StandardOutput, nullptr, nullptr, nullptr, &io_status_block, const_cast<PVOID>(pData), static_cast<ULONG>(nBytes), nullptr, nullptr); // NOLINT
  }
} // namespace ZQF::Zut::ZxLite
