#include <Windows.h>


namespace RxLite
{
	VOID SysErrorMsgBox(LPCWSTR wpMsg, BOOL isExit)
	{
		::MessageBoxW(NULL, wpMsg, NULL, NULL);
		isExit ? (::ExitProcess(-1)) : ((VOID)NULL);
	}

	VOID SysErrorMsgBox(LPCSTR cpMsg, BOOL isExit)
	{
		::MessageBoxA(NULL, cpMsg, NULL, NULL);
		isExit ? (::ExitProcess(-1)) : ((VOID)NULL);
	}
}

namespace RxLite
{
	PVOID MemoryXCopy(PVOID pDst, PVOID pSrc, SIZE_T nSize)
	{
		return ::memcpy(pDst, pSrc, nSize);
	}

	INT MemoryXCmp(const PVOID pDst, const PVOID pSrc, SIZE_T nSize)
	{
		return ::memcmp(pDst, pSrc, nSize);
	}

	PVOID MemoryXFill(PVOID pDst, BYTE bValue, SIZE_T nSize)
	{
		PBYTE dst_ptr = (PBYTE)pDst;
		while (nSize--)
		{
			__asm nop;
			*dst_ptr = bValue;
			dst_ptr++;
		}
		return dst_ptr;
	}

	SIZE_T StrCpy(LPSTR pBuffer, LPCSTR cpStr2)
	{
		PBYTE str_ptr = (PBYTE)cpStr2;
		while (*str_ptr)
		{
			*pBuffer++ = *str_ptr++;
		}
		*pBuffer = 0;
		return str_ptr - (PBYTE)cpStr2;
	}

	class StrView
	{
	public:
		const char* m_cpStr;
		SIZE_T m_nLen;

		constexpr StrView() noexcept : m_cpStr(NULL), m_nLen(NULL) {};
		template<SIZE_T N> constexpr StrView(const char(&aStr)[N]) noexcept : m_cpStr(aStr), m_nLen(N) {};
		constexpr const char* Data() const noexcept { return m_cpStr; }
		constexpr SIZE_T Size() const noexcept { return m_nLen; }
	};

	class WStrView
	{
	public:
		const wchar_t* m_wpStr;
		SIZE_T m_nLen;

		constexpr WStrView() noexcept : m_wpStr(NULL), m_nLen(NULL) {};
		template<SIZE_T N> constexpr WStrView(const wchar_t(&aStr)[N]) noexcept : m_wpStr(aStr), m_nLen(N) {};
		constexpr const wchar_t* Data() const noexcept { return m_wpStr; }
		constexpr SIZE_T Size() const noexcept { return m_nLen; }
	};
}

namespace RxLite
{
	LPVOID SysMemAlloc(SIZE_T nSize, DWORD uiAccess)
	{
		return ::VirtualAlloc(NULL, nSize, MEM_COMMIT, uiAccess);
	}

	BOOL SysMemFree(LPVOID lpAddress)
	{
		return ::VirtualFree(lpAddress, 0, MEM_RELEASE);
	}

	PVOID SysMemAlloc(SIZE_T nSize, DWORD uiAccess, LPCWSTR wpErrorMsg, BOOL isExit)
	{
		PVOID buffer_ptr = SysMemAlloc(nSize, uiAccess);
		if (buffer_ptr == NULL) { SysErrorMsgBox(wpErrorMsg, isExit); }
		return buffer_ptr;
	}

	BOOL SysMemAccess(PVOID pAddress, SIZE_T nSize, DWORD uiAccess, PDWORD pOldAccess)
	{
		DWORD old = 0;
		return ::VirtualProtect(pAddress, nSize, uiAccess, &old) ? (BOOL)(pOldAccess ? (*pOldAccess = old) : (TRUE)) : FALSE;
	}

	VOID SysMemAccess(PVOID pAddress, SIZE_T nSize, DWORD uiAccess, PDWORD pOldAccess, LPCWSTR wpErrorMsg, BOOL isExit)
	{
		SysMemAccess(pAddress, nSize, uiAccess, pOldAccess) ? (VOID)NULL : SysErrorMsgBox(wpErrorMsg, isExit);
	}

	BOOL SysMemFill(PVOID pDst, BYTE ucValue, SIZE_T nSize)
	{
		return SysMemAccess(pDst, nSize, PAGE_EXECUTE_READWRITE, NULL) ? (BOOL)MemoryXFill(pDst, ucValue, nSize) : (BOOL)FALSE;
	}

	VOID SysMemFill(PVOID pAddress, BYTE ucValue, SIZE_T nSize, LPWSTR wpErrorMsg, BOOL isExit)
	{
		SysMemFill(pAddress, ucValue, nSize) ? (VOID)NULL : SysErrorMsgBox(wpErrorMsg, isExit);
	}

