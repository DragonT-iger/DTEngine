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
    if (m_cols == 0 || m_rows == 0) return;

    m_currentTime += dTime;

    // 전체 프레임 수
    int totalFrames = m_cols * m_rows;
    float currentFrameFloat = m_currentTime / m_frameInterpolation;

    // 루프 애니메이션 처리
    int currentIdx = static_cast<int>(currentFrameFloat) % totalFrames;

    float uScale = 1.0f / m_cols;
    float vScale = 1.0f / m_rows;

    uint32_t currCol = currentIdx % m_cols;
    uint32_t currRow = currentIdx / m_cols;

    m_uvTrasnform.x = uScale;
    m_uvTrasnform.y = vScale;
    m_uvTrasnform.z = currCol * uScale;
    m_uvTrasnform.w = currRow * vScale;
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
