#include "MsgBox.h"
#include "Module.h"
#include "Hasher.h"
#include <phnt.h>

namespace ZQF::Zut::ZxLite
{
  MsgBox::MsgBox()
  {
    ZQF::Zut::ZxLite::OpenModule ntdll_module{ ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"ntdll.dll") };
    if (!ntdll_module)
    {
      return;
    }

    m_fnNtRaiseHardError = ntdll_module.GetProcedureBase(ZxLite::FNV1a<std::size_t>::HashCStrCompileTime("NtRaiseHardError"));
  }

  auto MsgBox::ShowInfo(const ZxLite::WStrView wsTitle, const ZxLite::WStrView wsText) -> MsgBox::Respon
  {
    if (m_fnNtRaiseHardError == nullptr)
    {
      return MsgBox::Respon::Ignore;
    }

    UNICODE_STRING title_unicode_str{ wsTitle.GetRtlStr() };
    UNICODE_STRING text_unicode_str{ wsText.GetRtlStr() };

    ResponParameters parameters{ .pTitle = &text_unicode_str, .pText = &title_unicode_str, .eStyle = (ULONG)MsgBox::Button::OK | (ULONG)MsgBox::Options::TopMost | (ULONG)MsgBox::Icon::Information, .nTimeout = INFINITE };

    ULONG respond{};
    reinterpret_cast<decltype(&::NtRaiseHardError)>(m_fnNtRaiseHardError)(STATUS_SERVICE_NOTIFICATION, ResponParameters::PARAM_COUNT, 0x3, reinterpret_cast<PULONG_PTR>(&parameters), (ULONG)Type::OK, &respond);
    return static_cast<MsgBox::Respon>(respond);
  }
} // namespace ZQF::Zut::ZxLite
