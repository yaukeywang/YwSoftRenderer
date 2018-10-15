// Add by yaukey at 2017-08-30.
// All input management.

#ifndef __YW_INPUT_MANAGER_H__
#define __YW_INPUT_MANAGER_H__

#include "System/Support/YwBase.h"
#include <dinput.h>
#include <dinputd.h>

namespace yw
{
    class InputDevice;
    typedef std::vector<InputDevice*> InputDeviceList;

    enum InputKeyId
    {
        IPKI_INVALID = -1,

        //
        IPKI_KEY_DEFINE_BEGIN = DIK_ESCAPE,

        IPKI_KEY_ESCAPE = DIK_ESCAPE,
        IPKI_KEY_1 = DIK_1,
        IPKI_KEY_2 = DIK_2,
        IPKI_KEY_3 = DIK_3,
        IPKI_KEY_4 = DIK_4,
        IPKI_KEY_5 = DIK_5,
        IPKI_KEY_6 = DIK_6,
        IPKI_KEY_7 = DIK_7,
        IPKI_KEY_8 = DIK_8,
        IPKI_KEY_9 = DIK_9,
        IPKI_KEY_0 = DIK_0,
        IPKI_KEY_MINUS = DIK_MINUS,        /* - on main keyboard */
        IPKI_KEY_EQUALS = DIK_EQUALS,
        IPKI_KEY_BACK = DIK_BACK,        /* backspace */
        IPKI_KEY_TAB = DIK_TAB,
        IPKI_KEY_Q = DIK_Q,
        IPKI_KEY_W = DIK_W,
        IPKI_KEY_E = DIK_E,
        IPKI_KEY_R = DIK_R,
        IPKI_KEY_T = DIK_T,
        IPKI_KEY_Y = DIK_Y,
        IPKI_KEY_U = DIK_U,
        IPKI_KEY_I = DIK_I,
        IPKI_KEY_O = DIK_O,
        IPKI_KEY_P = DIK_P,
        IPKI_KEY_LBRACKET = DIK_LBRACKET,
        IPKI_KEY_RBRACKET = DIK_RBRACKET,
        IPKI_KEY_RETURN = DIK_RETURN,        /* Enter on main keyboard */
        IPKI_KEY_LCONTROL = DIK_LCONTROL,
        IPKI_KEY_A = DIK_A,
        IPKI_KEY_S = DIK_S,
        IPKI_KEY_D = DIK_D,
        IPKI_KEY_F = DIK_F,
        IPKI_KEY_G = DIK_G,
        IPKI_KEY_H = DIK_H,
        IPKI_KEY_J = DIK_J,
        IPKI_KEY_K = DIK_K,
        IPKI_KEY_L = DIK_L,
        IPKI_KEY_SEMICOLON = DIK_SEMICOLON,
        IPKI_KEY_APOSTROPHE = DIK_APOSTROPHE,
        IPKI_KEY_GRAVE = DIK_GRAVE,        /* accent grave */
        IPKI_KEY_LSHIFT = DIK_LSHIFT,
        IPKI_KEY_BACKSLASH = DIK_BACKSLASH,
        IPKI_KEY_Z = DIK_Z,
        IPKI_KEY_X = DIK_X,
        IPKI_KEY_C = DIK_C,
        IPKI_KEY_V = DIK_V,
        IPKI_KEY_B = DIK_B,
        IPKI_KEY_N = DIK_N,
        IPKI_KEY_M = DIK_M,
        IPKI_KEY_COMMA = DIK_COMMA,
        IPKI_KEY_PERIOD = DIK_PERIOD,        /* . on main keyboard */
        IPKI_KEY_SLASH = DIK_SLASH,        /* / on main keyboard */
        IPKI_KEY_RSHIFT = DIK_RSHIFT,
        IPKI_KEY_MULTIPLY = DIK_MULTIPLY,        /* * on numeric keypad */
        IPKI_KEY_LMENU = DIK_LMENU,        /* left Alt */
        IPKI_KEY_SPACE = DIK_SPACE,
        IPKI_KEY_CAPITAL = DIK_CAPITAL,
        IPKI_KEY_F1 = DIK_F1,
        IPKI_KEY_F2 = DIK_F2,
        IPKI_KEY_F3 = DIK_F3,
        IPKI_KEY_F4 = DIK_F4,
        IPKI_KEY_F5 = DIK_F5,
        IPKI_KEY_F6 = DIK_F6,
        IPKI_KEY_F7 = DIK_F7,
        IPKI_KEY_F8 = DIK_F8,
        IPKI_KEY_F9 = DIK_F9,
        IPKI_KEY_F10 = DIK_F10,
        IPKI_KEY_NUMLOCK = DIK_NUMLOCK,
        IPKI_KEY_SCROLL = DIK_SCROLL,        /* Scroll Lock */
        IPKI_KEY_NUMPAD7 = DIK_NUMPAD7,
        IPKI_KEY_NUMPAD8 = DIK_NUMPAD8,
        IPKI_KEY_NUMPAD9 = DIK_NUMPAD9,
        IPKI_KEY_SUBTRACT = DIK_SUBTRACT,        /* - on numeric keypad */
        IPKI_KEY_NUMPAD4 = DIK_NUMPAD4,
        IPKI_KEY_NUMPAD5 = DIK_NUMPAD5,
        IPKI_KEY_NUMPAD6 = DIK_NUMPAD6,
        IPKI_KEY_ADD = DIK_ADD,        /* + on numeric keypad */
        IPKI_KEY_NUMPAD1 = DIK_NUMPAD1,
        IPKI_KEY_NUMPAD2 = DIK_NUMPAD2,
        IPKI_KEY_NUMPAD3 = DIK_NUMPAD3,
        IPKI_KEY_NUMPAD0 = DIK_NUMPAD0,
        IPKI_KEY_DECIMAL = DIK_DECIMAL,        /* . on numeric keypad */
        IPKI_KEY_OEM_102 = DIK_OEM_102,        /* <> or \| on RT 102-key keyboard (Non-U.S.) */
        IPKI_KEY_F11 = DIK_F11,
        IPKI_KEY_F12 = DIK_F12,
        IPKI_KEY_F13 = DIK_F13,        /*                     (NEC PC98) */
        IPKI_KEY_F14 = DIK_F14,        /*                     (NEC PC98) */
        IPKI_KEY_F15 = DIK_F15,        /*                     (NEC PC98) */
        IPKI_KEY_KANA = DIK_KANA,        /* (Japanese keyboard)            */
        IPKI_KEY_ABNT_C1 = DIK_ABNT_C1,        /* /? on Brazilian keyboard */
        IPKI_KEY_CONVERT = DIK_CONVERT,        /* (Japanese keyboard)            */
        IPKI_KEY_NOCONVERT = DIK_NOCONVERT,        /* (Japanese keyboard)            */
        IPKI_KEY_YEN = DIK_YEN,        /* (Japanese keyboard)            */
        IPKI_KEY_ABNT_C2 = DIK_ABNT_C2,        /* Numpad . on Brazilian keyboard */
        IPKI_KEY_NUMPADEQUALS = DIK_NUMPADEQUALS,        /* = on numeric keypad (NEC PC98) */
        IPKI_KEY_PREVTRACK = DIK_PREVTRACK,        /* Previous Track  IPKI_KEY_CIRCUMFLEX o(= DIK_CIRCUMFLEX o,n Japanese keyboard) */
        IPKI_KEY_AT = DIK_AT,        /*                     (NEC PC98) */
        IPKI_KEY_COLON = DIK_COLON,        /*                     (NEC PC98) */
        IPKI_KEY_UNDERLINE = DIK_UNDERLINE,        /*                     (NEC PC98) */
        IPKI_KEY_KANJI = DIK_KANJI,        /* (Japanese keyboard)            */
        IPKI_KEY_STOP = DIK_STOP,        /*                     (NEC PC98) */
        IPKI_KEY_AX = DIK_AX,        /*                     (Japan AX) */
        IPKI_KEY_UNLABELED = DIK_UNLABELED,        /*                        (J3100) */
        IPKI_KEY_NEXTTRACK = DIK_NEXTTRACK,        /* Next Track */
        IPKI_KEY_NUMPADENTER = DIK_NUMPADENTER,        /* Enter on numeric keypad */
        IPKI_KEY_RCONTROL = DIK_RCONTROL,
        IPKI_KEY_MUTE = DIK_MUTE,        /* Mute */
        IPKI_KEY_CALCULATOR = DIK_CALCULATOR,        /* Calculator */
        IPKI_KEY_PLAYPAUSE = DIK_PLAYPAUSE,        /* Play / Pause */
        IPKI_KEY_MEDIASTOP = DIK_MEDIASTOP,        /* Media Stop */
        IPKI_KEY_VOLUMEDOWN = DIK_VOLUMEDOWN,        /* Volume - */
        IPKI_KEY_VOLUMEUP = DIK_VOLUMEUP,        /* Volume + */
        IPKI_KEY_WEBHOME = DIK_WEBHOME,        /* Web home */
        IPKI_KEY_NUMPADCOMMA = DIK_NUMPADCOMMA,        /* , on numeric keypad (NEC PC98) */
        IPKI_KEY_DIVIDE = DIK_DIVIDE,        /* / on numeric keypad */
        IPKI_KEY_SYSRQ = DIK_SYSRQ,
        IPKI_KEY_RMENU = DIK_RMENU,        /* right Alt */
        IPKI_KEY_PAUSE = DIK_PAUSE,        /* Pause */
        IPKI_KEY_HOME = DIK_HOME,        /* Home on arrow keypad */
        IPKI_KEY_UP = DIK_UP,        /* UpArrow on arrow keypad */
        IPKI_KEY_PRIOR = DIK_PRIOR,        /* PgUp on arrow keypad */
        IPKI_KEY_LEFT = DIK_LEFT,        /* LeftArrow on arrow keypad */
        IPKI_KEY_RIGHT = DIK_RIGHT,        /* RightArrow on arrow keypad */
        IPKI_KEY_END = DIK_END,        /* End on arrow keypad */
        IPKI_KEY_DOWN = DIK_DOWN,        /* DownArrow on arrow keypad */
        IPKI_KEY_NEXT = DIK_NEXT,        /* PgDn on arrow keypad */
        IPKI_KEY_INSERT = DIK_INSERT,        /* Insert on arrow keypad */
        IPKI_KEY_DELETE = DIK_DELETE,        /* Delete on arrow keypad */
        IPKI_KEY_LWIN = DIK_LWIN,        /* Left Windows key */
        IPKI_KEY_RWIN = DIK_RWIN,        /* Right Windows key */
        IPKI_KEY_APPS = DIK_APPS,        /* AppMenu key */
        IPKI_KEY_POWER = DIK_POWER,        /* System Power */
        IPKI_KEY_SLEEP = DIK_SLEEP,        /* System Sleep */
        IPKI_KEY_WAKE = DIK_WAKE,        /* System Wake */
        IPKI_KEY_WEBSEARCH = DIK_WEBSEARCH,        /* Web Search */
        IPKI_KEY_WEBFAVORITES = DIK_WEBFAVORITES,        /* Web Favorites */
        IPKI_KEY_WEBREFRESH = DIK_WEBREFRESH,        /* Web Refresh */
        IPKI_KEY_WEBSTOP = DIK_WEBSTOP,        /* Web Stop */
        IPKI_KEY_WEBFORWARD = DIK_WEBFORWARD,        /* Web Forward */
        IPKI_KEY_WEBBACK = DIK_WEBBACK,        /* Web Back */
        IPKI_KEY_MYCOMPUTER = DIK_MYCOMPUTER,        /* My Computer */
        IPKI_KEY_MAIL = DIK_MAIL,        /* Mail */
        IPKI_KEY_MEDIASELECT = DIK_MEDIASELECT,        /* Media Select */

