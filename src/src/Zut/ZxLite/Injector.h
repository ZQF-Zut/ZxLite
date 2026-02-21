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
    Injector(const Injector&) noexcept = delete;
    Injector(Injector&&) = delete;
    Injector& operator=(const Injector&) = delete;
    Injector& operator=(Injector&&) noexcept = delete;
    ~Injector();

  public:
    auto ViaAPC(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool;
    auto ViaRemoteThread(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool;
    auto ViaMmap(const ZxLite::WStrView wsExePath, const ZxLite::WStrView wsDllPath) -> bool;
    auto Resume() const -> bool;
  };

} // namespace ZQF::Zut::ZxLite
