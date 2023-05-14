// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#include "YwDemoPBRIBLPrecompute.h"
#include "YwDemoPBRIBLShaderPrecompute.h"
#include "YwModel.h"

namespace yw
{
    bool DemoPBRIBLPrecompute::RenderEquirectangularMapToCubeMap(Graphics* graphics, Yw3dTexture* equirectangularTexture, Yw3dCubeTexture** envCubeTexture, Model* modelSphere)
    {
        if ((nullptr == graphics) || (nullptr == equirectangularTexture) || (nullptr == envCubeTexture) || (nullptr == modelSphere))
        {
            return false;
        }
        
        Yw3dDevice* device = graphics->GetYw3dDevice();
        if (nullptr == device)
        {
            return false;
        }

        // Render constants.
        const int32_t targetWidth = 512;
        const int32_t targetHeight = 512;
        const int32_t cubeLength = 512;
        const Yw3dFormat cubeFormat = Yw3d_FMT_R32G32B32A32F;
        const float fovy = YW_PI / 2.0f;
        const float aspect = 1.0f;
        const float ZNear = 0.1f;
        const float zFar = 10.0f;

        // Create a temp render target.
        Yw3dRenderTarget* rtCubemap = nullptr;
        if (YW3D_FAILED(device->CreateRenderTarget(&rtCubemap, targetWidth, targetHeight, cubeFormat, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
        {
            return false;
        }

        // Create cube map.
        YW_SAFE_RELEASE(*envCubeTexture);
        if (YW3D_FAILED(device->CreateCubeTexture(envCubeTexture, cubeLength, 0, cubeFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create cube texture failed."));
            return false;
        }

        // Backup old viewport matrix.sss
        const Matrix44* matViewportCurrentPointer;
        device->GetViewportMatrix(matViewportCurrentPointer);
        Matrix44 matViewportCurrent(*matViewportCurrentPointer);

        // Set device viewport.
        Matrix44 matViewportCubeMap;
        Matrix44Viewport(matViewportCubeMap, 0, 0, targetWidth, targetHeight, 0.0f, 1.0f);
        device->SetViewportMatrix(&matViewportCubeMap);

        // Construct view matrices.
        Matrix44 matViews[Yw3d_CF_NumCubeFaces];
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_X], Vector3::Zero(), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_X], Vector3::Zero(), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Y], Vector3::Zero(), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Y], Vector3::Zero(), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));

        // Construct projection matrix.
        Matrix44 matProjection;
        Matrix44PerspectiveFovLH(matProjection, fovy, aspect, ZNear, zFar);

        // Create shader.
        DemoPBRIBLEquirectangularMap2CubeMapVertexShader* equirectangularMapVertexShader = new DemoPBRIBLEquirectangularMap2CubeMapVertexShader();
        DemoPBRIBLEquirectangularMap2CubeMapPixelShader* equirectangularMapPixelShader = new DemoPBRIBLEquirectangularMap2CubeMapPixelShader();

        // Set render target.
        graphics->SetRenderTarget(rtCubemap);

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set cube texture.
        graphics->SetTexture(0, equirectangularTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(equirectangularMapVertexShader);
        graphics->SetPixelShader(equirectangularMapPixelShader);

        // Create cube map face textures.
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            // Render to target and copy to cube face.
            device->Clear(nullptr, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

            // This should be from device.
            Matrix44 matWorld;
            Matrix44 matWVP = matWorld * matViews[i] * matProjection;
            device->SetTransform(Yw3d_TS_World, &matWorld);
            device->SetTransform(Yw3d_TS_View, &matViews[i]);
            device->SetTransform(Yw3d_TS_Projection, &matProjection);
            device->SetTransform(Yw3d_TS_WVP, &matWVP);

            // Render this face.
            modelSphere->Render(device);

            // Copy pixels from render target to cube face.
            Yw3dSurface* srcSurface = rtCubemap->AcquireColorBuffer();
            Yw3dTexture* dstCubeFace = (*envCubeTexture)->AcquireCubeFace((Yw3dCubeFaces)i);
            Yw3dSurface* dstSurface = dstCubeFace->AcquireMipLevel(0);
            srcSurface->CopyToSurface(nullptr, dstSurface, nullptr, Yw3d_TF_Linear);
            YW_SAFE_RELEASE(dstSurface);
            YW_SAFE_RELEASE(dstCubeFace);
            YW_SAFE_RELEASE(srcSurface);
        }

        // Generate mip-map levels, used by prefilter generation.
        (*envCubeTexture)->GenerateMipSubLevels(0);

        // Recovery viewport.
        device->SetViewportMatrix(&matViewportCurrent);

        // Release temp resources.
        YW_SAFE_RELEASE(rtCubemap);
        YW_SAFE_RELEASE(equirectangularMapVertexShader);
        YW_SAFE_RELEASE(equirectangularMapPixelShader);

        return true;
    }

