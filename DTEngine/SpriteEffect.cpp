#include "pch.h"
#include "SpriteEffect.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "AssetDatabase.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "DX11Renderer.h"
BEGINPROPERTY(SpriteEffect)
DTPROPERTY_SETTER(SpriteEffect , m_spriteID ,SetSpriteSheet)
DTPROPERTY_SETTER(SpriteEffect, m_frameInterpolation, SetFrameInterpolation)
DTPROPERTY_SETTER(SpriteEffect, m_play, SetPlay)
DTPROPERTY_SETTER(SpriteEffect, m_loop, SetLoop)


ENDPROPERTY()


struct SpriteSheetDim
{
	uint32_t cols = 1;
	uint32_t rows = 1;
};

SpriteSheetDim ParseSpriteSheetDimFromPath(const std::string& path)
{
    // 1. 파일 이름만 추출 (경로 구분자 이후만 남기기)
    size_t lastSlash = path.find_last_of("\\/");
    std::string fileName = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

    // 2. 마지막 '_' 찾기
    size_t lastUnderscore = fileName.find_last_of('_');
    if (lastUnderscore == std::string::npos) return { 1, 1 };

    // 3. 확장자 이전까지의 문자열 추출 (예: "4X8")
    size_t lastDot = fileName.find_last_of('.');
    if (lastDot == std::string::npos || lastDot < lastUnderscore)
        lastDot = fileName.length();

    // "4X8" 부분만 잘라냄
    std::string dimStr = fileName.substr(lastUnderscore + 1, lastDot - (lastUnderscore + 1));

    // 4. 'X' 또는 'x' 위치 찾기
    size_t xPos = dimStr.find_first_of("Xx");
    if (xPos == std::string::npos) return { 1, 1 };

    try
    {
        uint32_t cols = std::stoul(dimStr.substr(xPos + 1));
        uint32_t rows = std::stoul(dimStr.substr(0 ,xPos));

        return { cols, rows };
    }
    catch (...)
    {
        // 숫자가 아니거나 변환 실패 시 기본값 반환
        return { 0, 0 };
    }
}



void SpriteEffect::Update(float dTime)
{
    if (!m_play) return;                
    if (m_cols == 0 || m_rows == 0) return;
    if (m_frameInterpolation <= 0.f) return;

    // 비루프 + 이미 끝났으면 더 이상 update 안 함
    if (!m_loop && m_finished) return;

    m_currentTime += dTime;

    const int totalFrames = static_cast<int>(m_cols * m_rows);
    int frameIdx = static_cast<int>(m_currentTime / m_frameInterpolation);

    if (m_loop)
    {
        frameIdx %= totalFrames;
    }
    else
    {
        // 끝났으면: 초기화 + update stop
        if (frameIdx >= totalFrames)
        {
            ResetToFirstFrame();     // “00으로”
            m_finished = true;
            m_play = false;          // update 안 받게
            return;
        }
    }

    const float uScale = 1.0f / m_cols;
    const float vScale = 1.0f / m_rows;

    const uint32_t currCol = static_cast<uint32_t>(frameIdx % m_cols);
    const uint32_t currRow = static_cast<uint32_t>(frameIdx / m_cols);

    m_uvTrasnform.x = uScale;
    m_uvTrasnform.y = vScale;
    m_uvTrasnform.z = currCol * uScale;
    m_uvTrasnform.w = currRow * vScale;
}

void SpriteEffect::ResetToFirstFrame()
{
    if (m_cols == 0 || m_rows == 0)
    {
        m_uvTrasnform = { 1.f, 1.f, 0.f, 0.f };
        return;
    }

    float uScale = 1.0f / m_cols;
    float vScale = 1.0f / m_rows;

    m_uvTrasnform.x = uScale;
    m_uvTrasnform.y = vScale;
    m_uvTrasnform.z = 0.f;
    m_uvTrasnform.w = 0.f;
}


void SpriteEffect::SetSpriteSheet(uint64_t id)
{
	if (m_spriteID == id) return;

	std::string path = AssetDatabase::Instance().GetPathFromID(id);

	if (path != "") 	m_spriteID = id;
	else return;


	Texture* texture = ResourceManager::Instance().Load<Texture>(path);

	if (texture) m_texture = texture;
	else
	{
		m_spriteID = 0;
		return;
	}

    SpriteSheetDim temp =  ParseSpriteSheetDimFromPath(path);
    if (!temp.cols || !temp.rows) return;

    //프레임 갯수 
    m_cols =  temp.cols;
    m_rows = temp.rows;

    m_spriteName = path;



}


void SpriteEffect::BindWrapped()
{
    auto cmp = _GetOwner()->GetComponent<MeshRenderer>();
    if (cmp)
    {
        auto mat = cmp->GetSharedMaterial();
        if(mat)mat->SetUVTransform(m_uvTrasnform);
    }
    __super::BindEP(); 
}

void SpriteEffect::SetPlay(bool onoff)
{
    if (m_play == onoff) return;
    m_play = onoff;

    if (m_play)
    {
        m_currentTime = 0.f;
        m_finished = false;
        m_startedOnce = true;
        ResetToFirstFrame();
    }
    else
    {
        ResetToFirstFrame();
    }
}
