#pragma once
#include <Zut/ZxLite/String.h>

namespace ZQF::Zut::ZxLite
{
  class OpenModule
  {
  private:
    PLDR_DATA_TABLE_ENTRY m_pLDRDataEntry;

  public:
    OpenModule(const ZxLite::WStrView wsModuleName);
    OpenModule(const std::size_t nModuleNameHashUpperCase);

  private:
    static auto FindLDRDataEntry(const ZxLite::WStrView wsModuleName) -> PLDR_DATA_TABLE_ENTRY;
    static auto FindLDRDataEntry(const std::size_t nModuleNameHashUpperCase) -> PLDR_DATA_TABLE_ENTRY;

  public:
    auto ImageBase() const -> void*;
    auto ModuleName() const -> ZxLite::WStrView;
    auto ModuleFullPath() const -> ZxLite::WStrView;
    auto GetProcedureBase(const std::size_t nHash) const -> void*;
    auto GetProcedure(const ZxLite::StrView msFnName) const -> void*;
    template <typename T = void*>
    auto GetProcedure(const std::size_t nHash) const -> T
    {
      return reinterpret_cast<T>(this->GetProcedureBase(nHash));
    }

  public:
    operator bool() const;
  };
} // namespace ZQF::Zut::ZxLite
