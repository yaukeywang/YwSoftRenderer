// Add by Yaukey at 2018-10-22.
// YW Soft Renderer application framework for input module implementation.

#ifndef __YW_BASE_INPUT_H__

#include <stdint.h>
#include "Yw3d.h"

namespace yw
{
    class IInput
    {
        friend class IApplication;
        friend class Application;

    protected:
        // The base constructor.
        IInput(class IApplication* application)
        {
            m_Application = application;
        }

        // Destructor.
        virtual ~IInput()
        {
            m_Application = nullptr;
        }

    protected:
        // Initialize the input system.
        // @return true if successful, false if not.
        virtual bool Initialize() = 0;

    public:
        // Get parent application.
        // @return the application instance.
        inline class IApplication* GetApplication() const
        {
            return m_Application;
        }

        // Update logic.
        virtual void Update() = 0;

        // Get if key down.
        // @return true if key down, false is not.
        virtual bool KeyDown(char keyCode) = 0;

        // Get if key up.
        // @return true if key up, false is not.
        virtual bool KeyUp(char keyCode) = 0;

        // Get if mouse down.
        // @return true if mouse button down, false is not.
        virtual bool MouseButtonDown(uint32_t keyCode) = 0;

        // Get if mouse up.
        // @return true if mouse button up, false is not.
        virtual bool MouseButtonUp(uint32_t keyCode) = 0;

        // Get mouse movement.
        virtual void GetMouseMovement(int32_t* deltaX, int32_t* deltaY) const = 0;

        // Get mouse wheel movement.
        // @return mouse delta movement, maybe negative.
        virtual int32_t GetMouseWheelMovement() const = 0;

        // Get mouse current position in window coordinate.
        // @param[in,out] posX current position x in window.
        // @param[in,out] posY current position y in window.
        virtual void GetMousePosition(int32_t* posX, int32_t* posY) const  = 0;

    protected:
        // The parent application class.
        class IApplication* m_Application;
    };
}

#endif // !__YW_BASE_INPUT_H__
