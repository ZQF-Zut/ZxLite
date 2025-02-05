#include <phnt.h>
#include <span>
#include <Zut/ZxLite/MsgBox.h>
#include <Zut/ZxLite/Console.h>
#include <Zut/ZxLite/Module.h>
#include <Zut/ZxLite/Injector.h>

using namespace ZQF::Zut;


static auto IntegerToStringImp(const std::span<wchar_t> spBuffer, const std::size_t nValue, const std::size_t nBase) -> ZxLite::WStrView
{
    UNICODE_STRING dll_base_ustr{ .Length = 0,.MaximumLength = (USHORT)spBuffer.size(),.Buffer = spBuffer.data() };
    const auto status = ::RtlIntegerToUnicodeString(static_cast<ULONG>(nValue), static_cast<ULONG>(nBase), &dll_base_ustr);
    return status == STATUS_SUCCESS ? ZxLite::WStrView{ dll_base_ustr.Buffer, dll_base_ustr.Length } : L"";
}

template<typename T>
static auto IntegerToString(const std::span<wchar_t> spBuffer, const T tValue, const std::size_t nBase = 16) -> ZxLite::WStrView
{
    return IntegerToStringImp(spBuffer, reinterpret_cast<std::size_t>(tValue), nBase);
}


auto Start() -> size_t
{
    //ZxLite::PutConsole<char>("hello, world!\n");
    //ZxLite::PutConsole<wchar_t>(L"hello, zxlite!\n");
    //ZxLite::MsgBox(L"可爱捏!", L"恒星妹妹！");

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
	injector.ViaAPC(LR"(D:\Soft\extra\Disk\Hash.exe)", LR"(C:\Users\Ptr\Desktop\vfseditor.dll)");
	injector.Resume();

	return 0;
}