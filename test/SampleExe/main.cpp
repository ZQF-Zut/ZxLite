#include <RxLite/RxLite.h>


DWORD Start()
{
	RxLite::Cmd cmd(L"MyConsole");

	wchar_t buffer[1];

	cmd.Read(buffer, 1);
	cmd.ClearInput();
	cmd.Write(buffer, 1);
	cmd.Write(L"\n", 1);

	cmd.Read(buffer, 1);
	cmd.ClearInput();
	cmd.Write(buffer, 2);
	cmd.Write(L"\n", 1);

	cmd.PutFormat(L"%s-%s\n", L"txt", L"exe");

	cmd.Wait();
	return 0;
}