#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"
#include "RenderTexture.h"
#include "IPostProcess.h"

struct Ray;

class Camera : public MonoBehaviour
{
    DTGENERATED_BODY(Camera);
public:

    Camera();
    virtual ~Camera() = default;

    void Awake() override;
    void LateUpdate(float deltaTime) override;

    void SetProjectionPerspective();
    void SetProjectionOrthographic();

    const Matrix& GetViewMatrix() const { return m_view; }
    const Matrix& GetProjectionMatrix() const { return m_projection; }

    void SetThisCameraToMain();

    void Bind();

    const float& GetNearZ() { return m_nearZ; }
    void SetNearZ(float nearZ) {
        m_nearZ = nearZ;
        
        //m_dirtyProj = true;
    }

    const float& GetFarZ() { return m_farZ; }
    void SetFarZ(float farZ) {
        m_farZ = farZ;

        //m_dirtyProj = true;
    }

    const Vector4& GetClearColor() const { return m_clearColor; }
    void SetClearColor(const Vector4& color) { m_clearColor = color; }


    const Vector3& GetCamPos() const;
    const Vector3& GetCamFor() const;

    const float& GetEditorFovY() const { return m_editorFovY; }
    void SetEditorFovY(float editorFovY) { m_editorFovY = editorFovY; m_fovY = SimpleMathHelper::Deg2Rad(editorFovY);  m_dirtyProj = true; }

    void SetViewDirty() { m_dirtyView = true; }
    void SetAspectRatio(float ratio) { m_aspectRatio = ratio; m_dirtyProj = true; }
	const float& GetAspectRatio() const { return m_aspectRatio; }

	const bool& IsOrthographic() const { return m_isOrthographic; }
	void SetIsOrthographic(bool isOrtho) { m_isOrthographic = isOrtho; m_dirtyProj = true; }

	const float& GetOrthographicSize() const { return m_orthographicSize; }
	void SetOrthographicSize(float size) { m_orthographicSize = size; m_dirtyProj = true; }

    void SetTargetTexture(RenderTexture* rt) { m_targetTexture = rt; }
    RenderTexture* GetTargetTexture() const { return m_targetTexture; }

    const Vector4& GetViewportRect() const { return m_viewportRect; }
    void SetViewportRect(const Vector4& rect) { m_viewportRect = rect; }

    Ray ScreenPointToRay(float x, float y, float viewportW, float viewportH) const;

    void SetPostProcessEffect(PostProcessType type, bool enable);
    bool IsEffectEnabled(PostProcessType type) const;
    uint32_t GetPostProcessMask() const { return m_postProcessMask; }


    const bool& GetUseGrayScale() const { return m_useGrayScale; }
    void SetUseGrayScale(bool use);

    const bool& GetUseVignette() const { return m_useVignette; }
    void SetUseVignette(bool use);

	const bool& GetUseBloom() const { return m_useBloom; }
	void SetUseBloom(bool use) { m_useBloom = use; }

    const float& GetBloomThreshold() const { return m_bloomThreshold; }
    void SetBloomThreshold(float value) { m_bloomThreshold = value; }

    const float& GetBloomIntensity() const { return m_bloomIntensity; }
    void SetBloomIntensity(float value) { m_bloomIntensity = value; }

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    Matrix m_view;
    Matrix m_projection;



    bool m_dirtyView = true;
    bool m_dirtyProj = true;


    RenderTexture* m_targetTexture = nullptr;

	// 아래는 직렬화와 관련된 값들 
    // fovY는 const &로 만들수가 없어서 따로 저장
    float m_aspectRatio = -1;

    float m_fovY = SimpleMathHelper::Deg2Rad(60.0f);
    float m_editorFovY = 60.0f;

    float m_nearZ = 0.1f;
    float m_farZ = 1000.0f;


	bool m_isOrthographic = false;
    float m_orthographicSize = 5.0f;

    Vector4 m_clearColor = { 0.2f,0.2f,0.2f,0.2f };


    Vector4 m_viewportRect = { 0.0f, 0.0f, 1.0f, 1.0f };

    bool m_enablePostProcess = true;

    uint32_t m_postProcessMask = static_cast<uint32_t>(PostProcessType::None);

    bool m_useGrayScale = false;
    bool m_useVignette = false;

	bool m_useBloom = false;
    float m_bloomThreshold = 1.0f; 
    float m_bloomIntensity = 1.0f; 
};