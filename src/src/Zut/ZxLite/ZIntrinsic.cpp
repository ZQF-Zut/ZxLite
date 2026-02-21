#include <cstddef>

#pragma function(wcslen)
extern "C" size_t wcslen(const wchar_t* s)
{
  size_t n;
  for (n = 0; *s != L'\0'; ++s) { ++n; }
  return n;
}

#pragma function(strlen)
extern "C" size_t strlen(const char* s)
{
  size_t n;
  for (n = 0; *s != '\0'; ++s) { ++n; }
  return n;
}

#pragma function(memset)
extern "C" void* __cdecl memset(void* dest, int ch, size_t count)
{
  if (count)
  {
    char* d = (char*)dest;

    do { *d++ = static_cast<char>(ch); } while (--count);
  }

  return dest;
}

#pragma function(memcpy)
extern "C" void* __cdecl memcpy(void* dest, const void* src, size_t count)
{
  char* d = (char*)dest;
  char* s = (char*)src;

  while (count--) { *d++ = *s++; }

  return dest;
}
