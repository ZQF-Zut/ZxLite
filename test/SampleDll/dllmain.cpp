#include <Windows.h>
#include <Zut/ZxLite/Module.h>


BOOL APIENTRY DllEntryPoint(HINSTANCE /*hinstDLL*/, DWORD fdwReason, LPVOID /*lpReserved*/)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        wchar_t kernel32_name[]{ 0x006B, 0x0065, 0x0072, 0x006E, 0x0065, 0x006C, 0x0033, 0x0032, 0x002E, 0x0064, 0x006C, 0x006C, 0x0000 };
        wchar_t kernel3x_name[]{ 0x006B, 0x0065, 0x0072, 0x006E, 0x0065, 0x006C, 0x0033, 0x0032, 0x002E, 0x006C, 0x006C, 0x006C, 0x0000 };
        char func_name[]{ 0x43, 0x6F, 0x70, 0x79, 0x46, 0x69, 0x6C, 0x65, 0x57, 0x00 };
        ZQF::Zut::ZxLite::OpenModule kernel32_module{ {kernel32_name, 12 } };
        if (kernel32_module)
        {
            auto fn = kernel32_module.GetProcedure(ZQF::Zut::ZxLite::FNV1a<std::size_t>::HashCStrArray("CopyFileW"));
            if (fn)
            {
                reinterpret_cast<decltype(&::CopyFileW)>(fn)(kernel32_name, kernel3x_name, true);
            }
        }
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