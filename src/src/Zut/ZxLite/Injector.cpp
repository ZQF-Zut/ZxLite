#include "Injector.h"
#include <phnt.h>
#include <Zut/ZxLite/Module.h>


namespace ZQF::Zut::ZxLite
{
	Injector::Injector()
	{

	}

	Injector::~Injector()
	{
		NtClose(m_hThread);
		NtClose(m_hProcess);
	}

	auto Injector::ViaAPC(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool
	{
		ZxLite::OpenModule kernel32_module{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"kernel32.dll") };
		if (kernel32_module == false) { return false; }

		const auto fn_createprocess_w = reinterpret_cast<decltype(&::CreateProcessW)>(kernel32_module.GetProcedure(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("CreateProcessW")));
		if (fn_createprocess_w == nullptr) { return false; }

		PROCESS_INFORMATION pi{};
		STARTUPINFOW si{ .cb = sizeof(STARTUPINFOW) };
		const auto create_process_status = fn_createprocess_w(wsExePath.Data(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
		if (create_process_status == FALSE) { return false; }

		m_hThread = pi.hThread;
		m_hProcess = pi.hProcess;
		if (m_hProcess == NULL) { return false; }

		SIZE_T dll_path_buf_bytes = wsDllPath.SizeBytes();
		PVOID dll_path_buffer_va = nullptr;
		const auto alloc_status = ::NtAllocateVirtualMemory(m_hProcess, &dll_path_buffer_va, 0, &dll_path_buf_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (alloc_status != STATUS_SUCCESS) { return false; }

		SIZE_T written_bytes{};
		const auto status_write_dll_path = ::NtWriteVirtualMemory(m_hProcess, dll_path_buffer_va, (PVOID)wsDllPath.Data(), dll_path_buf_bytes, &written_bytes);
		if (status_write_dll_path != STATUS_SUCCESS) { return false; }

		const auto fn_load_library_w = kernel32_module.GetProcedure(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("LoadLibraryW"));
		if (fn_load_library_w == nullptr) { return false; }

		const auto que_apc_status = ::NtQueueApcThread(m_hThread, reinterpret_cast<PPS_APC_ROUTINE>(fn_load_library_w), dll_path_buffer_va, nullptr, nullptr);
		if (que_apc_status == 0) { return false; }

		return true;
	}

	auto Injector::ViaRemoteThread(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool
	{
		ZxLite::OpenModule kernel32_module{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"kernel32.dll") };
		if (kernel32_module == false) { return false; }

		const auto fn_createprocess_w = reinterpret_cast<decltype(&::CreateProcessW)>(kernel32_module.GetProcedure(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("CreateProcessW")));
		if (fn_createprocess_w == nullptr) { return false; }

		PROCESS_INFORMATION pi{};
		STARTUPINFOW si{ .cb = sizeof(STARTUPINFOW) };
		const auto create_process_status = fn_createprocess_w(wsExePath.Data(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
		if (create_process_status == FALSE) { return false; }

		m_hThread = pi.hThread;
		m_hProcess = pi.hProcess;
		if (m_hProcess == NULL) { return false; }

		SIZE_T dll_path_buf_bytes = wsDllPath.SizeBytes() + 2;
		PVOID dll_path_buffer_va = nullptr;
		const auto alloc_status = ::NtAllocateVirtualMemory(m_hProcess, &dll_path_buffer_va, 0, &dll_path_buf_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (alloc_status != STATUS_SUCCESS) { return false; }

		SIZE_T written_bytes{};
		const auto status_write_dll_path = ::NtWriteVirtualMemory(m_hProcess, dll_path_buffer_va, (PVOID)wsDllPath.Data(), dll_path_buf_bytes, &written_bytes);
		if (status_write_dll_path != STATUS_SUCCESS) { return false; }

		const auto fn_load_library_w = kernel32_module.GetProcedure(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("LoadLibraryW"));
		if (fn_load_library_w == nullptr) { return false; }

		HANDLE thread_handle{};
		const auto thread_status = ::NtCreateThreadEx(&thread_handle, THREAD_ALL_ACCESS, nullptr, m_hProcess, reinterpret_cast<PUSER_THREAD_START_ROUTINE>(fn_load_library_w), dll_path_buffer_va, 0, 0, 0, 0, nullptr);
		if (thread_status != STATUS_SUCCESS) { return false; }

		return ::NtClose(thread_handle) != STATUS_SUCCESS ? false : true;
	}

	auto Injector::Resume() const -> bool
	{
		ULONG suspend_count{};
		const auto status = ::NtResumeThread(m_hThread, &suspend_count);
		return status != STATUS_SUCCESS ? false : true;
	}
}