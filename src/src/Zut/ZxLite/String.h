#pragma once
#include <phnt.h>
#include <span>
#include <cstddef>
#include <type_traits>
#include <Zut/ZxLite/ZIntrinsic.h>


namespace ZQF::Zut::ZxLite
{
	template<typename T>
	class BaseStrView
	{
	private:
		using Value_Type = T;
		using Size_Type = std::size_t;

	private:
		const Value_Type* m_pStr;
		Size_Type m_nSize;

	public:
		constexpr BaseStrView() noexcept;
		template<Size_Type N> 
		constexpr BaseStrView(const Value_Type(&aStr)[N]) noexcept : m_pStr(aStr), m_nSize(N){}
		constexpr BaseStrView(const Value_Type* pStr) noexcept;
		template<typename U> 
		constexpr BaseStrView(const Value_Type* pStr, const U nSize) noexcept;

	public:
		constexpr auto Data() const noexcept -> const Value_Type*;
		constexpr auto Size() const noexcept -> Size_Type;
		constexpr auto SizeBytes() const noexcept -> Size_Type;

	public:
		constexpr auto GetRtlStr() const noexcept;
		template<typename U> 
		static constexpr auto FromRtlStr(const U& rfRtlStr) -> BaseStrView<Value_Type>;
	};

	template<typename T>
	inline constexpr BaseStrView<T>::BaseStrView() noexcept : m_pStr{}, m_nSize{}
	{

	}

	template<typename T>
	inline constexpr BaseStrView<T>::BaseStrView(const Value_Type* pStr) noexcept : m_pStr(pStr)
	{
		if constexpr (std::is_same_v<Value_Type, char>)
		{
			m_nSize = ::strlen(pStr);
		}
		else if constexpr (std::is_same_v<Value_Type, wchar_t>)
		{
			m_nSize = ::wcslen(pStr);
		}
		else
		{
			static_assert(false, "error value type");
		}
	};

	template<typename T>
	template<typename U> 
	inline constexpr BaseStrView<T>::BaseStrView(const Value_Type* pStr, const U nSize) noexcept : m_pStr(pStr), m_nSize(static_cast<Size_Type>(nSize))
	{

	}

	template<typename T>
	inline constexpr auto BaseStrView<T>::Data() const noexcept -> const Value_Type* 
	{ 
		return m_pStr;
	}

	template<typename T>
	inline constexpr auto BaseStrView<T>::Size() const noexcept -> Size_Type 
	{ 
		return m_nSize;
	}

	template<typename T>
	inline constexpr auto BaseStrView<T>::SizeBytes() const noexcept -> Size_Type 
	{ 
		return m_nSize * sizeof(Value_Type);
	}

	template<typename T>
	inline constexpr auto BaseStrView<T>::GetRtlStr() const noexcept
	{
		if constexpr (std::is_same_v<Value_Type, char>)
		{
			return ANSI_STRING
			{
				.Length = (USHORT)this->SizeBytes(),
				.MaximumLength = (USHORT)(this->SizeBytes() + 1),
				.Buffer = (PCHAR)this->Data()
			};
		}
		else if constexpr (std::is_same_v<Value_Type, wchar_t>)
		{
			return UNICODE_STRING
			{
				.Length = (USHORT)this->SizeBytes(),
				.MaximumLength = (USHORT)(this->SizeBytes() + 2),
				.Buffer = (PWCH)this->Data()
			};
		}
		else
		{
			static_assert(false, "error value type");
		}
	}

	template<typename T>
	template<typename U>
	inline constexpr auto BaseStrView<T>::FromRtlStr(const U& rfRtlStr) -> BaseStrView<Value_Type>
	{
		if constexpr (std::is_same_v<Value_Type, char>)
		{
			return BaseStrView<T>{ rfRtlStr.Buffer, rfRtlStr.Length };
		}
		else if constexpr (std::is_same_v<Value_Type, wchar_t>)
		{
			return BaseStrView<T>{ rfRtlStr.Buffer, rfRtlStr.Length / 2 };
		}
		else
		{
			static_assert(false, "error value type");
		}
	}

	using StrView = BaseStrView<char>;
	using WStrView = BaseStrView<wchar_t>;
}

namespace ZQF::Zut::ZxLite
{
	auto NumToStrImp(const std::span<wchar_t> spBuffer, const std::size_t nValue, const std::size_t nBase) -> ZxLite::WStrView;

	template<typename T>
	static auto NumToStr(const std::span<wchar_t> spBuffer, const T tValue, const std::size_t nBase = 16) -> ZxLite::WStrView
	{
		return NumToStrImp(spBuffer, reinterpret_cast<std::size_t>(tValue), nBase);
	}

	auto CharUp(const char cChar) -> char;
	auto CharUp(const wchar_t wChar) -> wchar_t;
	auto StrCmp(const UNICODE_STRING& rfUStr0, const UNICODE_STRING& rfUStr1, const bool isIgnoreCase) -> std::size_t;

	template<typename T>
	auto StrCmp(const T* pStr0, const std::size_t nStr0Len, const T* pStr1, const std::size_t nStr1Len, const bool isIgnoreCase = false) -> std::size_t
	{
		auto bytes = (nStr0Len > nStr1Len ? nStr1Len : nStr0Len) / sizeof(T);

		std::size_t ret{};
		if (isIgnoreCase)
		{
			while ((ret == 0) && (bytes--))
			{
				ret = ZxLite::CharUp(*pStr0++) - ZxLite::CharUp(*pStr1++);
			}
		}
		else
		{
			while ((ret == 0) && (bytes--))
			{
				ret = *pStr0++ - *pStr1++;
			}
		}

		if (ret == 0)
		{
			ret = nStr0Len - nStr1Len;
		}

		return ret;
	}

	template<typename T>
	auto StrCmp(const T* pStr0, const T* pStr1, const bool isIgnoreCase = false) -> std::size_t
	{
		std::size_t ret{ SIZE_MAX };

		if (pStr0 == nullptr || pStr1 == nullptr)
		{
			return ret;
		}

		if (isIgnoreCase)
		{
			for (std::size_t idx = 0; ; idx++)
			{
				const auto char_0 = pStr0[idx];
				if (char_0 == 0)
				{
					break;
				}

				const auto char_1 = pStr1[idx];
				if (char_1 == 0)
				{
					break;
				}

				ret = ZxLite::CharUp(char_0) - ZxLite::CharUp(char_1);
				if (ret != 0)
				{
					return static_cast<std::size_t>(ret);
				}
			}
		}
		else
		{
			for (std::size_t idx = 0; ; idx++)
			{
				const auto char_0 = pStr0[idx];
				if (char_0 == 0)
				{
					break;
				}

				const auto char_1 = pStr1[idx];
				if (char_1 == 0)
				{
					break;
				}

				ret = char_0 - char_1;
				if (ret != 0)
				{
					return static_cast<std::size_t>(ret);
				}
			}
		}

		return ret;
	}

	template<typename T>
	auto StrLen(const T* pStr) -> std::size_t
	{
		std::size_t len;
		for (len = 0; *pStr != 0; ++pStr)
		{
			++len;
		}
		return len;
	}
}