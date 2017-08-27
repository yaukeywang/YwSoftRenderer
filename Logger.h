// Add by yaukey at 2017-08-27.
// Logger class.

#include "Base.h"

namespace yw
{
    // Base logger define.
    class ILogger
    {
    public:
        ILogger() {}
        virtual ~ILogger() {}

    public:
        virtual void Log(LPCTSTR message) = 0;
        virtual void LogFormat(LPCTSTR format, ...) = 0;

        virtual void LogInformation(LPCTSTR message) = 0;
        virtual void LogFormatInformation(LPCTSTR format, ...) = 0;

        virtual void LogWarning(LPCTSTR message) = 0;
        virtual void LogWarningFormat(LPCTSTR format, ...) = 0;

        virtual void LogError(LPCTSTR message) = 0;
        virtual void LogErrorFormat(LPCTSTR format, ...) = 0;
    };

    // Logger class.
    class Logger
    {
    public:
        Logger();
        virtual ~Logger();

    public:
        virtual void Log(LPCTSTR message);
        virtual void LogFormat(LPCTSTR format, ...);

        virtual void LogInformation(LPCTSTR message);
        virtual void LogFormatInformation(LPCTSTR format, ...);

        virtual void LogWarning(LPCTSTR message);
        virtual void LogWarningFormat(LPCTSTR format, ...);

        virtual void LogError(LPCTSTR message);
        virtual void LogErrorFormat(LPCTSTR format, ...);

    private:
        void OutputLog(int32_t level, LPCTSTR message);

    private:
        // The console window.
        HANDLE m_ConsoleWnd;
    };
}
