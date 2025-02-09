#include <phnt.h>
#include <Zut/ZxLite/MsgBox.h>
#include <Zut/ZxLite/Console.h>
#include <Zut/ZxLite/Module.h>
#include <Zut/ZxLite/Injector.h>

using namespace ZQF::Zut;




[[maybe_unused]] static auto TestStrcmp()
{
    //auto x0 = ZxLite::FNV1a<std::size_t>::HashCStrCompileTime(L"kernel32.dll");
    //auto x1 = ZxLite::FNV1a<std::size_t>::HashCStr(L"kernel32.dll");
    //auto x2 = ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCase(L"keRneL32.dll");
    //auto x3 = ZxLite::FNV1a<std::size_t>::HashCStrIgnoreCaseCompileTime(L"kernel32.dll");
    //int a{};

    //constexpr auto str0 = "abcdefg224dgJUGusfs";
    //constexpr auto str1 = "abcdefg224dgjuGusfs";
    //const auto compare_status_0 = ZxLite::StrCmp(str0, str0);
    //const auto compare_status_1 = ZxLite::StrCmp(str0, str1);
    //const auto compare_status_2 = ZxLite::StrCmp(str0, str1, true);
    //const auto compare_status_3 = ZxLite::StrCmp(str0, "", true);
}


auto Start() -> size_t
{
    // TestStrcmp();
    //ZxLite::PutConsole<char>("hello, world!\n");
    //ZxLite::PutConsole<wchar_t>(L"hello, zxlite!\n");
    const auto str0 = ZxLite::InCodeStr(L"窗口弹出测试！");
    const auto str1 = ZxLite::InCodeStr(L"可以看到窗口吗？");
    ZxLite::MsgBox{}.ShowInfo(str0.CStr(), str1.CStr());

    //ZxLite::OpenModule module_ntdll{ L"ntdll.dll" };
    //if (module_ntdll == false)
    //{
    //    ZxLite::MsgBox(L"Error", L"not find target module!");
    //    return 0;
    //}

    //wchar_t buffer[64];
    //const auto dll_base_str = IntegerToString(buffer, module_ntdll.ImageBase(), 16);

    //ZxLite::PutConsole<wchar_t>(L"ntdll.dll:");
    //ZxLite::PutConsole<wchar_t>(L"\n\tImageBase:0x");
    //ZxLite::PutConsole<wchar_t>(dll_base_str);
    //ZxLite::PutConsole<wchar_t>(L"\n\tModuleName:");
    //ZxLite::PutConsole<wchar_t>(module_ntdll.ModuleName());
    //ZxLite::PutConsole<wchar_t>(L"\n\tModuleFullPath:");
    //ZxLite::PutConsole<wchar_t>(module_ntdll.ModuleFullPath());


	//ZxLite::Injector injector;
	//injector.ViaAPC(LR"(Hash.exe)", LR"(vfseditor.dll)");
	//injector.Resume();

	return 0;
}