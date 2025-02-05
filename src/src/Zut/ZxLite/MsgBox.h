#pragma once
#include <Zut/ZxLite/StrView.h>


namespace ZQF::Zut::ZxLite
{
	auto MsgBox(const ZxLite::WStrView wsTitle, const ZxLite::WStrView wsText) -> void;
}