#include "MsgBox.h"
#include <phnt.h>


enum class HardErrorResponse {
    ResponseReturnToCaller,
    ResponseNotHandled,
    ResponseAbort, ResponseCancel,
    ResponseIgnore,
    ResponseNo,
    ResponseOk,
    ResponseRetry,
    ResponseYes
};

enum class HardErrorResponseType
{
    ResponseTypeAbortRetryIgnore,
    ResponseTypeOK,
    ResponseTypeOKCancel,
    ResponseTypeRetryCancel,
    ResponseTypeYesNo,
    ResponseTypeYesNoCancel,
    ResponseTypeShutdownSystem,
    ResponseTypeTrayNotify,
    ResponseTypeCancelTryAgainContinue
};

enum class HardErrorResponseButton
{
    ResponseButtonOK,
    ResponseButtonOKCancel,
    ResponseButtonAbortRetryIgnore,
    ResponseButtonYesNoCancel,
    ResponseButtonYesNo,
    ResponseButtonRetryCancel,
    ResponseButtonCancelTryAgainContinue
};

enum class HardErrorResponseDefaultButton 
{
    DefaultButton1 = 0,
    DefaultButton2 = 0x100,
    DefaultButton3 = 0x200
};

enum class HardErrorResponseIcon 
{
    IconAsterisk = 0x40,
    IconError = 0x10,
    IconExclamation = 0x30,
    IconHand = 0x10,
    IconInformation = 0x40,
    IconNone = 0,
    IconQuestion = 0x20,
    IconStop = 0x10,
    IconWarning = 0x30,
    IconUserIcon = 0x80
};

enum class HardErrorResponseOptions 
{
    ResponseOptionNone = 0,
    ResponseOptionDefaultDesktopOnly = 0x20000,
    ResponseOptionHelp = 0x4000,
    ResponseOptionRightAlign = 0x80000,
    ResponseOptionRightToLeftReading = 0x100000,
    ResponseOptionTopMost = 0x40000,
    ResponseOptionServiceNotification = 0x00200000,
    ResponseOptionServiceNotificationNT3X = 0x00040000,
    ResponseOptionSetForeground = 0x10000,
    ResponseOptionSystemModal = 0x1000,
    ResponseOptionTaskModal = 0x2000,
    ResponseOptionNoFocus = 0x00008000
};

struct HardErrorResponseParameters
{
    PUNICODE_STRING pTitle;
    PUNICODE_STRING pText;
    ULONG eStyle;
    ULONG nTimeout;

    static constexpr auto PARAM_COUNT = 4;
};

namespace ZQF::Zut::ZxLite
{
    auto MsgBox(const ZxLite::WStrView wsTitle, const ZxLite::WStrView wsText) -> void
    {
        UNICODE_STRING title_unicode_str{ wsTitle.GetRtlStr() };
        UNICODE_STRING text_unicode_str{ wsText.GetRtlStr() };

        HardErrorResponseParameters parameters
        {
            &text_unicode_str,
            &title_unicode_str,
            (ULONG)HardErrorResponseButton::ResponseButtonOK,
            INFINITE
        };

        ULONG respond; // HardErrorResponse
        ::NtRaiseHardError
        (
            STATUS_SERVICE_NOTIFICATION, 
            HardErrorResponseParameters::PARAM_COUNT, 
            0x3, 
            (PULONG_PTR)&parameters, 
            (ULONG)HardErrorResponseType::ResponseTypeOK, 
            &respond
        );
    }
}