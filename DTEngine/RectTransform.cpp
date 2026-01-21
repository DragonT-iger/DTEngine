#include "pch.h"
//#include "RectTransform.h"
//#include "Transform.h"
//#include "GameObject.h"
//#include "Canvas.h"
//
//BEGINPROPERTY(RectTransform)
//DTPROPERTY_ACCESSOR(RectTransform, m_anchorMin, GetAnchorMin, SetAnchorMin)
//DTPROPERTY_ACCESSOR(RectTransform, m_anchorMax, GetAnchorMax, SetAnchorMax)
//DTPROPERTY_ACCESSOR(RectTransform, m_pivot, GetPivot, SetPivot)
//DTPROPERTY_ACCESSOR(RectTransform, m_sizeDelta, GetSizeDelta, SetSizeDelta)
//DTPROPERTY_ACCESSOR(RectTransform, m_anchoredPosition, GetAnchoredPosition, SetAnchoredPosition)
//DTPROPERTY_ACCESSOR(RectTransform, m_useTransformAsSource, GetUseTransformAsSource, SetUseTransformAsSource)
//ENDPROPERTY()
//
//void RectTransform::SetAnchorMin(const Vector2& value)
//{
//    m_anchorMin = value;
//}
//
//void RectTransform::SetAnchorMax(const Vector2& value)
//{
//    m_anchorMax = value;
//}
//
//void RectTransform::SetPivot(const Vector2& value)
//{
//    m_pivot = value;
//}
//
//void RectTransform::SetSizeDelta(const Vector2& value)
//{
//    m_sizeDelta = value;
//}
//
//void RectTransform::SetAnchoredPosition(const Vector2& value)
//{
//    m_anchoredPosition = value;
//}
//
//RectTransform* RectTransform::GetParentRect() const
//{
//    const Transform* tf = GetTransform();
//    if (!tf) return nullptr;
//
//    Transform* parent = tf->GetParent();
//    if (!parent) return nullptr;
//
//    return parent->_GetOwner()->GetComponent<RectTransform>();
//}
//
//static bool NearlyEqual(float a, float b, float epsilon = 0.001f)
//{
//    return std::abs(a - b) <= epsilon;
//}
//
//static bool NearlyEqual(const Vector3& a, const Vector3& b, float epsilon = 0.001f)
//{
//    return NearlyEqual(a.x, b.x, epsilon) && NearlyEqual(a.y, b.y, epsilon) && NearlyEqual(a.z, b.z, epsilon);
//}
//
//static Canvas* GetCanvasInHierarchy(Transform* start)
//{
//    Transform* current = start;
//    while (current)
//    {
//        if (auto* canvas = current->_GetOwner()->GetComponent<Canvas>())
//        {
//            //printf("[CanvasPick] go=%s canvasGO=%s ref=(%.0f,%.0f) match=%.3f\n",
//            //    start->_GetOwner()->GetName().c_str(),
//            //    current->_GetOwner()->GetName().c_str(),
//            //    canvas->GetReferenceResolution().x, canvas->GetReferenceResolution().y,
//            //    canvas->GetMatchWidthOrHeight());
//            return canvas;
//        }
//        current = current->GetParent();
//    }
//    return nullptr;
//}
//void RectTransform::ApplyLayout(float screenWidth, float screenHeight)
//{
//    if (screenWidth <= 0.0f || screenHeight <= 0.0f) return;
//    Transform* tf = GetTransform();
//    if (!tf) return;
//
//    Canvas* canvas = GetCanvasInHierarchy(tf);
//    if (!canvas) return;
//
//    float layoutScale = canvas->GetScaleFactor(screenWidth, screenHeight);
//    Vector2 canvasSize = canvas->GetCanvasSize(screenWidth, screenHeight);
//
//    // 1. 역산 로직 (에디터 조작 대응)
//    if (m_useTransformAsSource)
//    {
//        Vector3 currentPos = tf->GetPosition();
//        Vector3 currentScale = tf->GetScale();
//
//        // [핵심] 이전 값과 오차가 아주 작으면(0.0001 미만) 무시함 (Drift 방지)
//        if (m_isFirstLayout ||
//            (currentPos - m_lastTransformPos).LengthSquared() > 0.000001f ||
//            (currentScale - m_lastTransformScale).LengthSquared() > 0.000001f)
//        {
//            if (layoutScale > 0.0f) {
//                m_sizeDelta.x = currentScale.x / layoutScale;
//                m_sizeDelta.y = currentScale.y / layoutScale;
//            }
//
//            Vector2 anchorCenter = (m_anchorMin + m_anchorMax) * 0.5f;
//            // 좌표 역산 공식 최적화
//            m_anchoredPosition.x = (currentPos.x / layoutScale) - (canvasSize.x * (anchorCenter.x - 0.5f));
//            m_anchoredPosition.y = (currentPos.y / layoutScale) - (canvasSize.y * (anchorCenter.y - 0.5f));
//
//            m_isFirstLayout = false;
//        }
//    }
//
//    // 2. 레이아웃 계산 (Forward)
//    Vector2 anchorMinPos = { canvasSize.x * m_anchorMin.x, canvasSize.y * m_anchorMin.y };
//    Vector2 anchorMaxPos = { canvasSize.x * m_anchorMax.x, canvasSize.y * m_anchorMax.y };
//
//    Vector2 size = m_sizeDelta;
//    if (m_anchorMin.x != m_anchorMax.x) size.x = (anchorMaxPos.x - anchorMinPos.x) + m_sizeDelta.x;
//    if (m_anchorMin.y != m_anchorMax.y) size.y = (anchorMaxPos.y - anchorMinPos.y) + m_sizeDelta.y;
//
//    Vector2 pivotOffset = { size.x * (m_pivot.x - 0.5f), size.y * (m_pivot.y - 0.5f) };
//    Vector2 anchorCenterPos = (anchorMinPos + anchorMaxPos) * 0.5f;
//    Vector2 worldCenter = anchorCenterPos + m_anchoredPosition - pivotOffset - (canvasSize * 0.5f);
//
//    // 3. Transform에 결과 적용 및 스냅샷 저장
//    if (layoutScale > 0.0f)
//    {
//        tf->SetPosition(Vector3(worldCenter.x * layoutScale, worldCenter.y * layoutScale, 0.0f));
//        tf->SetScale(Vector3(size.x * layoutScale, size.y * layoutScale, 1.0f));
//    }
//
//    // 다음 프레임 비교용 데이터 저장 (SetPosition 이후에 수행)
//    m_lastTransformPos = tf->GetPosition();
//    m_lastTransformScale = tf->GetScale();
//
//    // 4. 스크린 충돌 박스 계산
//    Vector2 centerInScreen;
//    centerInScreen.x = (screenWidth * 0.5f) + (worldCenter.x * layoutScale);
//    centerInScreen.y = (screenHeight * 0.5f) - (worldCenter.y * layoutScale);
//
//    Vector2 scaledSize = size * layoutScale;
//
//    // 이 계산이 소수점 오차에 민감하므로 정확하게 캐싱
//    m_screenRect.left = centerInScreen.x - (scaledSize.x * m_pivot.x);
//    m_screenRect.top = centerInScreen.y - (scaledSize.y * (1.0f - m_pivot.y));
//    m_screenRect.right = m_screenRect.left + scaledSize.x;
//    m_screenRect.bottom = m_screenRect.top + scaledSize.y;
//
//    m_cachedSize = size;
//    m_cachedWorldCenter = worldCenter;
//}
//
//void RectTransform::ApplyLayoutRecursive(float screenWidth, float screenHeight)
//{
//    ApplyLayout(screenWidth, screenHeight);
//
//    Transform* tf = GetTransform();
//    if (!tf) return;
//
//    for (Transform* child : tf->GetChildren())
//    {
//        if (!child) continue;
//        RectTransform* childRect = child->_GetOwner()->GetComponent<RectTransform>();
//        if (childRect)
//        {
//            childRect->ApplyLayoutRecursive(screenWidth, screenHeight);
//        }
//    }
//}
//
//Vector2 RectTransform::GetScreenPosition(float screenWidth, float screenHeight) const
//{
//    Transform* tf = _GetOwner() ? _GetOwner()->GetTransform() : nullptr;
//    Canvas* canvas = tf ? GetCanvasInHierarchy(tf) : nullptr;
//    float canvasScale = canvas ? canvas->GetScaleFactor(screenWidth, screenHeight) : 1.0f;
//    if (canvasScale <= 0.0f)
//    {
//        canvasScale = 1.0f;
//    }
//
//    Vector2 screenCenter = Vector2(screenWidth * 0.5f, screenHeight * 0.5f);
//    Vector2 scaledCenter = m_cachedWorldCenter * canvasScale;
//
//    return Vector2(screenCenter.x + scaledCenter.x, screenCenter.y - scaledCenter.y);
//}
