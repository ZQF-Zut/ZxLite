#pragma once
#include <Windows.h>
#include <Zut/ZxLite/String.h>


namespace ZQF::Zut::ZxLite
{
	class Injector
	{
	private:
		HANDLE m_hThread{};
		HANDLE m_hProcess{};

	public:
		Injector();
		~Injector();

	public:
		auto ViaAPC(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool;
		auto ViaRemoteThread(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool;
		auto Resume() const -> bool;
	};

}