// NOLINTBEGIN
#define _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE 1
#include <windows.h>
#include "Mock.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define NtCurrentPeb() (NtCurrentTeb()->ProcessEnvironmentBlock)
#define RtlProcessHeap() (NtCurrentPeb()->ProcessHeap)
#define RTL_CONSTANT_STRING(s) { sizeof(s) - sizeof((s)[0]), sizeof(s), s }

// NOLINTBEGIN
extern "C"
{
  typedef struct _PEB_LDR_DATA PEB_LDR_DATA, *PPEB_LDR_DATA;
  typedef struct _RTL_USER_PROCESS_PARAMETERS RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

  typedef struct _ANSI_STRING
  {
    USHORT Length;
    USHORT MaximumLength;
    PSTR Buffer;
  } ANSI_STRING;
  typedef ANSI_STRING* PANSI_STRING;

  typedef struct _UNICODE_STRING
  {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
  } UNICODE_STRING;
  typedef UNICODE_STRING* PUNICODE_STRING;

  typedef struct _CLIENT_ID
  {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
  } CLIENT_ID, *PCLIENT_ID;

  typedef struct _PEB
  {
    BOOLEAN InheritedAddressSpace;
    BOOLEAN ReadImageFileExecOptions;
    BOOLEAN BeingDebugged;
    union {
      BOOLEAN BitField;
      struct
      {
        BOOLEAN ImageUsesLargePages : 1;
        BOOLEAN IsProtectedProcess : 1;
        BOOLEAN IsImageDynamicallyRelocated : 1;
        BOOLEAN SkipPatchingUser32Forwarders : 1;
        BOOLEAN IsPackagedProcess : 1;
        BOOLEAN IsAppContainer : 1;
        BOOLEAN IsProtectedProcessLight : 1;
        BOOLEAN IsLongPathAwareProcess : 1;
      };
    };

    HANDLE Mutant;

    PVOID ImageBaseAddress;
    PPEB_LDR_DATA Ldr;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    PVOID SubSystemData;
    PVOID ProcessHeap;

    //
    // ...
    //

  } PEB, *PPEB;

  typedef struct _TEB
  {
    NT_TIB NtTib;

    PVOID EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID ActiveRpcHandle;
    PVOID ThreadLocalStoragePointer;
    PPEB ProcessEnvironmentBlock;
    ULONG LastErrorValue;

    //
    // ...
    //

  } TEB, *PTEB;
}
// NOLINTEND

