#include "Console.h"
#include <phnt.h>


namespace ZQF::Zut::ZxLite
{
	auto PutConsole(void* pData, const std::size_t nBytes) -> void
	{
		IO_STATUS_BLOCK io_status_block;
		::NtWriteFile(NtCurrentPeb()->ProcessParameters->StandardOutput, nullptr, nullptr, nullptr, &io_status_block, pData, static_cast<ULONG>(nBytes), nullptr, nullptr);
	}
}