    bool DemoPBRIBLPrecompute::RenderCubeMapToIrradianceMap(Graphics* graphics, Yw3dCubeTexture* envCubeTexture, Yw3dCubeTexture** irrandianceCubeTexture, Model* modelSphere)
    {
        if ((nullptr == graphics) || (nullptr == envCubeTexture) || (nullptr == irrandianceCubeTexture) || (nullptr == modelSphere))
        {
            return false;
        }
        
        Yw3dDevice* device = graphics->GetYw3dDevice();
        if (nullptr == device)
        {
            return false;
        }

        // Render constants.
        const int32_t targetWidth = 32;
        const int32_t targetHeight = 32;
        const int32_t cubeLength = 32;
        const Yw3dFormat cubeFormat = Yw3d_FMT_R32G32B32A32F;
        const float fovy = YW_PI / 2.0f;
        const float aspect = 1.0f;
        const float ZNear = 0.1f;
        const float zFar = 10.0f;

        // Create a temp render target.
        Yw3dRenderTarget* rtCubemap = nullptr;
        if (YW3D_FAILED(device->CreateRenderTarget(&rtCubemap, targetWidth, targetHeight, cubeFormat, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
        {
            return false;
        }

        // Create cube map.
        YW_SAFE_RELEASE(*irrandianceCubeTexture);
        if (YW3D_FAILED(device->CreateCubeTexture(irrandianceCubeTexture, cubeLength, 0, cubeFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create cube texture failed."));
            return false;
        }

        // Backup old viewport matrix.
        const Matrix44* matViewportCurrentPointer;
        device->GetViewportMatrix(matViewportCurrentPointer);
        Matrix44 matViewportCurrent(*matViewportCurrentPointer);

        // Set device viewport.
        Matrix44 matViewportCubeMap;
        Matrix44Viewport(matViewportCubeMap, 0, 0, targetWidth, targetHeight, 0.0f, 1.0f);
        device->SetViewportMatrix(&matViewportCubeMap);

        // Construct view matrices.
        Matrix44 matViews[Yw3d_CF_NumCubeFaces];
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_X], Vector3::Zero(), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_X], Vector3::Zero(), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Y], Vector3::Zero(), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Y], Vector3::Zero(), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));

        // Construct projection matrix.
        Matrix44 matProjection;
        Matrix44PerspectiveFovLH(matProjection, fovy, aspect, ZNear, zFar);

        // Create shader.
        DemoPBRIBLCubeMap2IrrandianceMapVertexShader* irrandianceMapVertexShader = new DemoPBRIBLCubeMap2IrrandianceMapVertexShader();
        DemoPBRIBLCubeMap2IrrandianceMapPixelShader* irrandianceMapPixelShader = new DemoPBRIBLCubeMap2IrrandianceMapPixelShader();

