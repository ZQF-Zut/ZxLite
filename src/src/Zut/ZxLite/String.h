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

	auto CharUp(const wchar_t wChar) -> wchar_t;
	auto StrCmp(const UNICODE_STRING& rfUStr0, const UNICODE_STRING& rfUStr1, const bool isIgnoreCase) -> std::size_t;


	template<typename T>
	class FNV1a
	{
	private:
		template<typename T>
		struct FNV1a_Seed {};

		template<>
		struct FNV1a_Seed<std::uint32_t>
		{
			inline static constexpr std::uint32_t Prime = 16777619U;
			inline static constexpr std::uint32_t Seed = 2166136261U;
		};

		template<>
		struct FNV1a_Seed<std::uint64_t>
		{
			inline static constexpr std::uint64_t Prime = 1099511628211ULL;
			inline static constexpr std::uint64_t Seed = 14695981039346656037ULL;
		};

	public:
		template<typename U>
		static constexpr auto Hash(const U tValue, const T nHash = FNV1a_Seed<T>::Seed) -> T
		{
			T hash = nHash;
			if constexpr (std::is_same_v<U, std::uint8_t>)
			{
				hash = (tValue ^ hash) * FNV1a_Seed<T>::Prime;
			}
			else if constexpr (std::is_same_v<U, std::uint16_t>)
			{
				hash = FNV1a::Hash<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 0) & 0xFF), hash);
				hash = FNV1a::Hash<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 8) & 0xFF), hash);
			}
			else if constexpr (std::is_same_v<U, std::uint32_t>)
			{
				hash = FNV1a::Hash<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 0) & 0xFF), hash);
				hash = FNV1a::Hash<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 8) & 0xFF), hash);
				hash = FNV1a::Hash<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 16) & 0xFF), hash);
				hash = FNV1a::Hash<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 24) & 0xFF), hash);
			}
			else
			{
				static_assert(false, "error type");
			}

			return hash;
		}

		template<typename U>
		static constexpr auto HashStr(const U* pText) -> T
		{
			T hash = FNV1a_Seed<T>::Seed;
			while (*pText)
			{
				if constexpr (sizeof(wchar_t) == 1)
				{
					hash = FNV1a::Hash<std::uint8_t>(*pText++, hash);
				}
				else if constexpr (sizeof(wchar_t) == 2)
				{
					hash = FNV1a::Hash<std::uint16_t>(*pText++, hash);
				}
				else if constexpr (sizeof(wchar_t) == 4)
				{
					hash = FNV1a::Hash<std::uint32_t>(*pText++, hash);
				}
				else
				{
					static_assert(false, "error type");
				}
			}
			return hash;
		}

		template<typename U>
		static constexpr auto HashStr(const U* pText, const std::size_t nSize) -> T
		{
			T hash = FNV1a_Seed<T>::Seed;
			for (std::size_t idx = 0; idx < nSize; idx++)
			{
				if constexpr (sizeof(U) == 1)
				{
					hash = FNV1a::Hash<std::uint8_t>(pText[idx], hash);
				}
				else if constexpr (sizeof(U) == 2)
				{
					hash = FNV1a::Hash<std::uint16_t>(pText[idx], hash);
				}
				else if constexpr (sizeof(U) == 4)
				{
					hash = FNV1a::Hash<std::uint32_t>(pText[idx], hash);
				}
				else
				{
					static_assert(false, "error type");
				}
			}
			return hash;
		}

		template<typename U, std::size_t N>
		static consteval auto HashStrConst(const U(&aText)[N]) -> T
		{
			return FNV1a::HashStr(aText, N);
		}
	};
}