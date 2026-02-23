#include <cstddef>
#include <Windows.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define NtCurrentProcess() ((HANDLE)(LONG_PTR) - 1)

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
  NTSYSCALLAPI NTSTATUS NTAPI NtAllocateVirtualMemory(
      _In_ HANDLE ProcessHandle, _Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID* BaseAddress, _In_ ULONG_PTR ZeroBits, _Inout_ PSIZE_T RegionSize,
      _In_ ULONG AllocationType, _In_ ULONG Protect);

  NTSYSCALLAPI NTSTATUS NTAPI NtFreeVirtualMemory(_In_ HANDLE ProcessHandle, _Inout_ PVOID* BaseAddress, _Inout_ PSIZE_T RegionSize, _In_ ULONG FreeType);
  NTSYSAPI ULONG NTAPI RtlNtStatusToDosError(_In_ NTSTATUS Status);
}

extern "C"
{
#if defined(_M_AMD64) || defined(_M_ARM64) || defined(_M_ARM)
  EXCEPTION_DISPOSITION __cdecl __C_specific_handler(_In_ struct _EXCEPTION_RECORD*, _In_ void*, _Inout_ struct _CONTEXT*, _Inout_ struct _DISPATCHER_CONTEXT*)
  {
#  ifdef _DEBUG
    __debugbreak();
#  endif

    *reinterpret_cast<void**>(nullptr) = nullptr;

    return (EXCEPTION_DISPOSITION)0;
  }

#endif // defined(_M_AMD64) || defined(_M_ARM64) || defined(_M_ARM)

#if defined(_M_IX86)
  int _callnewh(size_t size)
  {
    return 0;
  }

  EXCEPTION_DISPOSITION __cdecl _except_handler3(_In_ struct _EXCEPTION_RECORD*, _In_ void*, _Inout_ struct _CONTEXT*, _Inout_ struct _DISPATCHER_CONTEXT*)
  {
#  ifdef _DEBUG
    __debugbreak();
#  endif

    *reinterpret_cast<void**>(nullptr) = nullptr;

    return (EXCEPTION_DISPOSITION)0;
  }
#endif // defined(_M_IX86)
}

// NOLINTBEGIN
extern "C"
{
  IMAGE_LOAD_CONFIG_DIRECTORY _load_config_used;

  size_t wcslen(const wchar_t* s)
  {
    size_t n;
    for (n = 0; *s != L'\0'; ++s)
    {
      ++n; //
    }
    return n;
  }

  size_t strlen(const char* s)
  {
    size_t n;
    for (n = 0; *s != '\0'; ++s)
    {
      ++n; //
    }
    return n;
  }

  void* __cdecl memset(void* dest, int ch, size_t count)
  {
    if (count)
    {
      char* d = (char*)dest;

      do
      {
        *d++ = static_cast<char>(ch); //
      } while (--count);
    }

    return dest;
  }

  void* __cdecl memcpy(void* dest, const void* src, size_t count)
  {
    char* d = (char*)dest;
    char* s = (char*)src;

    while (count--)
    {
      *d++ = *s++; //
    }

    return dest;
  }

  void* __cdecl malloc(size_t size)
  {
    NTSTATUS status;
    LPVOID mem_address{};
    SIZE_T mem_size{ size };

    __try
    {
      status = ::NtAllocateVirtualMemory(NtCurrentProcess(), &mem_address, 0, &mem_size, MEM_COMMIT, PAGE_READWRITE);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      status = GetExceptionCode();
    }

    if (!NT_SUCCESS(status))
    {
      const auto err_code = ::RtlNtStatusToDosError(status);
      if (NtCurrentTeb()->LastErrorValue != err_code)
      {
        NtCurrentTeb()->LastErrorValue = err_code;
      }
      return nullptr;
    }

    return mem_address;
  }

  void __cdecl free(void* ptr)
  {
    LPVOID mem_address{ ptr };
    SIZE_T mem_bytes{ 0 };
    const NTSTATUS status = ::NtFreeVirtualMemory(NtCurrentProcess(), &mem_address, &mem_bytes, MEM_RELEASE);
    if (!NT_SUCCESS(status))
    {
      DWORD dwErrCode = ::RtlNtStatusToDosError(status);
      if (NtCurrentTeb()->LastErrorValue != dwErrCode)
      {
        NtCurrentTeb()->LastErrorValue = dwErrCode;
      }
    }
  }
}

void* __cdecl operator new(size_t size)
{
  return ::malloc(size);
}

void* __cdecl operator new[](size_t size)
{
  return ::malloc(size);
}

void __cdecl operator delete(void* pointer) noexcept
{
  ::free(pointer);
}

void __cdecl operator delete(void* pointer, size_t) noexcept
{
  ::free(pointer);
}

void __cdecl operator delete[](void* pointer) noexcept
{
  ::free(pointer);
}

void __cdecl operator delete[](void* pointer, size_t) noexcept
{
  ::free(pointer);
}
// NOLINTEND
