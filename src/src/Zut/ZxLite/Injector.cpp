#include "Injector.h"
#include "Hasher.h"
#include "Module.h"

namespace ZQF::Zut::ZxLite
{
  Injector::Injector() = default;

  Injector::~Injector()
  {
    ::NtClose(m_hThread);
    ::NtClose(m_hProcess);
  }

  auto Injector::ViaAPC(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool
  {
    ZxLite::OpenModule kernel32_module{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"kernel32.dll") };
    if (!kernel32_module)
    {
      return false;
    }

    const auto fn_createprocess_w = kernel32_module.GetProcedure<decltype(&::CreateProcessW)>(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("CreateProcessW"));
    if (fn_createprocess_w == nullptr)
    {
      return false;
    }

    PROCESS_INFORMATION pi{};
    STARTUPINFOW si{ .cb = sizeof(STARTUPINFOW) }; // NOLINT
    const auto create_process_status = fn_createprocess_w(wsExePath.CStr(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
    if (create_process_status == FALSE)
    {
      return false;
    }

    m_hThread = pi.hThread;
    m_hProcess = pi.hProcess;
    if (m_hProcess == NULL)
    {
      return false;
    }

    SIZE_T dll_path_buf_bytes = wsDllPath.SizeBytes();
    PVOID dll_path_buffer_va = nullptr;
    const auto alloc_status = ::NtAllocateVirtualMemory(m_hProcess, &dll_path_buffer_va, 0, &dll_path_buf_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (alloc_status != STATUS_SUCCESS)
    {
      return false;
    }

    SIZE_T written_bytes{};
    const auto status_write_dll_path = ::NtWriteVirtualMemory(m_hProcess, dll_path_buffer_va, (PVOID)wsDllPath.CStr(), dll_path_buf_bytes, &written_bytes);
    if (status_write_dll_path != STATUS_SUCCESS)
    {
      return false;
    }

    const auto fn_load_library_w = kernel32_module.GetProcedure(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("LoadLibraryW"));
    if (fn_load_library_w == nullptr)
    {
      return false;
    }

    const auto que_apc_status = ::NtQueueApcThread(m_hThread, reinterpret_cast<PPS_APC_ROUTINE>(fn_load_library_w), dll_path_buffer_va, nullptr, nullptr);
    return que_apc_status != 0;
  }

  auto Injector::ViaRemoteThread(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool
  {
    ZxLite::OpenModule kernel32_module{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"kernel32.dll") };
    if (!kernel32_module)
    {
      return false;
    }

    const auto fn_createprocess_w = kernel32_module.GetProcedure<decltype(&::CreateProcessW)>(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("CreateProcessW"));
    if (fn_createprocess_w == nullptr)
    {
      return false;
    }

    PROCESS_INFORMATION pi{};
    STARTUPINFOW si{ .cb = sizeof(STARTUPINFOW) }; // NOLINT
    const auto create_process_status = fn_createprocess_w(wsExePath.CStr(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
    if (create_process_status == FALSE)
    {
      return false;
    }

    m_hThread = pi.hThread;
    m_hProcess = pi.hProcess;
    if (m_hProcess == NULL)
    {
      return false;
    }

    SIZE_T dll_path_buf_bytes = wsDllPath.SizeBytes() + 2;
    PVOID dll_path_buffer_va = nullptr;
    const auto alloc_status = ::NtAllocateVirtualMemory(m_hProcess, &dll_path_buffer_va, 0, &dll_path_buf_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (alloc_status != STATUS_SUCCESS)
    {
      return false;
    }

    SIZE_T written_bytes{};
    const auto status_write_dll_path = ::NtWriteVirtualMemory(m_hProcess, dll_path_buffer_va, (PVOID)wsDllPath.CStr(), dll_path_buf_bytes, &written_bytes);
    if (status_write_dll_path != STATUS_SUCCESS)
    {
      return false;
    }

    const auto fn_load_library_w = kernel32_module.GetProcedure(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("LoadLibraryW"));
    if (fn_load_library_w == nullptr)
    {
      return false;
    }

    HANDLE thread_handle{};
    const auto thread_status = ::NtCreateThreadEx(&thread_handle, THREAD_ALL_ACCESS, nullptr, m_hProcess, reinterpret_cast<PUSER_THREAD_START_ROUTINE>(fn_load_library_w), dll_path_buffer_va, 0, 0, 0, 0, nullptr);
    if (thread_status != STATUS_SUCCESS)
    {
      return false;
    }

    return ::NtClose(thread_handle) == STATUS_SUCCESS;
  }

  auto Injector::ViaMmap(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool // NOLINT
  {
    const auto fn_fix_relocation_table = [](PVOID pPEFile, PVOID pTargetAddress) -> bool
    {
      const auto dos_header_ptr = reinterpret_cast<PIMAGE_DOS_HEADER>(pPEFile);
      if (dos_header_ptr->e_magic != IMAGE_DOS_SIGNATURE)
      {
        return false;
      }

      const auto nt_headers_ptr = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<PUCHAR>(pPEFile) + dos_header_ptr->e_lfanew);
      if (nt_headers_ptr->Signature != IMAGE_NT_SIGNATURE)
      {
        return false;
      }

      const auto reloc_dir_ptr = &nt_headers_ptr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

      if (reloc_dir_ptr->VirtualAddress == 0 || reloc_dir_ptr->Size == 0)
      {
        return true;
      }

      const auto delta = (LONG_PTR)((ULONG_PTR)pTargetAddress - (ULONG_PTR)nt_headers_ptr->OptionalHeader.ImageBase);
      if (delta == 0)
      {
        return true;
      }

      auto reloc_block_cur = reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<PUCHAR>(pPEFile) + reloc_dir_ptr->VirtualAddress);
      while (reloc_block_cur->VirtualAddress != 0 && reloc_block_cur->SizeOfBlock != 0)
      {
        const auto entry_count = (reloc_block_cur->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
        const auto type_offsets_ptr = (PWORD)((PUCHAR)reloc_block_cur + sizeof(IMAGE_BASE_RELOCATION));

        for (DWORD i = 0; i < entry_count; i++)
        {
          WORD type = type_offsets_ptr[i] >> 12;
          WORD offset = type_offsets_ptr[i] & 0x0FFF;

          if (type == IMAGE_REL_BASED_ABSOLUTE)
          {
            continue;
          }

          const auto patch_address = reinterpret_cast<PUCHAR>(pPEFile) + reloc_block_cur->VirtualAddress + offset;

          if (type == IMAGE_REL_BASED_DIR64)
          {
            *(PULONGLONG)patch_address += (ULONGLONG)delta;
          }
          else if (type == IMAGE_REL_BASED_HIGHLOW)
          {
            *(PDWORD)patch_address += (DWORD)delta;
          }
        }

        reloc_block_cur = (PIMAGE_BASE_RELOCATION)((PUCHAR)reloc_block_cur + reloc_block_cur->SizeOfBlock);
      }

      return true;
    };

    const auto fn_fix_import_table = [](PVOID pPEFile) -> bool
    {
      const auto dos_header_ptr = reinterpret_cast<PIMAGE_DOS_HEADER>(pPEFile);
      if (dos_header_ptr->e_magic != IMAGE_DOS_SIGNATURE)
      {
        return false;
      }

      const auto nt_headers_ptr = reinterpret_cast<PIMAGE_NT_HEADERS>((PUCHAR)pPEFile + dos_header_ptr->e_lfanew);
      if (nt_headers_ptr->Signature != IMAGE_NT_SIGNATURE)
      {
        return false;
      }

      const auto import_dir_ptr = &nt_headers_ptr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

      if (import_dir_ptr->VirtualAddress == 0 || import_dir_ptr->Size == 0)
      {
        return true;
      }

      constexpr auto ntdll_hash = ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime("ntdll.dll");
      PIMAGE_IMPORT_DESCRIPTOR ntdll_desc_ptr{};
      {
        auto import_desc_ite = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>((PUCHAR)pPEFile + import_dir_ptr->VirtualAddress);
        while (import_desc_ite->Name != 0)
        {
          const auto dllName = reinterpret_cast<LPCSTR>((PUCHAR)pPEFile + import_desc_ite->Name);

          if (ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCase(dllName) == ntdll_hash)
          {
            ntdll_desc_ptr = import_desc_ite;
            break;
          }

          import_desc_ite++;
        }

        if (ntdll_desc_ptr == nullptr)
        {
          return false;
        }
      }

      ZxLite::OpenModule ntdll_module{ ntdll_hash };

      auto thunk_iat_ptr = reinterpret_cast<PIMAGE_THUNK_DATA>((PUCHAR)pPEFile + ntdll_desc_ptr->FirstThunk);
      auto thunk_int_ptr = reinterpret_cast<PIMAGE_THUNK_DATA>((PUCHAR)pPEFile + (ntdll_desc_ptr->OriginalFirstThunk != 0 ? ntdll_desc_ptr->OriginalFirstThunk : ntdll_desc_ptr->FirstThunk));
      while (thunk_int_ptr->u1.AddressOfData != 0)
      {
        void* function_address{ nullptr };

        if (IMAGE_SNAP_BY_ORDINAL(thunk_int_ptr->u1.Ordinal))
        {
          // LPCSTR ordinal = (LPCSTR)IMAGE_ORDINAL(pThunkINT->u1.Ordinal);
          // functionAddress = ::GetProcAddress(hDll, ordinal);
          return false; // unimp
        }
        else
        {
          const auto import_by_name = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>((PUCHAR)pPEFile + thunk_int_ptr->u1.AddressOfData);
          const auto function_name = reinterpret_cast<LPCSTR>(import_by_name->Name);
          function_address = ntdll_module.GetProcedure(function_name);
        }

        if (!function_address)
        {
          return false;
        }

        thunk_iat_ptr->u1.Function = (ULONG_PTR)function_address;

        thunk_int_ptr++;
        thunk_iat_ptr++;
      }

      return true;
    };

    // create process
    {
      ZxLite::OpenModule kernel32_module{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"kernel32.dll") };
      if (!kernel32_module)
      {
        return false;
      }

      const auto fn_createprocess_w = kernel32_module.GetProcedure<decltype(&::CreateProcessW)>(ZxLite::FNV1a<std::size_t>::HashCStrArrayCompileTime("CreateProcessW"));
      if (fn_createprocess_w == nullptr)
      {
        return false;
      }

      PROCESS_INFORMATION pi{};
      STARTUPINFOW si{ .cb = sizeof(STARTUPINFOW) }; // NOLINT
      const auto create_process_status = fn_createprocess_w(wsExePath.CStr(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
      if (create_process_status == FALSE)
      {
        return false;
      }

      m_hThread = pi.hThread;
      m_hProcess = pi.hProcess;
      if (m_hProcess == NULL)
      {
        return false;
      }
    }

    using HashType = ZxLite::FNV1a<std::size_t>;

    ZxLite::OpenModule ntdll_module{ HashType::HashCStrIgnoreCaseCompileTime(L"ntdll.dll") };
    if (!ntdll_module)
    {
      return false;
    }

    const auto fn_NtOpenFile = ntdll_module.GetProcedure<decltype(&::NtOpenFile)>(HashType::HashCStrCompileTime("NtOpenFile")); // NOLINT
    if (fn_NtOpenFile == nullptr)
    {
      return false;
    }

    const auto fn_NtCreateSection = ntdll_module.GetProcedure<decltype(&::NtCreateSection)>(HashType::HashCStrCompileTime("NtCreateSection")); // NOLINT
    if (fn_NtCreateSection == nullptr)
    {
      return false;
    }

    const auto fn_NtMapViewOfSection = ntdll_module.GetProcedure<decltype(&::NtMapViewOfSection)>(HashType::HashCStrCompileTime("NtMapViewOfSection")); // NOLINT
    if (fn_NtMapViewOfSection == nullptr)
    {
      return false;
    }

    // open dll file
    HANDLE file_handle{};
    {
      const auto path = wsDllPath.GetRtlStr();
      OBJECT_ATTRIBUTES obj_attr;
      InitializeObjectAttributes(&obj_attr, &path, OBJ_CASE_INSENSITIVE, NULL, NULL);
      IO_STATUS_BLOCK io_status_block;
      const auto status = fn_NtOpenFile(&file_handle, FILE_GENERIC_READ | SYNCHRONIZE, &obj_attr, &io_status_block, FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT); // NOLINT
      if (status != STATUS_SUCCESS)
      {
        return false;
      }
    }

    // load dll file
    SIZE_T dll_bytes{};
    PVOID dll_addr{};
    {
      LARGE_INTEGER offset{};
      HANDLE section_handle{};
      const auto status_create = fn_NtCreateSection(&section_handle, STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_READ, nullptr, 0, PAGE_READONLY, SEC_IMAGE_NO_EXECUTE, file_handle);
      if (status_create != STATUS_SUCCESS)
      {
        return false;
      }

      const auto status_map = fn_NtMapViewOfSection(section_handle, NtCurrentProcess(), &dll_addr, 0, 0, &offset, &dll_bytes, ViewShare, NULL, PAGE_READONLY);
      if (status_map != STATUS_SUCCESS)
      {
        return false;
      }
    }

    // map to remote process
    {
      HANDLE section_handle{};
      LARGE_INTEGER section_size{};
      section_size.QuadPart = static_cast<LONGLONG>(dll_bytes);
      const auto status_create = fn_NtCreateSection(&section_handle, SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE, nullptr, &section_size, PAGE_EXECUTE_READWRITE, SEC_COMMIT, nullptr);
      if (status_create != STATUS_SUCCESS)
      {
        return false;
      }

      PVOID local_section_address{};
      const auto status_map_current_process = fn_NtMapViewOfSection(section_handle, NtCurrentProcess(), &local_section_address, 0, 0, nullptr, &dll_bytes, ViewShare, NULL, PAGE_READWRITE);
      if (status_map_current_process != STATUS_SUCCESS)
      {
        return false;
      }

      PVOID remote_section_address{};
      const auto status_map_remote_process = fn_NtMapViewOfSection(section_handle, m_hProcess, &remote_section_address, 0, 0, nullptr, &dll_bytes, ViewShare, NULL, PAGE_EXECUTE_READWRITE);
      if (status_map_remote_process != STATUS_SUCCESS)
      {
        return false;
      }

      ZxLite::MemCpy(local_section_address, dll_addr, dll_bytes);

      const auto status_relocation = fn_fix_relocation_table(local_section_address, remote_section_address);
      if (!status_relocation)
      {
        return false;
      }

      const auto status_fix_import_table = fn_fix_import_table(local_section_address);
      if (!status_fix_import_table)
      {
        return false;
      }
    }

    return true;
  }

  auto Injector::Resume() const -> bool
  {
    ULONG suspend_count{};
    const auto status = ::NtResumeThread(m_hThread, &suspend_count);
    return status == STATUS_SUCCESS;
  }
} // namespace ZQF::Zut::ZxLite
