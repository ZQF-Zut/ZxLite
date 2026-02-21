#include <Windows.h>
#include <Zut/ZxLite/String.h>
#include <Zut/ZxLite/MsgBox.h>

using namespace ZQF::Zut;


BOOL APIENTRY DllEntryPoint(HINSTANCE /*hinstDLL*/, DWORD fdwReason, LPVOID /*lpReserved*/)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        const auto title = ZxLite::InCodeStr(L"标题");
        const auto text = ZxLite::InCodeStr(L"测试文本！测试InCodeStr是否正常显示。");
        ZQF::Zut::ZxLite::MsgBox{}.ShowInfo(title.CStr(), text.CStr());
    }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//VOID  __declspec(dllexport) DirA() {}