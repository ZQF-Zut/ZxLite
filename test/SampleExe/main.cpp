#include <phnt.h>
#include <Zut/ZxLite/MsgBox.h>
#include <Zut/ZxLite/Console.h>
#include <Zut/ZxLite/Module.h>
#include <Zut/ZxLite/Injector.h>

using namespace ZQF::Zut;


auto Start() -> size_t
{
    //ZxLite::PutConsole<char>("hello, world!\n");
    //ZxLite::PutConsole<wchar_t>(L"hello, zxlite!\n");
    ZxLite::MsgBox::ShowInfo(L"窗口弹出测试！", L"可以看到窗口吗？");

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


	ZxLite::Injector injector;
	injector.ViaRemoteThread(LR"(Hash.exe)", LR"(vfseditor.dll)");
	injector.Resume();

	return 0;
}