#pragma once

class RenderTexture;

enum class PostProcessType : uint32_t
{
    None        = 0,
    Bloom       = 1 << 0,
    ToneMapping = 1 << 1,
    Vignette    = 1 << 2,
    Blur        = 1 << 3,
    Chromatic   = 1 << 4, 
    GrayScale   = 1 << 5,
    All         = 0xFFFFFFFF
};

inline PostProcessType operator|(PostProcessType a, PostProcessType b) {
    return static_cast<PostProcessType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline uint32_t operator&(PostProcessType a, PostProcessType b) {
    return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
}

class IPostProcess
{
public:
    virtual ~IPostProcess() = default;
    virtual void Initialize() = 0;
    virtual void Render(RenderTexture* src, RenderTexture* dest = nullptr) = 0;

    virtual PostProcessType GetType() const = 0;
};