#pragma once

#include <cstdint>
#include <wrl/client.h> 
#include "Singleton.h"
#include "SimpleMathHelper.h"

#include <memory>
#include <vector>
#include "Texture.h"
#include "Material.h" // CULLMODE 때문에 나중에 뺴도 됨 따로
#include "RenderViewport.h"

#include "ConstantBuffers.h"

#include "ResourceManager.h"

struct HWND__;
using HWND = HWND__*;


struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain1;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Buffer;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;
struct ID3D11BlendState;
class Light;
class Camera;


namespace DirectX {
	inline namespace DX11 {
        class SpriteBatch;
        class SpriteFont;
        class CommonStates;
    }
}

enum class BlendMode { Opaque, AlphaBlend };




class DX11Renderer : public Singleton<DX11Renderer>
{
public:
    DX11Renderer();
    ~DX11Renderer();

    bool Initialize(HWND hwnd, int width, int height, bool vsync = false);
    void Resize(int width, int height);


    void BeginUIRender(float renderWidth, float renderHeight);
    void EndUIRender();

    void CreateShadowMap(int width, int height);

    void BeginShadowPass(const Vector3& lightPos, const Vector3& lightDir, bool isDirectional = true, float size = 30.0f);

    ID3D11ShaderResourceView* GetShadowMapSRV() const { return m_shadowSRV.Get(); }

    const Matrix& GetLightViewProjScaleMatrix() const { return m_lightViewProjScale; }

    void DrawUI(Texture* texture, const Vector2& position, const Vector2& size, const Vector4& color = Vector4(1, 1, 1, 1));
    
    // 텍스쳐는 그냥 쉐이더로 그리는게 맞다고 생각함 간단하고 조작도 쉽고
	// DXTK로 하면 MeshRenderer의 의존성을 줄여버릴 수 있으므로 그냥 이걸로 처리



    void DrawString(const std::wstring& text, const Vector2& position, const float& fontSize, const Vector4& color = Vector4(0, 0, 0, 1));

    //void DrawString3D(const std::wstring& text, const Vector3& localPos, const Vector4& color, const Matrix& worldMatrix);

    void SetBlendMode(BlendMode mode);
    void SetCullMode(CullMode mode);

    void BeginFrame(const float clearColor[4]);
    void BindGlobalResources(); // CB, SAMPLER , 기본 STATE 

    void CreateConstantBuffers();

    void EndFrame();
    void Present();

    void SetFullscreen(bool enable);
    void Destroy();

    void SetRenderTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv = nullptr);

    void SetViewport(float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f, float topLeftX = 0.0f, float topLeftY = 0.0f);

    HWND GetHwnd();
    ID3D11Device* GetDevice()  const;
    ID3D11DeviceContext* GetContext() const;
    ID3D11RenderTargetView* GetBackbufferRTV() const;

    int  Width()  const { return m_width; }
    int  Height() const { return m_height; }

    bool GetVsync() { return m_vsync; }
    void SetVsync(bool vsync) { m_vsync = vsync; }

	int GetRefWidth() const { return m_refWidth; }
	int GetRefHeight() const { return m_refHeight; }

    float GetAspectRatio() const
    {
        if (m_height == 0) return 1.0f;
        return static_cast<float>(m_width) / static_cast<float>(m_height);
    }

    float GetRefAspectRatio() const
    {
		return static_cast<float>(m_refWidth) / static_cast<float>(m_refHeight);
	}


    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }
    void SetWidth(int width) { m_width = width; }
    void SetHeight(int height) { m_height = height; }

    //CB Buffer Data map/unmap ; 외부에서 call 
    void UpdateObject_CBUFFER(const Matrix& Worrld, const Matrix& WorldTranspose); // r1
    void UpdateFrame_CBUFFER(const Matrix& viewTM, const Matrix& projectionTM);    // r0
    void UpdateLights_CBUFFER(const std::vector<Light*>& lights, Camera* camera);
    void UpdateMaterial_CBUFFER(const MaterialData& M_Data); //r3

    void UpdateBoneCBuffer(const std::vector<Matrix>& bones);

    void UpdateTextureFlag_CBUFFER(uint32_t Flags);
    void UpdateMatrixPallette_CBUFFER(std::vector<Matrix>& matrix);

    //기계 장치에 대한 Bind를 Cycle Update 다응에 매 프레임마다 설정하기 
    void ClearCache();
    void ResetRenderState();
    void BindShader(Shader* shader);
    void BindTexture(int slot, ID3D11ShaderResourceView* srv);
    ID3D11SamplerState* GetSampler(FilterMode filter, WrapMode wrap);

    const Matrix& GetViewMatrix() const { return m_viewTM; }
    const Matrix& GetProjectionMatrix() const { return m_projTM; }

    void OffPS();