	BOOL SysMemCopy(PVOID pDst, PVOID pSrc, SIZE_T nSize)
	{
		return SysMemAccess(pSrc, nSize, PAGE_EXECUTE_READWRITE, NULL) ? (BOOL)MemoryXCopy(pDst, pSrc, nSize) : (BOOL)FALSE;
	}

	VOID SysMemCopy(PVOID pDst, PVOID pSrc, SIZE_T nSize, LPWSTR wpErrorMsg, BOOL isExit)
	{
		SysMemCopy(pDst, pSrc, nSize) ? (VOID)NULL : SysErrorMsgBox(wpErrorMsg, isExit);
	}


	PVOID SysMemMemSearch(const PVOID pStartAddress, SIZE_T nMaxSearchSize, const PVOID pMatchData, const SIZE_T nMatchDataBytes, const BOOL isBackward)
	{
		PBYTE start_ptr = (PBYTE)pStartAddress;
		if (start_ptr && nMatchDataBytes)
		{
			if (isBackward)
			{
				for (SIZE_T ite = 0; ite < nMaxSearchSize; ite++)
				{
					if (!MemoryXCmp(pMatchData, start_ptr--, nMatchDataBytes))
					{
						return (start_ptr + 1);
					}
				}
			}
			else
			{
				for (SIZE_T ite = 0; ite < nMaxSearchSize; ite++)
				{
					if (!MemoryXCmp(pMatchData, start_ptr++, nMatchDataBytes))
					{
						return (start_ptr - 1);
					}
				}
			}
		}

		return nullptr;
	}

}

namespace RxLite
{
	class Cmd
	{
	private:
		HANDLE m_hInput;
		HANDLE m_hOutput;
		LPVOID m_pBuffer;

	public:
		Cmd(SIZE_T nBufferSize = 2048)
		{
			this->Init(nBufferSize);
		};

		Cmd(LPCWSTR wpTitle, SIZE_T nBufferSize = 2048)
		{
			this->Init(nBufferSize);
			this->SetTitle(wpTitle);
		};

		~Cmd()
		{
			::FreeConsole();
			SysMemFree(m_pBuffer);
			m_hInput = NULL;
			m_hOutput = NULL;
			m_pBuffer = NULL;
		};

		VOID Init(SIZE_T nBufferSize)
		{
			::AllocConsole();
			::AttachConsole(ATTACH_PARENT_PROCESS);
			m_hInput = ::GetStdHandle(STD_INPUT_HANDLE);
			m_hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
			m_pBuffer = SysMemAlloc(nBufferSize, PAGE_READWRITE);
		}

		BOOL SetTitle(LPCWSTR wpText)
		{
			return ::SetConsoleTitleW(wpText);
		}

		BOOL Write(LPCSTR cpText, DWORD nCharsToWrite)
		{
			DWORD written = 0;
			return ::WriteConsoleA(m_hOutput, cpText, nCharsToWrite, &written, NULL);
		}

		BOOL Write(LPCWSTR wpText, DWORD nCharsToWrite)
		{
			DWORD written = 0;
			return ::WriteConsoleW(m_hOutput, wpText, nCharsToWrite, &written, NULL);
		}

		BOOL Read(LPSTR cpBuffer, DWORD nCharsToRead)
		{
			DWORD read = 0;
			return ::ReadConsoleA(m_hInput, cpBuffer, nCharsToRead, &read, NULL);
		}

		BOOL Read(LPWSTR wpBuffer, DWORD nCharsToRead)
		{
			DWORD read = 0;
			return ::ReadConsoleW(m_hInput, wpBuffer, nCharsToRead, &read, NULL);
		}

		BOOL Write(StrView msStr)
		{
			return Write(msStr.Data(), msStr.Size());
		}

		BOOL Write(WStrView wsStr)
		{
			return Write(wsStr.Data(), wsStr.Size());
		}

		VOID ClearInput()
		{
			DWORD read = 0;
			DWORD buffer = 0;

			while (true)
			{
				::ReadConsoleW(m_hInput, &buffer, 1, &read, NULL);
				if (buffer != L'\r') { continue; }
				::ReadConsoleW(m_hInput, &buffer, 1, &read, NULL);
				if (buffer != L'\n') { continue; }
				return;
			}
		}

		VOID Wait()
		{
			wchar_t buffer[1];
			this->Read(buffer, 1);
		}

		BOOL PutFormat(LPCSTR cpFormat, ...)
		{
			char* buffer = (char*)m_pBuffer;

			va_list args = nullptr;
			va_start(args, cpFormat);
			size_t cch = ::wvsprintfA(buffer, cpFormat, args);
			va_end(args);

			return (cch <= 0) ? (false) : (Write(buffer, cch));
		}