        // Set render target.
        graphics->SetRenderTarget(rtCubemap);

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set cube texture.
        graphics->SetTexture(0, envCubeTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(irrandianceMapVertexShader);
        graphics->SetPixelShader(irrandianceMapPixelShader);

        // Create cube map face textures.
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            // Render to target and copy to cube face.
            device->Clear(nullptr, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

            // This should be from device.
            Matrix44 matWorld;
            Matrix44 matWVP = matWorld * matViews[i] * matProjection;
            device->SetTransform(Yw3d_TS_World, &matWorld);
            device->SetTransform(Yw3d_TS_View, &matViews[i]);
            device->SetTransform(Yw3d_TS_Projection, &matProjection);
            device->SetTransform(Yw3d_TS_WVP, &matWVP);

            // Render this face.
            modelSphere->Render(device);

            // Copy pixels from render target to cube face.
            Yw3dSurface* srcSurface = rtCubemap->AcquireColorBuffer();
            Yw3dTexture* dstCubeFace = (*irrandianceCubeTexture)->AcquireCubeFace((Yw3dCubeFaces)i);
            Yw3dSurface* dstSurface = dstCubeFace->AcquireMipLevel(0);
            srcSurface->CopyToSurface(nullptr, dstSurface, nullptr, Yw3d_TF_Linear);
            YW_SAFE_RELEASE(dstSurface);
            YW_SAFE_RELEASE(dstCubeFace);
            YW_SAFE_RELEASE(srcSurface);
        }

        // Generate mip-map levels.
        (*irrandianceCubeTexture)->GenerateMipSubLevels(0);

        // Recovery viewport.
        device->SetViewportMatrix(&matViewportCurrent);

        // Release temp resources.
        YW_SAFE_RELEASE(rtCubemap);
        YW_SAFE_RELEASE(irrandianceMapVertexShader);
        YW_SAFE_RELEASE(irrandianceMapPixelShader);

        return true;
    }

