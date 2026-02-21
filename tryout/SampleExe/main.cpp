#include <phnt_windows.h>
#include <phnt.h>
#include <Zut/ZxLite/Hasher.h>
#include <Zut/ZxLite/MsgBox.h>
#include <Zut/ZxLite/Console.h>
#include <Zut/ZxLite/Module.h>
#include <Zut/ZxLite/Injector.h>

using namespace ZQF::Zut;

extern "C" auto Start() -> size_t
{
  ZxLite::OpenModule module_ntdll{ L"ntdll.dll" };
  ZxLite::Console console;
  console.Put<wchar_t>(L"ntdll.dll:");
  console.Put<wchar_t>(L"\n\tModuleName:");
  console.Put<wchar_t>(module_ntdll.ModuleName());
  console.Put<wchar_t>(L"\n\tModuleFullPath:");
  console.Put<wchar_t>(module_ntdll.ModuleFullPath());

  // ZxLite::Injector injector;
  // injector.ViaMmap(LR"(Hash.exe)", LR"(vfseditor.dll)");
  // injector.Resume();
  return 0;
}
