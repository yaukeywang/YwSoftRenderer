// Add by yaukey at 2017-08-27.
// The global manager class.

#ifndef __GLOBAL_MANAGER_H__
#define __GLOBAL_MANAGER_H__

namespace yw
{
    class ILogger;

    class GlobalManager
    {
    private:
        GlobalManager();

    public:
        virtual ~GlobalManager();

    public:
        void Initialize();
        void Release();
        ILogger* GetLogger();

    public:
        static GlobalManager* GetGlobalManager();

    private:
        static GlobalManager* s_GlobalManager;

    private:
        ILogger* m_Logger;
    };
}

#endif // __GLOBAL_MANAGER_H__
