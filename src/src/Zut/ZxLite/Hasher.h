#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>


namespace ZQF::Zut::ZxLite
{
	template<typename HashValueType>
	class FNV1a
	{
	private:
		template<typename HashValueType>
		struct FNV1a_MagicNum {};

		template<>
		struct FNV1a_MagicNum<std::uint32_t>
		{
			inline static constexpr std::uint32_t Prime = 16777619U;
			inline static constexpr std::uint32_t Seed = 2166136261U;
		};

		template<>
		struct FNV1a_MagicNum<std::uint64_t>
		{
			inline static constexpr std::uint64_t Prime = 1099511628211ULL;
			inline static constexpr std::uint64_t Seed = 14695981039346656037ULL;
		};

		static constexpr auto CharLow(const char cChar) -> char
		{
			return ((cChar >= 'A') && (cChar <= 'Z')) ? (cChar + ('a' - 'A')) : cChar;
		}

		static constexpr auto CharLow(const wchar_t wChar) -> wchar_t
		{
			return ((wChar >= L'A') && (wChar <= L'Z')) ? (wChar + (L'a' - L'A')) : wChar;
		}

	public:
		template<typename T>
		static constexpr auto HashInterger(const T tValue, const HashValueType nHash = FNV1a_MagicNum<HashValueType>::Seed) -> HashValueType
		{
			HashValueType hash = nHash;
			if constexpr (std::is_same_v<T, std::uint8_t>)
			{
				hash = (tValue ^ hash) * FNV1a_MagicNum<HashValueType>::Prime;
			}
			else if constexpr (std::is_same_v<T, std::uint16_t>)
			{
				hash = FNV1a::HashInterger<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 0) & 0xFF), hash);
				hash = FNV1a::HashInterger<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 8) & 0xFF), hash);
			}
			else if constexpr (std::is_same_v<T, std::uint32_t>)
			{
				hash = FNV1a::HashInterger<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 0) & 0xFF), hash);
				hash = FNV1a::HashInterger<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 8) & 0xFF), hash);
				hash = FNV1a::HashInterger<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 16) & 0xFF), hash);
				hash = FNV1a::HashInterger<std::uint8_t>(static_cast<std::uint8_t>((tValue >> 24) & 0xFF), hash);
			}
			else
			{
				static_assert(false, "FNV1a::Hash(): error type");
			}

			return hash;
		}

		template<typename ElementType, bool isIgnoreCase = false>
		static constexpr auto HashMemory(const ElementType* pText, const std::size_t nSize) -> HashValueType
		{
			HashValueType hash = FNV1a_MagicNum<HashValueType>::Seed;

			for (std::size_t idx = 0; idx < nSize; idx++)
			{
				auto char_v = pText[idx];
				if constexpr (isIgnoreCase)
				{
					char_v = FNV1a::CharLow(char_v);
				}

				if constexpr (sizeof(ElementType) == 1)
				{
					hash = FNV1a::HashInterger<std::uint8_t>(char_v, hash);
				}
				else if constexpr (sizeof(ElementType) == 2)
				{
					hash = FNV1a::HashInterger<std::uint16_t>(char_v, hash);
				}
				else if constexpr (sizeof(ElementType) == 4)
				{
					hash = FNV1a::HashInterger<std::uint32_t>(char_v, hash);
				}
				else
				{
					static_assert(false, "FNV1a::HashMemory(): error type");
				}
			}

			return hash;
		}

		template<typename CStrCharType>
		static constexpr auto HashCStr(const CStrCharType* pText, const std::size_t nLen) -> HashValueType
		{
			return FNV1a::HashMemory<CStrCharType>(pText, nLen);
		}

		template<typename CStrCharType>
		static consteval auto HashCStrCompileTime(const CStrCharType* pText, const std::size_t nLen) -> HashValueType
		{
			return FNV1a::HashMemory<CStrCharType>(pText, nLen);
		}

		template<typename CStrCharType>
		static constexpr auto HashCStrIgnoreCase(const CStrCharType* pText, const std::size_t nLen) -> HashValueType
		{
			return FNV1a::HashMemory<CStrCharType, true>(pText, nLen);
		}

		template<typename CStrCharType>
		static consteval auto HashCStrIgnoreCaseCompileTime(const CStrCharType* pText, const std::size_t nLen) -> HashValueType
		{
			return FNV1a::HashMemory<CStrCharType, true>(pText, nLen);
		}


		template<typename CStrCharType, bool isIgnoreCase = false>
		static constexpr auto HashCStr(const CStrCharType* pText) -> HashValueType
		{
			HashValueType hash = FNV1a_MagicNum<HashValueType>::Seed;

			while (*pText)
			{
				auto char_v = *pText++;
				if constexpr (isIgnoreCase)
				{
					char_v = FNV1a::CharLow(char_v);
				}

				if constexpr (sizeof(CStrCharType) == 1)
				{
					hash = FNV1a::HashInterger<std::uint8_t>(char_v, hash);
				}
				else if constexpr (sizeof(CStrCharType) == 2)
				{
					hash = FNV1a::HashInterger<std::uint16_t>(char_v, hash);
				}
				else if constexpr (sizeof(CStrCharType) == 4)
				{
					hash = FNV1a::HashInterger<std::uint32_t>(char_v, hash);
				}
				else
				{
					static_assert(false, "FNV1a::HashCStr(): error type");
				}
			}

			return hash;
		}

		template<typename CStrCharType>
		static consteval auto HashCStrCompileTime(const CStrCharType* pText) -> HashValueType
		{
			return FNV1a::HashCStr<CStrCharType>(pText);
		}

		template<typename CStrCharType>
		static constexpr auto HashCStrIgnoreCase(const CStrCharType* pText) -> HashValueType
		{
			return FNV1a::HashCStr<CStrCharType, true>(pText);
		}

		template<typename CStrCharType>
		static consteval auto HashCStrIgnoreCaseCompileTime(const CStrCharType* pText) -> HashValueType
		{
			return FNV1a::HashCStr<CStrCharType, true>(pText);
		}


		template<typename ArrayElementType, std::size_t N>
		static constexpr auto HashCStrArray(const ArrayElementType(&aText)[N]) -> HashValueType
		{
			return FNV1a::HashMemory<ArrayElementType>(aText, N - 1);
		}

		template<typename ArrayElementType, std::size_t N>
		static consteval auto HashCStrArrayCompileTime(const ArrayElementType(&aText)[N]) -> HashValueType
		{
			return FNV1a::HashMemory<ArrayElementType>(aText, N - 1);
		}

		template<typename ArrayElementType, std::size_t N>
		static constexpr auto HashCStrArrayIgnoreCase(const ArrayElementType(&aText)[N]) -> HashValueType
		{
			return FNV1a::HashMemory<ArrayElementType, true>(aText, N - 1);
		}

		template<typename ArrayElementType, std::size_t N>
		static consteval auto HashCStrArrayIgnoreCaseCompileTime(const ArrayElementType(&aText)[N]) -> HashValueType
		{
			return FNV1a::HashMemory<ArrayElementType, true>(aText, N - 1);
		}
	};
}