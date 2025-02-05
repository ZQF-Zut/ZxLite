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
		NtClose(m_ProcessInfo.hThread);
		NtClose(m_ProcessInfo.hProcess);
	}

	auto Injector::ViaAPC(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool
	{
		STARTUPINFOW si{ .cb = sizeof(STARTUPINFOW) };
		const auto create_process_status = ::CreateProcessW(wsExePath.Data(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &m_ProcessInfo);
		if (create_process_status == FALSE) { return false; }

		SIZE_T dll_path_buf_bytes = wsDllPath.SizeBytes();
		PVOID dll_path_buffer_va = nullptr;
		const auto alloc_status = ::NtAllocateVirtualMemory(m_ProcessInfo.hProcess, &dll_path_buffer_va, 0, &dll_path_buf_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (alloc_status != STATUS_SUCCESS) { return false; }

		SIZE_T written_bytes{};
		const auto status_write_dll_path = ::NtWriteVirtualMemory(m_ProcessInfo.hProcess, dll_path_buffer_va, (PVOID)wsDllPath.Data(), dll_path_buf_bytes, &written_bytes);
		if (status_write_dll_path != STATUS_SUCCESS) { return false; }

		ZxLite::OpenModule kernel32_module{ L"kernel32.dll" };
		if (kernel32_module == false) { return false; }

		const auto fn_load_library_w = kernel32_module.GetExportFn("LoadLibraryW");
		if (fn_load_library_w == NULL) { return false; }

		const auto que_apc_status = ::NtQueueApcThread(m_ProcessInfo.hThread, reinterpret_cast<PPS_APC_ROUTINE>(fn_load_library_w), dll_path_buffer_va, nullptr, nullptr);
		if (que_apc_status == 0) { return false; }

		return true;
	}

	auto Injector::ViaRemoteThread(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool
	{
		STARTUPINFOW si{ .cb = sizeof(STARTUPINFOW) };
		const auto create_process_status = ::CreateProcessW(wsExePath.Data(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &m_ProcessInfo);
		if (create_process_status == FALSE) { return false; }

		const SIZE_T dll_path_buf_bytes = wsDllPath.SizeBytes();

		const auto dll_path_buffer_va = ::VirtualAllocEx(m_ProcessInfo.hProcess, nullptr, dll_path_buf_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (dll_path_buffer_va == NULL) { return false; }

		SIZE_T written_bytes{};
		const auto status_write_dll_path = ::WriteProcessMemory(m_ProcessInfo.hProcess, dll_path_buffer_va, wsDllPath.Data(), dll_path_buf_bytes, &written_bytes);
		if (status_write_dll_path != STATUS_SUCCESS) { return false; }

		ZxLite::OpenModule kernel32_module{ L"kernel32.dll" };
		if (kernel32_module == false) { return false; }

		const auto fn_load_library_w = kernel32_module.GetExportFn("LoadLibraryW");
		if (fn_load_library_w == NULL) { return false; }

		HANDLE thread_handle{};
		OBJECT_ATTRIBUTES object_attributes{};
		const auto thread_status = ::NtCreateThreadEx(&thread_handle, PROCESS_ALL_ACCESS, &object_attributes, m_ProcessInfo.hProcess, reinterpret_cast<PUSER_THREAD_START_ROUTINE>(fn_load_library_w), dll_path_buffer_va, 0, 0, 0, 0, nullptr);
		if (thread_status != STATUS_SUCCESS) { return false; }

		return ::NtClose(thread_handle) != STATUS_SUCCESS ? false : true;
	}

	auto Injector::Resume() const -> bool
	{
		ULONG suspend_count{};
		const auto status = ::NtResumeThread(m_ProcessInfo.hThread, &suspend_count);
		return status != STATUS_SUCCESS ? false : true;
	}
}