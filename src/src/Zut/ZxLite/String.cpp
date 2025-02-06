#include "String.h"
#include <span>


namespace ZQF::Zut::ZxLite
{
	auto NumToStrImp(const std::span<wchar_t> spBuffer, const std::size_t nValue, const std::size_t nBase) -> ZxLite::WStrView
	{
		UNICODE_STRING dll_base_ustr{ .Length = 0,.MaximumLength = (USHORT)spBuffer.size(),.Buffer = spBuffer.data() };
		const auto status = ::RtlIntegerToUnicodeString(static_cast<ULONG>(nValue), static_cast<ULONG>(nBase), &dll_base_ustr);
		return status == STATUS_SUCCESS ? ZxLite::WStrView{ dll_base_ustr.Buffer, dll_base_ustr.Length } : L"";
	}

	auto CharUp(const wchar_t wChar) -> wchar_t
	{
		return ((wChar >= L'a') && (wChar <= L'z')) ? (wChar - (L'a' - L'A')) : wChar;
	}

	auto StrCmp(const UNICODE_STRING& rfUStr0, const UNICODE_STRING& rfUStr1, const bool isIgnoreCase) -> std::size_t
	{
		auto bytes = (rfUStr0.Length > rfUStr1.Length ? rfUStr1.Length : rfUStr0.Length) / sizeof(WCHAR);
		auto str0_ptr = rfUStr0.Buffer;
		auto str1_ptr = rfUStr1.Buffer;

		std::size_t ret{};
		if (isIgnoreCase)
		{
			while ((ret == 0) && (bytes--))
			{
				ret = ZxLite::CharUp(*str0_ptr++) - ZxLite::CharUp(*str1_ptr++);
			}
		}
		else
		{
			while ((ret == 0) && (bytes--))
			{
				ret = *str0_ptr++ - *str1_ptr++;
			}
		}

		if (ret == 0)
		{
			ret = rfUStr0.Length - rfUStr1.Length;
		}

		return ret;
	}
}