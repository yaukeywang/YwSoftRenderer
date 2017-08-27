// Add by yaukey at 2017-08-27.
// The global manager class.

#ifndef __GLOBAL_MANAGER_H__
#define __GLOBAL_MANAGER_H__

namespace yw
{
    class ILogger;

    class GlobalMgr
    {
    private:
        GlobalMgr();

    public:
        virtual ~GlobalMgr();

    public:
        void Initialize();
        void Release();
        ILogger* GetLogger();

    public:
        static GlobalMgr* GetGlobalMgr();

    private:
        static GlobalMgr* s_GlobalMgr;

    private:
        ILogger* m_Logger;
    };
}

#endif // __GLOBAL_MANAGER_H__
