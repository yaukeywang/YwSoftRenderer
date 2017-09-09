// Add by yaukey at 2017-08-27.
// The global manager class.

#ifndef __GLOBAL_MANAGER_H__
#define __GLOBAL_MANAGER_H__

namespace yw
{
    class AppBase;
    class ILogger;
    class InputManager;

    class GlobalManager
    {
    private:
        GlobalManager();

    public:
        virtual ~GlobalManager();

    public:
        bool Initialize(AppBase* app);
        void Release();

        // Get modules.
        ILogger* GetLogger() { return m_Logger; }
        InputManager* GetInputManager() { return m_InputManager; }

    public:
        static GlobalManager* GetGlobalManager();
        static void ReleaseGlobalManager();

    private:
        static GlobalManager* s_GlobalManager;

    private:
        ILogger* m_Logger;
        InputManager* m_InputManager;
    };
}

#endif // __GLOBAL_MANAGER_H__
