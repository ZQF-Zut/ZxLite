#include <Windows.h>


namespace RLite
{
	class StrView
	{
	public:
		const char* m_cpStr;
		SIZE_T m_nLen;

		constexpr StrView() noexcept : m_cpStr(NULL), m_nLen(NULL) {};
		template<SIZE_T N> constexpr StrView(const char(&aStr) [N]) noexcept : m_cpStr(aStr), m_nLen(N) {};
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

namespace RLite::Cmd
{
	static constexpr size_t PUT_BUFFER_MAX = 1024;
	static HANDLE sg_hInputHandle = INVALID_HANDLE_VALUE;
	static HANDLE sg_hOutputHandle = INVALID_HANDLE_VALUE;

	BOOL Alloc()
	{
		::AllocConsole();
		::AttachConsole(ATTACH_PARENT_PROCESS);
		sg_hInputHandle = ::GetStdHandle(STD_INPUT_HANDLE);
		sg_hOutputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		return (sg_hOutputHandle == INVALID_HANDLE_VALUE || sg_hInputHandle == INVALID_HANDLE_VALUE) ? false : true;
	}

	BOOL SetTitle(LPCWSTR wpText)
	{
		return ::SetConsoleTitleW(wpText);
	}

	BOOL Write(LPCSTR cpText, DWORD nCharsToWrite)
	{
		DWORD written = 0;
		return ::WriteConsoleA(sg_hOutputHandle, cpText, nCharsToWrite, &written, NULL);
	}

	BOOL Write(LPCWSTR wpText, DWORD nCharsToWrite)
	{
		DWORD written = 0;
		return ::WriteConsoleW(sg_hOutputHandle, wpText, nCharsToWrite, &written, NULL);
	}

	BOOL Read(LPSTR cpBuffer, DWORD nCharsToRead)
	{
		DWORD read = 0;
		return ::ReadConsoleA(sg_hInputHandle, cpBuffer, nCharsToRead, &read, NULL);
	}

	BOOL Read(LPWSTR wpBuffer, DWORD nCharsToRead)
	{
		DWORD read = 0;
		return ::ReadConsoleW(sg_hInputHandle, wpBuffer, nCharsToRead, &read, NULL);
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
			::ReadConsoleW(sg_hInputHandle, &buffer, 1, &read, NULL);
			if (buffer != L'\r') { continue; }
			::ReadConsoleW(sg_hInputHandle, &buffer, 1, &read, NULL);
			if (buffer != L'\n') { continue; }
			return;
		}
	}

	VOID Wait()
	{
		wchar_t buffer[1];
		RLite::Cmd::Read(buffer, 1);
	}

	BOOL PutFormat(LPCSTR cpFormat, ...)
	{
		char buffer[PUT_BUFFER_MAX];

		va_list args = nullptr;
		va_start(args, cpFormat);
		size_t cch = ::wvsprintfA(buffer, cpFormat, args);
		va_end(args);

		return (cch <= 0) ? (false) : (Write(buffer, cch));
	}

	BOOL PutFormat(LPCWSTR wpFormat, ...)
	{
		wchar_t buffer[PUT_BUFFER_MAX];

		va_list args = nullptr;
		va_start(args, wpFormat);
		size_t cch = ::wvsprintfW(buffer, wpFormat, args);
		va_end(args);

		return (cch <= 0) ? (false) : (Write(buffer, cch));
	}
}

namespace RLite::SysMem
{
	template<class T> T Alloc(SIZE_T nSize, DWORD uiAccess = PAGE_READWRITE)
	{
		return (T)::VirtualAlloc(NULL, nSize, MEM_COMMIT, uiAccess);
	}

	template<class T> BOOL Free(T pAddress)
	{
		return ::VirtualFree((T)pAddress, 0, MEM_RELEASE);
	}
}

namespace RLite
{
	class Auto
	{
	public:
		PBYTE m_pMem;
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
			SysMem::Free(m_pMem);
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
			m_pMem = SysMem::Alloc<PBYTE>(nNewSize);
			m_nMemSize = nNewSize;
		}
		else if (nNewSize > m_nMemSize)
		{
			if (m_pMem != NULL)
			{
				SysMem::Free(m_pMem);
			}
			m_pMem = SysMem::Alloc<PBYTE>(nNewSize);
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
