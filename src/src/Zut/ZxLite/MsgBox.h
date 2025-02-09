#pragma once
#include <Zut/ZxLite/String.h>


namespace ZQF::Zut::ZxLite
{
	class MsgBox
	{
	public:
        enum class Respon 
        {
            ReturnToCaller,
            NotHandled,
            Abort, 
            ResponseCancel,
            Ignore,
            No,
            Ok,
            Retry,
            Yes
        };

        enum class Type
        {
            AbortRetryIgnore,
            OK,
            OKCancel,
            RetryCancel,
            YesNo,
            YesNoCancel,
            ShutdownSystem,
            TrayNotify,
            CancelTryAgainContinue
        };

        enum class Button
        {
            OK,
            OKCancel,
            AbortRetryIgnore,
            YesNoCancel,
            YesNo,
            RetryCancel,
            CancelTryAgainContinue
        };

        enum class DefaultButton
        {
            Default1 = 0,
            Default2 = 0x100,
            Default3 = 0x200
        };

        enum class Icon
        {
            Asterisk = 0x40,
            Error = 0x10,
            Exclamation = 0x30,
            Hand = 0x10,
            Information = 0x40,
            None = 0,
            Question = 0x20,
            Stop = 0x10,
            Warning = 0x30,
            UserIcon = 0x80
        };

        enum class Options
        {
            None = 0,
            DefaultDesktopOnly = 0x20000,
            Help = 0x4000,
            RightAlign = 0x80000,
            RightToLeftReading = 0x100000,
            TopMost = 0x40000,
            ServiceNotification = 0x00200000,
            ServiceNotificationNT3X = 0x00040000,
            SetForeground = 0x10000,
            SystemModal = 0x1000,
            TaskModal = 0x2000,
            NoFocus = 0x00008000
        };

        struct ResponParameters
        {
            PUNICODE_STRING pTitle;
            PUNICODE_STRING pText;
            ULONG eStyle;
            ULONG nTimeout;

            static constexpr auto PARAM_COUNT = 4;
        };

    private:
        void* m_fnNtRaiseHardError{};

    public:
        MsgBox();

	public:
		auto ShowInfo(const ZxLite::WStrView wsTitle, const ZxLite::WStrView wsText) -> MsgBox::Respon;

	};
}