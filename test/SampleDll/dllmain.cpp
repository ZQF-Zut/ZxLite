#include <RxLite/RxLite.h>


BOOL APIENTRY DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        //::MessageBoxW(NULL, L"DLL_PROCESS_ATTACH", NULL, NULL);
        break;
    case DLL_THREAD_ATTACH:
        //::MessageBoxW(NULL, L"DLL_THREAD_ATTACH", NULL, NULL);
        break;
    case DLL_THREAD_DETACH:
        //::MessageBoxW(NULL, L"DLL_THREAD_DETACH", NULL, NULL);
        break;
    case DLL_PROCESS_DETACH:
        //::MessageBoxW(NULL, L"DLL_PROCESS_DETACH", NULL, NULL);
        break;
    }
    return TRUE;
}

VOID  __declspec(dllexport) DirA() {}