// Add by Yaukey at 2018-07-10.
// YW Soft Renderer 3d device state control class.

#ifndef __YW_STATE_BLOCK_H__
#define  __YW_STATE_BLOCK_H__

#include <map>
#include "Yw3d.h"

namespace yw
{
    // The state block class.
    class StateBlock
    {
    public:
        StateBlock();
        ~StateBlock();

    private:
        std::map<Yw3dRenderState, uint32_t> m_RenderStates;
    };
}

#endif // !__YW_STATE_BLOCK_H__
