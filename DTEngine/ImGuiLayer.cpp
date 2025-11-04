#include "pch.h"
#include "ImGuiLayer.h"
#include "DX11Renderer.h"

#include <Windows.h>
#include <d3d11.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

ImGuiLayer::ImGuiLayer() = default;
ImGuiLayer::~ImGuiLayer() { Shutdown(); }

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool ImGuiLayer::Initialize(DX11Renderer* renderer)
{
    if (renderer == nullptr) { return false; }
    
    m_renderer = renderer;
    m_hwnd = renderer ? renderer->GetHwnd() : nullptr;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // 백엔드 초기화
    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_renderer->GetDevice(), m_renderer->GetContext());
    return true;
}

void ImGuiLayer::NewFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::Shutdown()
{
    if (ImGui::GetCurrentContext() == nullptr) return;
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool ImGuiLayer::WndProcHandler(HWND hWnd, uint32_t msg, uintptr_t wParam, intptr_t lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, (UINT)msg, (WPARAM)wParam, (LPARAM)lParam) != 0;
}

bool ImGuiLayer::WantsMouse() const { return ImGui::GetIO().WantCaptureMouse; }
bool ImGuiLayer::WantsKeyboard() const { return ImGui::GetIO().WantCaptureKeyboard; }