        IPKI_KEY_DEFINE_END = DIK_MEDIASELECT,
        //

        //
        IPKI_MOUSE_DEFINE_BEGIN,

        IPKI_MOUSE_LEFT,
        IPKI_MOUSE_RIGHT,
        IPKI_MOUSE_WHEEL,

        IPKI_MOUSE_3,
        IPKI_MOUSE_4,
        IPKI_MOUSE_5,
        IPKI_MOUSE_6,
        IPKI_MOUSE_7,

        IPKI_MOUSE_MOVE,
        IPKI_MOUSE_MOVE_WHEEL,

        IPKI_MOUSE_DEFINE_END,
        //

        IPKI_END
    };

    enum InputKeyState
    {
        IPKS_INVALID = -1,

        IPKS_PRESSED,
        IPKS_RELEASED,

        IPKS_END
    };

    struct InputKey
    {
        InputKeyId m_Id;
        InputKeyState m_State;
        int32_t m_X;
        int32_t m_Y;
        int32_t m_Z;

        InputKey(InputKeyId inputKeyId) : m_Id(inputKeyId), m_State(IPKS_INVALID), m_X(0), m_Y(0), m_Z(0) {}
    };

    class InputManager
    {
    public:
        InputManager();
        virtual ~InputManager();

    public:
        bool Initialize(HWND mainWindow);
        void Release();
        void Update();

        HWND GetMainWindowHandle() const { return m_MainWindow; }
        LPDIRECTINPUT8 GetDirectInput() const { return m_DirectInput; }

        void AddInputDevice(InputDevice* inputDevice);
        void OnInputKey(const InputKey* inputKey);

    private:
        // The handle of the main window.
        HWND m_MainWindow;

        // The direct input device.
        LPDIRECTINPUT8 m_DirectInput;

        // The input device list.
        InputDeviceList m_InputDevices;
    };
}

#endif // !__YW_INPUT_MANAGER_H__
