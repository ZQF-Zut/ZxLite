#pragma once
// https://github.com/wbenny/DetoursNT

#define _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE 1
#include <windows.h>

#define NtCurrentProcess() ((HANDLE)(LONG_PTR) - 1)
#define ZwCurrentProcess() NtCurrentProcess()
#define NtCurrentThread() ((HANDLE)(LONG_PTR) - 2)
#define ZwCurrentThread() NtCurrentThread()

#define GetLastError Mock_GetLastError
#define SetLastError Mock_SetLastError

#define GetCurrentProcess() NtCurrentProcess() // NOLINT
#define GetCurrentThread() NtCurrentThread()   // NOLINT
#define GetCurrentThreadId Mock_GetCurrentThreadId

#define VirtualAlloc Mock_VirtualAlloc
#define VirtualAllocEx Mock_VirtualAllocEx
#define VirtualFree Mock_VirtualFree
#define VirtualFreeEx Mock_VirtualFreeEx
#define VirtualProtect Mock_VirtualProtect
#define VirtualProtectEx Mock_VirtualProtectEx
#define VirtualQuery Mock_VirtualQuery
#define VirtualQueryEx Mock_VirtualQueryEx
#define ReadProcessMemory Mock_ReadProcessMemory
#define WriteProcessMemory Mock_WriteProcessMemory
#define FlushInstructionCache Mock_FlushInstructionCache
#define SuspendThread Mock_SuspendThread
#define ResumeThread Mock_ResumeThread
#define GetThreadContext Mock_GetThreadContext
#define SetThreadContext Mock_SetThreadContext
#define DebugBreak __debugbreak

// NOLINTBEGIN
#ifdef __cplusplus
extern "C"
{
#endif
  extern IMAGE_DOS_HEADER __ImageBase;

  ULONG WINAPI DetourGetModuleSize(_In_opt_ HMODULE hModule);

  typedef enum _MEMORY_INFORMATION_CLASS
  {
    MemoryBasicInformation,
    MemoryWorkingSetList,
    MemorySectionName,
    MemoryBasicVlmInformation,
    MemoryWorkingSetExList
  } MEMORY_INFORMATION_CLASS;

  NTSYSCALLAPI NTSTATUS NTAPI NtAllocateVirtualMemory(
      _In_ HANDLE ProcessHandle, _Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID* BaseAddress, _In_ ULONG_PTR ZeroBits, _Inout_ PSIZE_T RegionSize,
      _In_ ULONG AllocationType, _In_ ULONG Protect);

  NTSYSCALLAPI NTSTATUS NTAPI NtFreeVirtualMemory(_In_ HANDLE ProcessHandle, _Inout_ PVOID* BaseAddress, _Inout_ PSIZE_T RegionSize, _In_ ULONG FreeType);
  NTSYSCALLAPI NTSTATUS NTAPI NtProtectVirtualMemory(_In_ HANDLE ProcessHandle, _Inout_ PVOID* BaseAddress, _Inout_ PSIZE_T RegionSize, _In_ ULONG NewProtect, _Out_ PULONG OldProtect);
  NTSYSCALLAPI NTSTATUS NTAPI NtQueryVirtualMemory(
      _In_ HANDLE ProcessHandle, _In_opt_ PVOID BaseAddress, _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass, _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation, _In_ SIZE_T MemoryInformationLength, _Out_opt_ PSIZE_T ReturnLength);
  NTSYSCALLAPI NTSTATUS NTAPI NtReadVirtualMemory(_In_ HANDLE ProcessHandle, _In_opt_ PVOID BaseAddress, _Out_writes_bytes_(BufferSize) PVOID Buffer, _In_ SIZE_T BufferSize, _Out_opt_ PSIZE_T NumberOfBytesRead);
  NTSYSCALLAPI NTSTATUS NTAPI NtWriteVirtualMemory(_In_ HANDLE ProcessHandle, _In_opt_ PVOID BaseAddress, _In_reads_bytes_(BufferSize) LPCVOID Buffer, _In_ SIZE_T BufferSize, _Out_opt_ PSIZE_T NumberOfBytesWritten);
  NTSYSCALLAPI NTSTATUS NTAPI NtFlushInstructionCache(_In_ HANDLE ProcessHandle, _In_opt_ PVOID BaseAddress, _In_ SIZE_T Length);
  NTSYSCALLAPI NTSTATUS NTAPI NtSuspendThread(_In_ HANDLE ThreadHandle, _Out_opt_ PULONG PreviousSuspendCount);
  NTSYSCALLAPI NTSTATUS NTAPI NtResumeThread(_In_ HANDLE ThreadHandle, _Out_opt_ PULONG PreviousSuspendCount);
  NTSYSCALLAPI NTSTATUS NTAPI NtGetContextThread(_In_ HANDLE ThreadHandle, _Inout_ PCONTEXT ThreadContext);
  NTSYSCALLAPI NTSTATUS NTAPI NtSetContextThread(_In_ HANDLE ThreadHandle, _In_ PCONTEXT ThreadContext);

  DWORD WINAPI GetLastError(VOID);
  VOID WINAPI SetLastError(IN DWORD dwErrCode);
  DWORD WINAPI GetCurrentThreadId(VOID);
  LPVOID NTAPI VirtualAlloc(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect);
  LPVOID NTAPI VirtualAllocEx(IN HANDLE hProcess, IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect);
  BOOL NTAPI VirtualFree(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType);
  BOOL NTAPI VirtualFreeEx(IN HANDLE hProcess, IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType);
  BOOL NTAPI VirtualProtect(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flNewProtect, OUT PDWORD lpflOldProtect);
  BOOL NTAPI VirtualProtectEx(IN HANDLE hProcess, IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flNewProtect, OUT PDWORD lpflOldProtect);
  SIZE_T NTAPI VirtualQuery(IN LPCVOID lpAddress, OUT PMEMORY_BASIC_INFORMATION lpBuffer, IN SIZE_T dwLength);
  SIZE_T NTAPI VirtualQueryEx(IN HANDLE hProcess, IN LPCVOID lpAddress, OUT PMEMORY_BASIC_INFORMATION lpBuffer, IN SIZE_T dwLength);
  BOOL NTAPI ReadProcessMemory(IN HANDLE hProcess, IN LPCVOID lpBaseAddress, IN LPVOID lpBuffer, IN SIZE_T nSize, OUT SIZE_T* lpNumberOfBytesRead);
  BOOL WINAPI WriteProcessMemory(_In_ HANDLE hProcess, _In_ LPVOID lpBaseAddress, _In_reads_bytes_(nSize) LPCVOID lpBuffer, _In_ SIZE_T nSize, _Out_opt_ SIZE_T* lpNumberOfBytesWritten);
  BOOL WINAPI FlushInstructionCache(IN HANDLE hProcess, IN LPCVOID lpBaseAddress, IN SIZE_T nSize);
  DWORD WINAPI SuspendThread(IN HANDLE hThread);
  DWORD WINAPI ResumeThread(IN HANDLE hThread);
  BOOL WINAPI GetThreadContext(IN HANDLE hThread, OUT LPCONTEXT lpContext);
  BOOL WINAPI SetThreadContext(IN HANDLE hThread, IN CONST CONTEXT* lpContext);

#ifdef __cplusplus
}
#endif
// NOLINTEND

#undef _DEBUG