    bool DemoPBRIBLPrecompute::RenderPrefilterReflectionMap(Graphics* graphics, Yw3dCubeTexture* envCubeTexture, Yw3dCubeTexture** prefilterReflectionCubeTexture, Model* modelSphere)
    {
        if ((nullptr == graphics) || (nullptr == envCubeTexture) || (nullptr == prefilterReflectionCubeTexture) || (nullptr == modelSphere))
        {
            return false;
        }
        
        Yw3dDevice* device = graphics->GetYw3dDevice();
        if (nullptr == device)
        {
            return false;
        }

        // Render constants.
        const int32_t targetWidth = 128;
        const int32_t targetHeight = 128;
        const int32_t cubeLength = 128;
        const int32_t cubeMaxMipLevels = 4; // Max texture lod level is 4.
        const Yw3dFormat cubeFormat = Yw3d_FMT_R32G32B32A32F;
        const float fovy = YW_PI / 2.0f;
        const float aspect = 1.0f;
        const float ZNear = 0.1f;
        const float zFar = 10.0f;

        // Create cube map.
        YW_SAFE_RELEASE(*prefilterReflectionCubeTexture);
        if (YW3D_FAILED(device->CreateCubeTexture(prefilterReflectionCubeTexture, cubeLength, cubeMaxMipLevels, cubeFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create cube texture failed."));
            return false;
        }

        // Generate mip-map levels to store prefilter result by roughness.
        (*prefilterReflectionCubeTexture)->GenerateMipSubLevels(0);

        // Backup old viewport matrix.sss
        const Matrix44* matViewportCurrentPointer;
        device->GetViewportMatrix(matViewportCurrentPointer);
        Matrix44 matViewportCurrent(*matViewportCurrentPointer);

        // Construct view matrices.
        Matrix44 matViews[Yw3d_CF_NumCubeFaces];
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_X], Vector3::Zero(), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_X], Vector3::Zero(), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Y], Vector3::Zero(), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Y], Vector3::Zero(), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));

        // Construct projection matrix.
        Matrix44 matProjection;
        Matrix44PerspectiveFovLH(matProjection, fovy, aspect, ZNear, zFar);

        // Create shader.
        DemoPBRIBLPrefilterReflectionMapVertexShader* prefilterReflectionMapVertexShader = new DemoPBRIBLPrefilterReflectionMapVertexShader();
        DemoPBRIBLPrefilterReflectionMapPixelShader* prefilterReflectionMapPixelShader = new DemoPBRIBLPrefilterReflectionMapPixelShader();

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set cube texture.
        graphics->SetTexture(0, envCubeTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(prefilterReflectionMapVertexShader);
        graphics->SetPixelShader(prefilterReflectionMapPixelShader);

        uint32_t mipLevels = min(5, (*prefilterReflectionCubeTexture)->GetMipLevels());
        for (uint32_t mip = 0; mip < mipLevels; mip++)
        {
            // Get cube edge size by mip level.
            uint32_t mipEdgeLength = (*prefilterReflectionCubeTexture)->GetEdgeLength(mip);

            // Set device viewport.
            Matrix44 matViewportCubeMap;
            Matrix44Viewport(matViewportCubeMap, 0, 0, mipEdgeLength, mipEdgeLength, 0.0f, 1.0f);
            device->SetViewportMatrix(&matViewportCubeMap);

            // Create a temp render target.
            Yw3dRenderTarget* rtPrefilterMap = nullptr;
            if (YW3D_FAILED(device->CreateRenderTarget(&rtPrefilterMap, mipEdgeLength, mipEdgeLength, cubeFormat, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
            {
                return false;
            }

            // Set render target.
            graphics->SetRenderTarget(rtPrefilterMap);

            // Create cube map face textures.
            for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
            {
                // Render to target and copy to cube face.
                device->Clear(nullptr, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

                // This should be from device.
                Matrix44 matWorld;
                Matrix44 matWVP = matWorld * matViews[i] * matProjection;
                device->SetTransform(Yw3d_TS_World, &matWorld);
                device->SetTransform(Yw3d_TS_View, &matViews[i]);
                device->SetTransform(Yw3d_TS_Projection, &matProjection);
                device->SetTransform(Yw3d_TS_WVP, &matWVP);

                float roughness = (float)mip / (float)(mipLevels - 1);
                prefilterReflectionMapPixelShader->SetFloat(0, roughness);

                // Render this face.
                modelSphere->Render(device);

                // Copy pixels from render target to cube face.
                Yw3dSurface* srcSurface = rtPrefilterMap->AcquireColorBuffer();
                Yw3dTexture* dstCubeFace = (*prefilterReflectionCubeTexture)->AcquireCubeFace((Yw3dCubeFaces)i);
                Yw3dSurface* dstSurface = dstCubeFace->AcquireMipLevel(mip);
                srcSurface->CopyToSurface(nullptr, dstSurface, nullptr, Yw3d_TF_Linear);
                YW_SAFE_RELEASE(dstSurface);
                YW_SAFE_RELEASE(dstCubeFace);
                YW_SAFE_RELEASE(srcSurface);
            }

            // Release render target.
            YW_SAFE_RELEASE(rtPrefilterMap);
        }

        // Recovery viewport.
        device->SetViewportMatrix(&matViewportCurrent);

        // Release temp resources.
        YW_SAFE_RELEASE(prefilterReflectionMapVertexShader);
        YW_SAFE_RELEASE(prefilterReflectionMapPixelShader);

        return true;
    }

    bool DemoPBRIBLPrecompute::RenderPreintegrateBRDFMap(Graphics* graphics, Yw3dTexture** preintegrateBRDFTexture)
    {
        if ((nullptr == graphics) || (nullptr == preintegrateBRDFTexture))
        {
            return false;
        }
        
        Yw3dDevice* device = graphics->GetYw3dDevice();
        if (nullptr == device)
        {
            return false;
        }

        // Render constants.
        const int32_t targetWidth = 512;
        const int32_t targetHeight = 512;
        const Yw3dFormat targetFormat = Yw3d_FMT_R32G32B32A32F;
        const float fovy = YW_PI / 2.0f;
        const float aspect = 1.0f;
        const float ZNear = 0.1f;
        const float zFar = 10.0f;
        const uint32_t numVertices = 4;

        // Create primitive data.

        // Define vertex format.
        struct VertexElement
        {
            Vector3 position;
            Vector2 uv;
        };

        // Vertex element declaration.
        Yw3dVertexElement VertexDeclaration[] =
        {
            YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 0),
            YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector2, 1)
        };

        // Create vertex format.
        Yw3dVertexFormat* vertexFormat = nullptr;
        if (YW3D_FAILED(device->CreateVertexFormat(&vertexFormat, VertexDeclaration, sizeof(VertexDeclaration))))
        {
            return false;
        }

        // Create vertex buffer.
        Yw3dVertexBuffer* vertexBuffer = nullptr;
        if (YW3D_FAILED(device->CreateVertexBuffer(&vertexBuffer, sizeof(VertexElement) * numVertices)))
        {
            return false;
        }

        // Get vertex buffer pointer.
        VertexElement* vertexElement = nullptr;
        if (YW3D_FAILED(vertexBuffer->GetPointer(0, (void**)&vertexElement)))
        {
            return false;
        }

        // NDC vertex position and texture coordinates.
        vertexElement[0].position = Vector3(-1.0f, -1.0f, 0.0f);
        vertexElement[0].uv = Vector2(1.0f / (float)targetWidth, 1.0f);
        vertexElement[1].position = Vector3(-1.0f, 1.0f, 0.0f);
        vertexElement[1].uv = Vector2(1.0f / (float)targetWidth, 0.0f);
        vertexElement[2].position = Vector3(1.0f, -1.0f, 0.0f);
        vertexElement[2].uv = Vector2(1.0f, 1.0f);
        vertexElement[3].position = Vector3(1.0f, 1.0f, 0.0f);
        vertexElement[3].uv = Vector2(1.0f, 0.0f);

        // Create render target and render texture.

        // Create a temp render target.
        Yw3dRenderTarget* rtBRDFMap = nullptr;
        if (YW3D_FAILED(device->CreateRenderTarget(&rtBRDFMap, targetWidth, targetHeight, targetFormat, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
        {
            return false;
        }

        // Create brdf texture, no need mipmap.
        YW_SAFE_RELEASE(*preintegrateBRDFTexture);
        if (YW3D_FAILED(device->CreateTexture(preintegrateBRDFTexture, targetWidth, targetHeight, 1, targetFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create brdf texture failed."));
            return false;
        }

        // Backup old viewport matrix.sss
        const Matrix44* matViewportCurrentPointer;
        device->GetViewportMatrix(matViewportCurrentPointer);
        Matrix44 matViewportCurrent(*matViewportCurrentPointer);

        // Set device viewport.
        Matrix44 matViewportCubeMap;
        Matrix44Viewport(matViewportCubeMap, 0, 0, targetWidth, targetHeight, 0.0f, 1.0f);
        device->SetViewportMatrix(&matViewportCubeMap);

        // Construct view matrices.
        Camera* camera = graphics->GetCurrentCamera();
        Matrix44 matViews; // = graphics->GetCurrentCamera()->GetViewMatrix();
        Matrix44LookAtLH(matViews, Vector3(0.0f, 0.0f, -2.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));

        // Construct projection matrix.
        Matrix44 matProjection;
        Matrix44PerspectiveFovLH(matProjection, fovy, aspect, ZNear, zFar);

        // Create shader.
        DemoPBRIBLPreintegrateBRDFMapVertexShader* preintegrateBRDFMapVertexShader = new DemoPBRIBLPreintegrateBRDFMapVertexShader();
        DemoPBRIBLPreintegrateBRDFMapPixelShader* preintegrateBRDFMapPixelShader = new DemoPBRIBLPreintegrateBRDFMapPixelShader();

        // Set render target.
        graphics->SetRenderTarget(rtBRDFMap);

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CCW); // Yw3d_Cull_CW
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set primitive data.
        // This non-persistent data should not be saved in "Graphics", because the data address will be invalid when leaving the stack of this function.
        device->SetVertexFormat(vertexFormat);
        device->SetVertexStream(0, vertexBuffer, 0, sizeof(VertexElement));

        // Set vertex and pixel shader.
        device->SetVertexShader(preintegrateBRDFMapVertexShader);
        device->SetPixelShader(preintegrateBRDFMapPixelShader);

        // Render to target and copy to cube face.
        device->Clear(nullptr, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

        // NDC vertices, no need transforms.

        // Render Quad.
        device->DrawPrimitive(Yw3d_PT_TriangleStrip, 0, 2);

        // Copy pixels from render target to cube face.
        Yw3dSurface* srcSurface = rtBRDFMap->AcquireColorBuffer();
        Yw3dSurface* dstSurface = (*preintegrateBRDFTexture)->AcquireMipLevel(0);
        srcSurface->CopyToSurface(nullptr, dstSurface, nullptr, Yw3d_TF_Linear);
        YW_SAFE_RELEASE(dstSurface);
        YW_SAFE_RELEASE(srcSurface);

        // Recovery viewport.
        device->SetViewportMatrix(&matViewportCurrent);

        YW_SAFE_RELEASE(vertexFormat);
        YW_SAFE_RELEASE(vertexBuffer);
        YW_SAFE_RELEASE(rtBRDFMap);
        YW_SAFE_RELEASE(preintegrateBRDFMapVertexShader);
        YW_SAFE_RELEASE(preintegrateBRDFMapPixelShader);

        return true;
    }
}
