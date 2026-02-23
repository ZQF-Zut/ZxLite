#pragma once
#include <Zut/ZxLite/String.h>

namespace ZQF::Zut::ZxLite
{
  class Console
  {
  private:
    void* m_fnNtWriteFile{};

  public:
    Console();

  public:
    auto Put(const void* pData, const std::size_t nBytes) -> void;

    template <typename T>
    auto Put(const BaseStrView<T> tStr) -> void
    {
      this->Put(tStr.CStr(), tStr.SizeBytes());
    }
  };

} // namespace ZQF::Zut::ZxLite
