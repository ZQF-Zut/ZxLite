#pragma once
#include <Windows.h>
#include <Zut/ZxLite/StrView.h>


namespace ZQF::Zut::ZxLite
{
	class Injector
	{
	private:
		PROCESS_INFORMATION m_ProcessInfo{ };

	public:
		Injector();
		~Injector();

	public:
		auto ViaAPC(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool;
		auto ViaRemoteThread(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool;
		auto Resume() const -> bool;
	};

}