// Add by yaukey at 2017-09-04.
// Input device base class.

#ifndef __INPUT_DEVICE_H__
#define __INPUT_DEVICE_H__

#include "System/Support/Base.h"
#include <dinput.h>
#include <dinputd.h>

namespace yw
{
    class InputManager;

    // Input device base calss.
    class InputDevice
    {
    public:
        InputDevice(InputManager* inputManager, LPCTSTR deviceName, const GUID& deviceGUID);
        virtual ~InputDevice();

    public:
        bool CreateInputDevice(const DIDATAFORMAT* dataFormat, DWORD cooperativeLevel, DWORD bufferSize);
        bool Acquire();
        bool Unacquire();

        InputManager* GetInputManager() const { return m_InputManager; }
        LPCTSTR GetDeviceName() const { return m_DeviceName; }
        int32_t GetDeviceType() const { return m_DeviceType; }
        LPDIRECTINPUTDEVICE8 GetDevice() const { return m_Device; }

        virtual bool Initialize() = 0;
        virtual void Release() = 0;
        virtual void Update() = 0;

    protected:
        // The input manager belongs to.
        InputManager* m_InputManager;

        // The device name.
        LPCTSTR m_DeviceName;

        // The input device.
        LPDIRECTINPUTDEVICE8 m_Device;

        // Device cooperative level.
        DWORD m_CooperativeLevel;

        // Device type.
        int32_t m_DeviceType;

        // Device guid.
        const GUID& m_GUID;

        // Device data format.
        const DIDATAFORMAT* m_DataFormat;
    };
}

#endif // !__INPUT_DEVICE_H__
