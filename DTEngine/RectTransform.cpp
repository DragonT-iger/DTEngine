#include "pch.h"
#include "RectTransform.h"
#include "Transform.h"
#include "GameObject.h"
#include "Canvas.h"
#include "SimpleMathHelper.h"

BEGINPROPERTY(RectTransform)
DTPROPERTY_ACCESSOR(RectTransform, m_anchorMin, GetAnchorMin, SetAnchorMin)
DTPROPERTY_ACCESSOR(RectTransform, m_anchorMax, GetAnchorMax, SetAnchorMax)
DTPROPERTY_ACCESSOR(RectTransform, m_pivot, GetPivot, SetPivot)
DTPROPERTY_ACCESSOR(RectTransform, m_sizeDelta, GetSizeDelta, SetSizeDelta)
DTPROPERTY_ACCESSOR(RectTransform, m_anchoredPosition, GetAnchoredPosition, SetAnchoredPosition)
ENDPROPERTY()

void RectTransform::SetAnchorMin(const Vector2& value)
{
    m_anchorMin = value;
    m_transformOverride = false;
}

void RectTransform::SetAnchorMax(const Vector2& value)
{
    m_anchorMax = value;
    m_transformOverride = false;
}

void RectTransform::SetPivot(const Vector2& value)
{
    m_pivot = value;
    m_transformOverride = false;
}

void RectTransform::SetSizeDelta(const Vector2& value)
{
    m_sizeDelta = value;
    m_transformOverride = false;
}

void RectTransform::SetAnchoredPosition(const Vector2& value)
{
    m_anchoredPosition = value;
    m_transformOverride = false;
}

RectTransform* RectTransform::GetParentRect() const
{
    const Transform* tf = GetTransform();
    if (!tf) return nullptr;

    Transform* parent = tf->GetParent();
    if (!parent) return nullptr;

    return parent->_GetOwner()->GetComponent<RectTransform>();
}

static bool NearlyEqual(float a, float b, float epsilon = 0.01f)
{
    return std::abs(a - b) <= epsilon;
}

static bool NearlyEqual(const Vector2& a, const Vector2& b, float epsilon = 0.01f)
{
    return NearlyEqual(a.x, b.x, epsilon) && NearlyEqual(a.y, b.y, epsilon);
}


static Canvas* GetCanvasInHierarchy(Transform* start)
{
    Transform* current = start;
    while (current)
    {
        if (auto* canvas = current->_GetOwner()->GetComponent<Canvas>())
        {
            //printf("[CanvasPick] go=%s canvasGO=%s ref=(%.0f,%.0f) match=%.3f\n",
            //    start->_GetOwner()->GetName().c_str(),
            //    current->_GetOwner()->GetName().c_str(),
            //    canvas->GetReferenceResolution().x, canvas->GetReferenceResolution().y,
            //    canvas->GetMatchWidthOrHeight());
            return canvas;
        }
        current = current->GetParent();
    }
    return nullptr;
}

