#include "String.h"
#include <span>

namespace ZQF::Zut::ZxLite
{
  auto NumToStrImp(const std::span<wchar_t> spBuffer, const std::size_t nValue, const std::size_t nBase) -> ZxLite::WStrView
  {
    UNICODE_STRING dll_base_ustr{ .Length = 0, .MaximumLength = static_cast<USHORT>(spBuffer.size()), .Buffer = spBuffer.data() };
    const auto status = ::RtlIntegerToUnicodeString(static_cast<ULONG>(nValue), static_cast<ULONG>(nBase), &dll_base_ustr);
    return status == STATUS_SUCCESS ? ZxLite::WStrView{ dll_base_ustr.Buffer, dll_base_ustr.Length } : L"";
  }

  auto CharUp(const char cChar) -> char
  {
    return ((cChar >= 'a') && (cChar <= 'z')) ? static_cast<char>(cChar - ('a' - 'A')) : cChar;
  }

  auto CharUp(const wchar_t wChar) -> wchar_t
  {
    return ((wChar >= L'a') && (wChar <= L'z')) ? static_cast<wchar_t>(wChar - (L'a' - L'A')) : wChar;
  }

  auto StrCmp(const UNICODE_STRING& rfUStr0, const UNICODE_STRING& rfUStr1, const bool isIgnoreCase) -> std::size_t
  {
    return ZxLite::StrCmp(rfUStr0.Buffer, rfUStr0.Length, rfUStr1.Buffer, rfUStr1.Length, isIgnoreCase);
  }

  auto MemCpy(void* dest, const void* src, size_t count) -> void*
  {
    return ::memcpy(dest, src, count);
  }
} // namespace ZQF::Zut::ZxLite
