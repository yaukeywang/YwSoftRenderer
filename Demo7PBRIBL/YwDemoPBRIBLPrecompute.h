// Add by Yaukey at 2022-05-14.
// All IBL precompute utilities.

#ifndef __YW_DEMO_PBR_IBL_PRECOMPUTE_H__
#define __YW_DEMO_PBR_IBL_PRECOMPUTE_H__

#include "YwGraphics.h"

namespace yw
{
    class Model;
    
    // All ibl precompute utilities.
    class DemoPBRIBLPrecompute
    {
    public:
        /*
         * Convert hdr equirectangular map to hdr cube map.
         */
        static bool RenderEquirectangularMapToCubeMap(Graphics* graphics, Yw3dTexture* equirectangularTexture, Yw3dCubeTexture** envCubeTexture, Model* modelSphere);

        /*
         * Convert hdr cube map to hdr irrandiance map.
         */
        static bool RenderCubeMapToIrradianceMap(Graphics* graphics, Yw3dCubeTexture* envCubeTexture, Yw3dCubeTexture** irrandianceCubeTexture, Model* modelSphere);

        /**
         * Render prefilter hdr reflection map.
         */
        static bool RenderPrefilterReflectionMap(Graphics* graphics, Yw3dCubeTexture* envCubeTexture, Yw3dCubeTexture** prefilterReflectionCubeTexture, Model* modelSphere);

        /*
         * Render preintegrate brdf lut map.
         */
        static bool RenderPreintegrateBRDFMap(Graphics* graphics, Yw3dTexture** preintegrateBRDFTexture);
    };
}

#endif //!__YW_DEMO_PBR_IBL_PRECOMPUTE_H__