		BOOL PutFormat(LPCWSTR wpFormat, ...)
		{
			wchar_t* buffer = (wchar_t*)m_pBuffer;

			va_list args = nullptr;
			va_start(args, wpFormat);
			size_t cch = ::wvsprintfW(buffer, wpFormat, args);
			va_end(args);

			return (cch <= 0) ? (false) : (Write(buffer, cch));
		}

	};
}

namespace RxLite
{
	class Auto
	{
	public:
		PVOID m_pMem;
		SIZE_T m_nMemSize;
		SIZE_T m_nUserSize;

		Auto();
		~Auto();

		Auto& Move(Auto&& buffer) noexcept;

		void SetSize(SIZE_T nNewSize, BOOL isCopy = false);
		template<class T = PBYTE> T GetPtr() const noexcept;
		template<class T = SIZE_T> constexpr T GetSize() const noexcept;

	};

	Auto::Auto()
	{
		m_pMem = NULL;
		m_nMemSize = NULL;
		m_nUserSize = NULL;
	}

	Auto::~Auto()
	{
		if (m_pMem != NULL)
		{
			SysMemFree(m_pMem);
			m_pMem = NULL;
			m_nMemSize = NULL;
			m_nUserSize = NULL;
		}
	}

	Auto& Auto::Move(Auto&& buffer) noexcept
	{
		this->m_pMem = buffer.m_pMem;
		this->m_nMemSize = buffer.m_nMemSize;
		this->m_nUserSize = buffer.m_nUserSize;

		buffer.m_pMem = NULL;
		buffer.m_nMemSize = NULL;
		buffer.m_nUserSize = NULL;

		return *this;
	}

	void Auto::SetSize(SIZE_T nNewSize, BOOL isCopy)
	{
		if (m_nMemSize == 0)
		{
			m_pMem = SysMemAlloc(nNewSize, PAGE_READWRITE);
			m_nMemSize = nNewSize;
		}
		else if (nNewSize > m_nMemSize)
		{
			if (m_pMem != NULL)
			{
				SysMemFree(m_pMem);
			}
			m_pMem = SysMemAlloc(nNewSize, PAGE_READWRITE);
			m_nMemSize = nNewSize;
		}

		m_nUserSize = nNewSize;
	}

	template<class T> T Auto::GetPtr() const noexcept
	{
		return reinterpret_cast<T>(m_pMem);
	}

	template<class T> constexpr T Auto::GetSize() const noexcept
	{
		return reinterpret_cast<T>(m_nMemSize);
	}
}

namespace RxLite
{
	VOID WriteJmp(PVOID pFunc, PVOID pDest, SIZE_T nCoverSize, BYTE ucAsmCode)
	{
		SysMemAccess(pFunc, nCoverSize, PAGE_EXECUTE_READWRITE, NULL, L"RxLite: Access Memory Error!", TRUE);
		*(PUINT8)((PUINT8)pFunc + 0) = ucAsmCode;
		*(PDWORD)((PUINT8)pFunc + 1) = (DWORD)pDest - (DWORD)pFunc - 5;
		(nCoverSize > 0x5) ? (VOID)MemoryXFill((PBYTE)pFunc + 0x5, 0x90, nCoverSize - 0x5) : (VOID)NULL;
	}

	PVOID AllocTrampolineFunc(PVOID pFunc, SIZE_T nSize)
	{
		DWORD copy_src_func_asm_size = nSize;
		DWORD trampoline_func_size = copy_src_func_asm_size + 5;
		PVOID tpl_func_buffer = SysMemAlloc(trampoline_func_size, PAGE_EXECUTE_READWRITE);
		if (tpl_func_buffer)
		{
			if (MemoryXCopy(tpl_func_buffer, pFunc, copy_src_func_asm_size))
			{
				WriteJmp((PBYTE)tpl_func_buffer + copy_src_func_asm_size, (PBYTE)pFunc + copy_src_func_asm_size, 5, 0xE9);
				return tpl_func_buffer;
			}
		}
		return NULL;
	}

	BOOL FreeTrampolineFunc(PVOID ppFunc)
	{
		PVOID* fn_tpl_pp = (PVOID*)ppFunc;
		PVOID fn_tpl = *fn_tpl_pp;
		return SysMemFree(fn_tpl);
	}

	VOID TrampolineHook(PVOID ppFunc, SIZE_T nSize, PVOID pDetour)
	{
		PVOID* fn_org_pp = (PVOID*)ppFunc;
		PVOID fn_org = *fn_org_pp;
		*fn_org_pp = AllocTrampolineFunc(fn_org, nSize);
		WriteJmp(fn_org, pDetour, nSize, 0xE9);
	}
}