extern "C"
{
  NTSYSAPI PVOID NTAPI RtlAllocateHeap(_In_ PVOID HeapHandle, _In_opt_ ULONG Flags, _In_ SIZE_T Size);
  NTSYSAPI BOOLEAN NTAPI RtlFreeHeap(_In_ PVOID HeapHandle, _In_opt_ ULONG Flags, _Frees_ptr_opt_ PVOID BaseAddress);
  NTSYSAPI PVOID NTAPI RtlReAllocateHeap(_In_ PVOID HeapHandle, _In_ ULONG Flags, _Frees_ptr_opt_ PVOID BaseAddress, _In_ SIZE_T Size);
  NTSYSAPI ULONG NTAPI RtlNtStatusToDosError(_In_ NTSTATUS Status);
  NTSYSAPI NTSTATUS NTAPI LdrLoadDll(_In_opt_ PWSTR DllPath, _In_opt_ PULONG DllCharacteristics, _In_ PUNICODE_STRING DllName, _Out_ PVOID* DllHandle);
  NTSYSAPI NTSTATUS NTAPI LdrGetProcedureAddress(_In_ PVOID DllHandle, _In_opt_ PANSI_STRING ProcedureName, _In_opt_ ULONG ProcedureNumber, _Out_ PVOID* ProcedureAddress);

  ULONG WINAPI DetourGetModuleSize(_In_opt_ HMODULE hModule)
  {
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (hModule == NULL)
    {
      // pDosHeader = (PIMAGE_DOS_HEADER)GetModuleHandleW(NULL);
      pDosHeader = &__ImageBase;
    }

    __try
    {
#pragma warning(suppress : 6011) // GetModuleHandleW(NULL) never returns NULL.
      if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
      {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return 0;
      }

      PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader + pDosHeader->e_lfanew);
      if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
      {
        SetLastError(ERROR_INVALID_EXE_SIGNATURE);
        return 0;
      }
      if (pNtHeader->FileHeader.SizeOfOptionalHeader == 0)
      {
        SetLastError(ERROR_EXE_MARKED_INVALID);
        return 0;
      }
      SetLastError(NO_ERROR);

      return (pNtHeader->OptionalHeader.SizeOfImage);
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
      SetLastError(ERROR_EXE_MARKED_INVALID);
      return 0;
    }
  }

  DWORD WINAPI GetLastError(VOID)
  {
    return NtCurrentTeb()->LastErrorValue;
  }

  VOID WINAPI SetLastError(IN DWORD dwErrCode)
  {
    if (NtCurrentTeb()->LastErrorValue != dwErrCode)
      NtCurrentTeb()->LastErrorValue = dwErrCode;
  }

  DWORD WINAPI BaseSetLastNTError(IN NTSTATUS Status)
  {
    DWORD dwErrCode;

    /* Convert from NT to Win32, then set */
    dwErrCode = RtlNtStatusToDosError(Status);
    SetLastError(dwErrCode);
    return dwErrCode;
  }

  DWORD WINAPI GetCurrentThreadId(VOID)
  {
    return HandleToUlong(NtCurrentTeb()->ClientId.UniqueThread);
  }

  LPVOID NTAPI VirtualAlloc(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect)
  {
    /* Call the extended API */
    return VirtualAllocEx(GetCurrentProcess(), lpAddress, dwSize, flAllocationType, flProtect);
  }

  LPVOID NTAPI VirtualAllocEx(IN HANDLE hProcess, IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect)
  {
    NTSTATUS Status;

    /* Handle any possible exceptions */
    __try
    {
      /* Allocate the memory */
      Status = NtAllocateVirtualMemory(hProcess, &lpAddress, 0, &dwSize, flAllocationType, flProtect);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      Status = GetExceptionCode();
    }

    /* Check for status */
    if (!NT_SUCCESS(Status))
    {
      /* We failed */
      BaseSetLastNTError(Status);
      return NULL;
    }

    /* Return the allocated address */
    return lpAddress;
  }

  BOOL NTAPI VirtualFree(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType)
  {
    /* Call the extended API */
    return VirtualFreeEx(GetCurrentProcess(), lpAddress, dwSize, dwFreeType);
  }

  BOOL NTAPI VirtualFreeEx(IN HANDLE hProcess, IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType)
  {
    NTSTATUS Status;

    /* Validate size and flags */
    if (!(dwSize) || !(dwFreeType & MEM_RELEASE))
    {
      /* Free the memory */
      Status = NtFreeVirtualMemory(hProcess, &lpAddress, &dwSize, dwFreeType);
      if (!NT_SUCCESS(Status))
      {
        /* We failed */
        BaseSetLastNTError(Status);
        return FALSE;
      }

      /* Return success */
      return TRUE;
    }

    /* Invalid combo */
    BaseSetLastNTError(STATUS_INVALID_PARAMETER);
    return FALSE;
  }

  BOOL NTAPI VirtualProtect(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flNewProtect, OUT PDWORD lpflOldProtect)
  {
    /* Call the extended API */
    return VirtualProtectEx(GetCurrentProcess(), lpAddress, dwSize, flNewProtect, lpflOldProtect);
  }

  BOOL NTAPI VirtualProtectEx(IN HANDLE hProcess, IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flNewProtect, OUT PDWORD lpflOldProtect)
  {
    NTSTATUS Status;

    /* Change the protection */
    Status = NtProtectVirtualMemory(hProcess, &lpAddress, &dwSize, flNewProtect, (PULONG)lpflOldProtect);
    if (!NT_SUCCESS(Status))
    {
      /* We failed */
      BaseSetLastNTError(Status);
      return FALSE;
    }

    /* Return success */
    return TRUE;
  }

  SIZE_T NTAPI VirtualQuery(IN LPCVOID lpAddress, OUT PMEMORY_BASIC_INFORMATION lpBuffer, IN SIZE_T dwLength)
  {
    /* Call the extended API */
    return VirtualQueryEx(NtCurrentProcess(), lpAddress, lpBuffer, dwLength);
  }

  SIZE_T NTAPI VirtualQueryEx(IN HANDLE hProcess, IN LPCVOID lpAddress, OUT PMEMORY_BASIC_INFORMATION lpBuffer, IN SIZE_T dwLength)
  {
    NTSTATUS Status;
    SIZE_T ResultLength;

    /* Query basic information */
    Status = NtQueryVirtualMemory(hProcess, (LPVOID)lpAddress, MemoryBasicInformation, lpBuffer, dwLength, &ResultLength);
    if (!NT_SUCCESS(Status))
    {
      /* We failed */
      BaseSetLastNTError(Status);
      return 0;
    }

    /* Return the length returned */
    return ResultLength;
  }

  BOOL WINAPI WriteProcessMemory(_In_ HANDLE hProcess, _In_ LPVOID lpBaseAddress, _In_reads_bytes_(nSize) LPCVOID lpBuffer, _In_ SIZE_T nSize, _Out_opt_ SIZE_T* lpNumberOfBytesWritten)
  {
    NTSTATUS Status;

    /* Do the write */
    Status = NtWriteVirtualMemory(hProcess, (PVOID)lpBaseAddress, lpBuffer, nSize, &nSize);

    /* In user-mode, this parameter is optional */
    if (lpNumberOfBytesWritten)
      *lpNumberOfBytesWritten = nSize;
    if (!NT_SUCCESS(Status))
    {
      /* We failed */
      BaseSetLastNTError(Status);
      return FALSE;
    }

    /* Return success */
    return TRUE;
  }

  BOOL NTAPI ReadProcessMemory(IN HANDLE hProcess, IN LPCVOID lpBaseAddress, IN LPVOID lpBuffer, IN SIZE_T nSize, OUT SIZE_T* lpNumberOfBytesRead)
  {
    NTSTATUS Status;

    /* Do the read */
    Status = NtReadVirtualMemory(hProcess, (PVOID)lpBaseAddress, lpBuffer, nSize, &nSize);

    /* In user-mode, this parameter is optional */
    if (lpNumberOfBytesRead)
      *lpNumberOfBytesRead = nSize;
    if (!NT_SUCCESS(Status))
    {
      /* We failed */
      BaseSetLastNTError(Status);
      return FALSE;
    }

    /* Return success */
    return TRUE;
  }

  BOOL WINAPI FlushInstructionCache(IN HANDLE hProcess, IN LPCVOID lpBaseAddress, IN SIZE_T nSize)
  {
    NTSTATUS Status;

    /* Call the native function */
    Status = NtFlushInstructionCache(hProcess, (PVOID)lpBaseAddress, nSize);
    if (!NT_SUCCESS(Status))
    {
      /* Handle failure case */
      BaseSetLastNTError(Status);
      return FALSE;
    }

    /* All good */
    return TRUE;
  }

  DWORD WINAPI SuspendThread(IN HANDLE hThread)
  {
    ULONG PreviousSuspendCount;
    NTSTATUS Status;

    Status = NtSuspendThread(hThread, &PreviousSuspendCount);
    if (!NT_SUCCESS(Status))
    {
      BaseSetLastNTError(Status);
      return -1;
    }

    return PreviousSuspendCount;
  }

  DWORD WINAPI ResumeThread(IN HANDLE hThread)
  {
    ULONG PreviousResumeCount;
    NTSTATUS Status;

    Status = NtResumeThread(hThread, &PreviousResumeCount);
    if (!NT_SUCCESS(Status))
    {
      BaseSetLastNTError(Status);
      return -1;
    }

    return PreviousResumeCount;
  }

  BOOL WINAPI GetThreadContext(IN HANDLE hThread, OUT LPCONTEXT lpContext)
  {
    NTSTATUS Status;

    Status = NtGetContextThread(hThread, lpContext);
    if (!NT_SUCCESS(Status))
    {
      BaseSetLastNTError(Status);
      return FALSE;
    }

    return TRUE;
  }

  BOOL WINAPI SetThreadContext(IN HANDLE hThread, IN CONST CONTEXT* lpContext)
  {
    NTSTATUS Status;

    Status = NtSetContextThread(hThread, (PCONTEXT)lpContext);
    if (!NT_SUCCESS(Status))
    {
      BaseSetLastNTError(Status);
      return FALSE;
    }

    return TRUE;
  }
}
// NOLINTEND