void RectTransform::ApplyLayout(float screenWidth, float screenHeight)
{
    if (screenWidth <= 0.0f || screenHeight <= 0.0f) return;

    RectTransform* parentRect = GetParentRect();
    Transform* tf = GetTransform();
    Canvas* canvas = tf ? GetCanvasInHierarchy(tf) : nullptr;
    float canvasScale = canvas ? canvas->GetScaleFactor(screenWidth, screenHeight) : 1.0f;
    if (canvasScale <= 0.0f)
    {
        canvasScale = 1.0f;
    }

    Vector2 parentSize = Vector2(screenWidth, screenHeight);
    Vector2 parentCenter = Vector2(0.0f, 0.0f);

    bool hasParentRect = (parentRect != nullptr);
    if (parentRect)
    {
        parentSize = parentRect->GetSize();
        parentCenter = parentRect->GetWorldCenter2D();
    }
    else
    {
        if (canvas)
        {
            parentSize = canvas->GetCanvasSize(screenWidth / canvasScale, screenHeight / canvasScale);
        }
    }

    Vector2 parentMin = Vector2(-parentSize.x * 0.5f, -parentSize.y * 0.5f);
    Vector2 anchorMinPos = parentMin + Vector2(parentSize.x * m_anchorMin.x, parentSize.y * m_anchorMin.y);
    Vector2 anchorMaxPos = parentMin + Vector2(parentSize.x * m_anchorMax.x, parentSize.y * m_anchorMax.y);
    Vector2 anchorSpan = anchorMaxPos - anchorMinPos;

    Vector2 size = anchorSpan + m_sizeDelta;
    Vector2 anchorCenter = (anchorMinPos + anchorMaxPos) * 0.5f;

    Vector2 pivotOffset = Vector2((0.5f - m_pivot.x) * size.x, (0.5f - m_pivot.y) * size.y);
    Vector2 localCenter = anchorCenter + m_anchoredPosition + pivotOffset;
    Vector2 worldCenter = parentCenter + localCenter;
    //Vector2 layoutCenter = parentRect ? localCenter : worldCenter;

    //float layoutScale = parentRect ? 1.0f : canvasScale;

    Vector2 layoutCenter = hasParentRect ? localCenter : worldCenter;
    float layoutScale = hasParentRect ? 1.0f : canvasScale;

    if (tf)
    {
        //printf("[UI] sw=%.1f sh=%.1f cs=%.6f pw=%.3f ph=%.3f ls=%.6f\n",
        //    screenWidth, screenHeight,
        //    canvasScale,
        //    parentSize.x, parentSize.y,
        //    layoutScale);

       /* Vector3 position = tf->GetPosition();
        position.x = layoutCenter.x * layoutScale;
        position.y = layoutCenter.y * layoutScale;
        tf->SetPosition(position);

        Vector3 scale = tf->GetScale();
        scale.x = size.x * layoutScale;
        scale.y = size.y * layoutScale;
        tf->SetScale(scale);*/

        //printf("[UI] pos=(%.3f,%.3f) size=(%.3f,%.3f)\n",position.x, position.y, scale.x, scale.y);

        Vector2 desiredPosition = layoutCenter * layoutScale;
        Vector2 desiredScale = size * layoutScale;

        Vector2 parentScale = parentSize;
        if (parentScale.x == 0.0f) parentScale.x = 1.0f;
        if (parentScale.y == 0.0f) parentScale.y = 1.0f;


        Vector3 currentPosition = tf->GetPosition();
        Vector3 currentScale = tf->GetScale();
        Vector2 currentPosition2D = Vector2(currentPosition.x, currentPosition.y);
        Vector2 currentScale2D = Vector2(currentScale.x, currentScale.y);

        bool transformEdited = m_hasApplied &&
            (!NearlyEqual(currentPosition2D, m_lastAppliedPosition) ||
                !NearlyEqual(currentScale2D, m_lastAppliedScale));

        if (transformEdited)
        {
            float sourceScale = m_lastAppliedLayoutScale > 0.0001f ? m_lastAppliedLayoutScale : layoutScale;
            if (sourceScale <= 0.0001f)
            {
                sourceScale = 1.0f;
            }

            Vector2 sizeFromTransform = hasParentRect
                ? Vector2(currentScale2D.x * parentScale.x, currentScale2D.y * parentScale.y)
                : currentScale2D / sourceScale;
            Vector2 localCenterFromTransform = hasParentRect
                ? currentPosition2D
                : (currentPosition2D / sourceScale) - parentCenter;

            if (!std::isfinite(sizeFromTransform.x) || !std::isfinite(sizeFromTransform.y))
            {
                sizeFromTransform = size;
            }

            if (!std::isfinite(localCenterFromTransform.x) || !std::isfinite(localCenterFromTransform.y))
            {
                localCenterFromTransform = localCenter;
            }
            Vector2 pivotOffsetFromTransform = Vector2((0.5f - m_pivot.x) * sizeFromTransform.x, (0.5f - m_pivot.y) * sizeFromTransform.y);

            m_sizeDelta = sizeFromTransform - anchorSpan;
            m_anchoredPosition = localCenterFromTransform - anchorCenter - pivotOffsetFromTransform;

            size = anchorSpan + m_sizeDelta;
            pivotOffset = Vector2((0.5f - m_pivot.x) * size.x, (0.5f - m_pivot.y) * size.y);
            localCenter = anchorCenter + m_anchoredPosition + pivotOffset;
            worldCenter = parentCenter + localCenter;

            desiredPosition = (hasParentRect ? localCenter : worldCenter) * layoutScale;
            desiredScale = hasParentRect
                ? Vector2(size.x / parentScale.x, size.y / parentScale.y)
                : size * layoutScale;

            currentPosition.x = desiredPosition.x;
            currentPosition.y = desiredPosition.y;
            tf->SetPosition(currentPosition);

            currentScale.x = desiredScale.x;
            currentScale.y = desiredScale.y;
            tf->SetScale(currentScale);

            currentPosition2D = desiredPosition;
            currentScale2D = desiredScale;
        }
        else
        {
            if (hasParentRect)
            {
                desiredScale = Vector2(size.x / parentScale.x, size.y / parentScale.y);
            }

            currentPosition.x = desiredPosition.x;
            currentPosition.y = desiredPosition.y;
            tf->SetPosition(currentPosition);

            currentScale.x = desiredScale.x;
            currentScale.y = desiredScale.y;
            tf->SetScale(currentScale);

            currentPosition2D = desiredPosition;
            currentScale2D = desiredScale;
        }

        m_lastAppliedPosition = currentPosition2D;
        m_lastAppliedScale = currentScale2D;
        m_lastAppliedLayoutScale = layoutScale;
        m_hasApplied = true;
    }

    m_cachedSize = size;
    m_cachedWorldCenter = worldCenter;
}

void RectTransform::ApplyLayoutRecursive(float screenWidth, float screenHeight)
{
    ApplyLayout(screenWidth, screenHeight);

    Transform* tf = GetTransform();
    if (!tf) return;

    for (Transform* child : tf->GetChildren())
    {
        if (!child) continue;
        RectTransform* childRect = child->_GetOwner()->GetComponent<RectTransform>();
        if (childRect)
        {
            childRect->ApplyLayoutRecursive(screenWidth, screenHeight);
        }
    }
}

Vector2 RectTransform::GetScreenPosition(float screenWidth, float screenHeight) const
{
    Transform* tf = _GetOwner() ? _GetOwner()->GetTransform() : nullptr;
    Canvas* canvas = tf ? GetCanvasInHierarchy(tf) : nullptr;
    float canvasScale = canvas ? canvas->GetScaleFactor(screenWidth, screenHeight) : 1.0f;
    if (canvasScale <= 0.0f)
    {
        canvasScale = 1.0f;
    }

    Vector2 screenCenter = Vector2(screenWidth * 0.5f, screenHeight * 0.5f);
    Vector2 scaledCenter = m_cachedWorldCenter * canvasScale;

    return Vector2(screenCenter.x + scaledCenter.x, screenCenter.y - scaledCenter.y);
}