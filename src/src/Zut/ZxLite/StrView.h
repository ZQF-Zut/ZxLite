#pragma once
#include <phnt.h>
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
		constexpr BaseStrView() noexcept : m_pStr{}, m_nSize{} {};
		template<Size_Type N> constexpr BaseStrView(const Value_Type(&aStr)[N]) noexcept : m_pStr(aStr), m_nSize(N) {};
		constexpr BaseStrView(const Value_Type* pStr) noexcept;
		template<typename U> constexpr BaseStrView(const Value_Type* pStr, const U nSize) noexcept : m_pStr(pStr), m_nSize(static_cast<Size_Type>(nSize)) {};
		constexpr auto Data() const noexcept -> const Value_Type* { return m_pStr; }
		constexpr auto Size() const noexcept -> Size_Type { return m_nSize; }
		constexpr auto SizeBytes() const noexcept -> Size_Type { return m_nSize * sizeof(Value_Type); }
		constexpr auto GetRtlStr() const
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

		template<typename U>
		static constexpr auto FromRtlStr(const U& rfRtlStr) -> BaseStrView<Value_Type>
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
	};

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

	using StrView = BaseStrView<char>;
	using WStrView = BaseStrView<wchar_t>;
}