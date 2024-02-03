#include <RxLite/RxLite.h>


DWORD Start()
{
	RLite::Cmd::Alloc();
	RLite::Cmd::SetTitle(L"MyConsole");

	wchar_t buffer[1];

	RLite::Cmd::Read(buffer, 1);
	RLite::Cmd::ClearInput();
	RLite::Cmd::Write(buffer, 1);
	RLite::Cmd::Write(L"\n", 1);

	RLite::Cmd::Read(buffer, 1);
	RLite::Cmd::ClearInput();
	RLite::Cmd::Write(buffer, 2);
	RLite::Cmd::Write(L"\n", 1);

	RLite::Cmd::PutFormat(L"%s-%s\n", L"txt", L"exe");

	RLite::Cmd::Wait();
	return 0;
}