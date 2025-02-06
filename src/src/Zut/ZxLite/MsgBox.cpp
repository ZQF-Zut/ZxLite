#include "MsgBox.h"
#include <phnt.h>


namespace ZQF::Zut::ZxLite
{
    auto MsgBox::ShowInfo(const ZxLite::WStrView wsTitle, const ZxLite::WStrView wsText) -> MsgBox::Respon
    {
        UNICODE_STRING title_unicode_str{ wsTitle.GetRtlStr() };
        UNICODE_STRING text_unicode_str{ wsText.GetRtlStr() };

        ResponParameters parameters
        {
            &text_unicode_str, &title_unicode_str,
            (ULONG)MsgBox::Button::OK | (ULONG)MsgBox::Options::TopMost | (ULONG)MsgBox::Icon::Information,
            INFINITE
        };

        ULONG respond;
        ::NtRaiseHardError(STATUS_SERVICE_NOTIFICATION, ResponParameters::PARAM_COUNT, 0x3, (PULONG_PTR)&parameters, (ULONG)Type::OK, &respond);

        return static_cast<MsgBox::Respon>(respond);
    }
}