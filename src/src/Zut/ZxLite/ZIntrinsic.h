#pragma once

#pragma warning(disable:4273)

extern "C" size_t strlen(const char* s);
#pragma intrinsic(strlen)
extern "C" size_t wcslen(const wchar_t* s);
#pragma intrinsic(wcslen)
extern "C" void* memset(void* dest, int ch, size_t count);
#pragma intrinsic(memset)
extern "C" void* memcpy(void* dest, const void* src, size_t count);
#pragma intrinsic(memcpy)