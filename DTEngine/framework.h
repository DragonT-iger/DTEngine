#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <cassert>
#include <memory>









// 일반 new/delete 포인터 안전 삭제
template <typename T>
inline void SafeDelete(T*& p) noexcept {
    if (p) { delete p; p = nullptr; }
}

// new[]/delete[] 배열 포인터 안전 삭제
template <typename T>
inline void SafeDeleteArray(T*& p) noexcept {
    if (p) { delete[] p; p = nullptr; }
}

// 핸들/리소스 커스텀 종료자 패턴
// 사용 예: SafeClose(handle, [](auto h){ ::CloseHandle(h); });
template <typename T, typename Closer>
inline void SafeClose(T& handle, Closer closer) noexcept {
    if (handle) {
        closer(handle);
        handle = {};
    }
}