#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <Zut/ZxLite/Hasher.h>
#include <Zut/ZxLite/String.h>
#include <Zut/ZxLite/Console.h>
#include <Zut/ZxLite/Module.h>

TEST_CASE("string")
{
  using namespace ZQF::Zut;

  REQUIRE(ZxLite::FNV1a<std::uint64_t>::HashCStrArray(L"kernel32.dll") == 0x7f1bf8b449d16c2d);
  REQUIRE(ZxLite::FNV1a<std::uint64_t>::HashCStrArrayIgnoreCase(L"KerNel32.dll") == 0x7f1bf8b449d16c2d);
  static_assert(ZxLite::FNV1a<std::uint64_t>::HashCStrArrayCompileTime(L"kernel32.dll") == 0x7f1bf8b449d16c2d);
  static_assert(ZxLite::FNV1a<std::uint64_t>::HashCStrArrayIgnoreCaseCompileTime(L"KerNel32.dll") == 0x7f1bf8b449d16c2d);

  REQUIRE(ZxLite::FNV1a<std::uint64_t>::HashCStr(L"kernel32.dll") == 0x7f1bf8b449d16c2d);
  REQUIRE(ZxLite::FNV1a<std::uint64_t>::HashCStrIgnoreCase(L"keRnEL32.dll") == 0x7f1bf8b449d16c2d);
  static_assert(ZxLite::FNV1a<std::uint64_t>::HashCStrCompileTime(L"kernel32.dll") == 0x7f1bf8b449d16c2d);
  static_assert(ZxLite::FNV1a<std::uint64_t>::HashCStrIgnoreCaseCompileTime(L"KerNel32.dll") == 0x7f1bf8b449d16c2d);
}

TEST_CASE("string cmp")
{
  using namespace ZQF::Zut;

  constexpr auto str0 = "abcdefg224dgJUGusfs";
  constexpr auto str1 = "abcdefg224dgjuGusfs";
  constexpr auto str2 = "abcdeFg224dgjuGuSfs";
  REQUIRE(ZxLite::StrCmp(str0, str0) == 0);
  REQUIRE(ZxLite::StrCmp(str0, str1) != 0);
  REQUIRE(ZxLite::StrCmp(str0, str2) != 0);
  REQUIRE(ZxLite::StrCmp(str0, str2, true) == 0);
  REQUIRE(ZxLite::StrCmp(str0, "", true) != 0);
}

TEST_CASE("string view")
{
  using namespace ZQF::Zut;
  {
    const auto str_view = ZxLite::StrView{ "123djbweuiuwew" };
    REQUIRE(str_view.Size() == 14);
    REQUIRE(str_view.SizeBytes() == 14);
    REQUIRE(str_view.CStr() != nullptr);

    const auto rtl_str = str_view.GetRtlStr();
    REQUIRE(rtl_str.Buffer == str_view.CStr());
    REQUIRE(rtl_str.Length == str_view.SizeBytes());
    REQUIRE(rtl_str.MaximumLength == str_view.SizeBytes() + 1);

    const auto str_view_2 = ZxLite::StrView::FromRtlStr(rtl_str);
    REQUIRE(str_view_2.Size() == 14);
    REQUIRE(str_view_2.SizeBytes() == 14);
    REQUIRE(str_view_2.CStr() != nullptr);
  }

  {
    const auto str_view = ZxLite::WStrView{ L"123djbweuiuwew" };
    REQUIRE(str_view.Size() == 14);
    REQUIRE(str_view.SizeBytes() == (14 * 2));
    REQUIRE(str_view.CStr() != nullptr);

    const auto rtl_str = str_view.GetRtlStr();
    REQUIRE(rtl_str.Buffer == str_view.CStr());
    REQUIRE(rtl_str.Length == str_view.SizeBytes());
    REQUIRE(rtl_str.MaximumLength == str_view.SizeBytes() + 2);

    const auto str_view_2 = ZxLite::WStrView::FromRtlStr(rtl_str);
    REQUIRE(str_view_2.Size() == 14);
    REQUIRE(str_view_2.SizeBytes() == (14 * 2));
    REQUIRE(str_view_2.CStr() != nullptr);
  }
}

TEST_CASE("in code string")
{
  using namespace ZQF::Zut;
  {
    auto incode_str = ZxLite::InCodeStr("rwhouwe89gh29ib2vnwehnoweuiovbuiowebuvue9buosenvboseuoive");
    REQUIRE(incode_str.Size() == 57);
    REQUIRE(incode_str.SizeBytes() == 57);
    REQUIRE(incode_str.Data() != nullptr);
  }
  {
    auto incode_str = ZxLite::InCodeStr(L"egwuegiweuibguiewbguiwbeuibguiwebguibweuie");
    REQUIRE(incode_str.Size() == 42);
    REQUIRE(incode_str.SizeBytes() == (42 * 2));
    REQUIRE(incode_str.Data() != nullptr);
  }
  {
    constexpr auto incode_str = ZxLite::InCodeStr(L"egwuegiweuibguiewbguiwbeuibguiwebguibweuie");
    static_assert(incode_str.Size() == 42);
    static_assert(incode_str.SizeBytes() == (42 * 2));
  }
}

TEST_CASE("console")
{
  using namespace ZQF::Zut;

  ZxLite::OpenModule module_ntdll{ L"ntdll.dll" };
  REQUIRE(module_ntdll == true);
  ZxLite::Console console;
  console.Put<wchar_t>(L"ntdll.dll:");
  console.Put<wchar_t>(L"\n\tModuleName:");
  console.Put<wchar_t>(module_ntdll.ModuleName());
  console.Put<wchar_t>(L"\n\tModuleFullPath:");
  console.Put<wchar_t>(module_ntdll.ModuleFullPath());
}