private:
    bool CreateDeviceAndSwapchain();
    void CreateBackbuffers(int width, int height);
    void ReleaseBackbuffers();
    void ReleaseCB();
    void CreateSamplers();

   
    struct LightData
    {
        DirectX::SimpleMath::Vector4 PositionRange;  // xyz: 위치, w: 범위(Range)
        DirectX::SimpleMath::Vector4 DirectionType;  // xyz: 방향, w: 타입(0=Dir, 1=Point)
        DirectX::SimpleMath::Vector4 ColorIntensity; // xyz: 색상, w: 강도(Intensity)
    };


    constexpr static int MAX_LIGHTS = 4;

    __declspec(align(16))
        struct CBuffer_GlobalLight
    {
        LightData Lights[MAX_LIGHTS];                // 배열로 선언
        int ActiveCount;                             // 현재 활성화된 조명 개수
        Vector3 CameraPos;

        Vector3 CameraDir; 
        float IsOrtho;

        Matrix LightViewProjScale;
        Vector4 ShadowMapInfo; // 텍셀 크기
    };
 

private:
    // Platform
    HWND m_hwnd = nullptr;

    // D3D11 core
    Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>        m_swapchain;

    // Backbuffer & Depth
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_backbufferTex; 
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_depthTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;


    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBoolBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pLightBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_p_VP_MatBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIBL_Buffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pPointLight_Buffer = nullptr;


#pragma region CB

    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_frame = nullptr;  
    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_world_M = nullptr; 

    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_lights = nullptr; 
    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_material = nullptr; 

    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_Texture_flags = nullptr; 
    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_matrix_pallette = nullptr; 

    //01_10 일단 넌 후순위 
    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_IBL = nullptr; // 

#pragma endregion 

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_defaultDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_defaultRasterizerState;

    Microsoft::WRL::ComPtr<ID3D11BlendState>        m_alphaBlendState;
    //Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_defaultSamplerState;


    static constexpr int SamplerCnt = 6;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_samplers[6];
    Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_uiSampler;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_shadowSampler;

    // State
    int   m_width = 0;
    int   m_height = 0;
    bool  m_vsync = false;

    int m_refWidth = 1920;
	int m_refHeight = 1200; // 16:10 의도한거임  // 이게 UI의 기준이 되는 사이즈
	float m_refAspect = static_cast<float>(m_refWidth) / static_cast<float>(m_refHeight);

    Matrix m_viewTM;
    Matrix m_projTM;

    bool m_isOrthoBackup;

    // UI 렌더링
    std::unique_ptr<DirectX::DX11::SpriteBatch>  m_spriteBatch;
    std::unique_ptr<DirectX::DX11::SpriteFont>   m_font;
    std::unique_ptr<DirectX::DX11::CommonStates> m_states;


    // 컬링
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsCullBack; 
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsCullFront;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsCullNone; 


    Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_shadowMapTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_shadowDSV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowSRV;
    RenderViewport                                   m_shadowViewport;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_msaaTargetTex; 
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_msaaTargetRTV;
    UINT m_msaaQuality = 0;
    int m_msaa = 8;



    Matrix m_lightViewProjScale;

    static constexpr int MAX_BONES = 128;

    __declspec(align(16))
        struct CBuffer_BoneData
    {
        Matrix BoneTransforms[MAX_BONES];
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbuffer_bones;




    //cacching  ★ 
    private:
      uint16_t m_currentShaderID = 0; 
      ID3D11ShaderResourceView* m_currentSRVs[16] = { nullptr, };
};
