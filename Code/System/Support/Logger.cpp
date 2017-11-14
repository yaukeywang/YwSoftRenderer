// Add by yaukey at 2017-08-27.
// Logger class.

#include "stdafx.h"
#include "Logger.h"

namespace yw
{
    Logger::Logger()
    {
        // Alloc console window for this process.
        AllocConsole();
        freopen("CONOUT$", "w+t", stdout);
        freopen("CONIN$", "r+t", stdin);

        // Get console window handler.
        m_ConsoleWnd = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    Logger::~Logger()
    {
        // Free console window for this process.
        fclose(stdout);
        fclose(stdin);
        FreeConsole();

        // Clear console window handle.
        m_ConsoleWnd = nullptr;
    }

    void Logger::Log(LPCTSTR message)
    {
        OutputLog(0, message);
    }

    void Logger::LogFormat(LPCTSTR format, ...)
    {
        va_list args;
        va_start(args, format);

        TCHAR buffer[1024];
#ifdef UNICODE
        vswprintf(buffer, format, args);
#else
        vsprintf(buffer, format, args);
#endif

        va_end(args);

        // Output result.
        OutputLog(0, buffer);
    }

    void Logger::LogInformation(LPCTSTR message)
    {
        OutputLog(1, message);
    }

    void Logger::LogInformationFormat(LPCTSTR format, ...)
    {
        va_list args;
        va_start(args, format);

        TCHAR buffer[1024];
#ifdef UNICODE
        vswprintf(buffer, format, args);
#else
        vsprintf(buffer, format, args);
#endif

        va_end(args);

        // Output result.
        OutputLog(1, buffer);
    }

    void Logger::LogWarning(LPCTSTR message)
    {
        OutputLog(2, message);
    }

    void Logger::LogWarningFormat(LPCTSTR format, ...)
    {
        va_list args;
        va_start(args, format);

        TCHAR buffer[1024];
#ifdef UNICODE
        vswprintf(buffer, format, args);
#else
        vsprintf(buffer, format, args);
#endif

        va_end(args);

        // Output result.
        OutputLog(2, buffer);
    }

    void Logger::LogError(LPCTSTR message)
    {
        OutputLog(3, message);
    }

    void Logger::LogErrorFormat(LPCTSTR format, ...)
    {
        va_list args;
        va_start(args, format);

        TCHAR buffer[1024];
#ifdef UNICODE
        vswprintf(buffer, format, args);
#else
        vsprintf(buffer, format, args);
#endif

        va_end(args);

        // Output result.
        OutputLog(3, buffer);
    }

    void Logger::OutputLog(int32_t level, LPCTSTR message)
    {
        SYSTEMTIME time;
        GetSystemTime(&time);

        TCHAR buffer[1024];
        switch (level)
        {
        case 0:
            SetConsoleTextAttribute(m_ConsoleWnd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
            break;
        case 1:
            SetConsoleTextAttribute(m_ConsoleWnd, FOREGROUND_BLUE | FOREGROUND_GREEN);
            break;
        case 2:
            SetConsoleTextAttribute(m_ConsoleWnd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
            break;
        case 3:
            SetConsoleTextAttribute(m_ConsoleWnd, FOREGROUND_RED | FOREGROUND_INTENSITY);
            break;
        default:
            SetConsoleTextAttribute(m_ConsoleWnd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
            break;
        }

#ifdef UNICODE
        swprintf(buffer, _T("[%04d-%02d-%02d %02d:%02d] %s \r\n"), time.wYear, time.wMonth, time.wDay, (time.wHour + 8) % 24, time.wMinute, message);
        std::wcout << buffer;
#else
        sprintf(buffer, _T("[%04d-%02d-%02d %02d:%02d] %s \r\n"), time.wYear, time.wMonth, time.wDay, (time.wHour + 8) % 24, time.wMinute, message);
        std::cout << buffer;
#endif
    }
}
