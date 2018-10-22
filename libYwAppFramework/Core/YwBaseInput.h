// Add by Yaukey at 2018-10-22.
// YW Soft Renderer application framework for input module implementation.

#ifndef __YW_BASE_INPUT_H__

#include <stdint.h>

namespace yw
{
    class IInput
    {
        friend class IApplication;
        friend class Application;

    protected:
        IInput(class IApplication* application)
        {
            m_Application = application;
        }

        // Destructor.
        ~IInput()
        {
            m_Application = nullptr;
        }

    public:
        // Get parent application.
        inline class IApplication* GetApplication() const
        {
            return m_Application;
        }

        // Get if key down.
        virtual bool KeyDown(uint32_t keyCode) = 0;

        // Get if key up.
        virtual bool KeyUp(uint32_t keyCode) = 0;

        // Get if mouse down.
        virtual bool MouseButtonDown(uint32_t keyCode, int32_t xPos, int32_t yPos) = 0;

        // Get if mouse up.
        virtual bool MouseButtonUp(uint32_t keyCode, int32_t xPos, int32_t yPos) = 0;

        // Get mouse movement.
        virtual void GetMouseMovement(int32_t& deltaX, int32_t& deltaY) = 0;

        // Get mouse wheel movement.
        virtual int GetMouseWheelMovement() = 0;

    protected:
        // The parent application class.
        class IApplication* m_Application;
    };
}

#endif // !__YW_BASE_INPUT_H__
