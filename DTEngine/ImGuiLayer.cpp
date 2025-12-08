#include "pch.h"
#include "ImGuiLayer.h"
#include "DX11Renderer.h"

#include <Windows.h>
#include <d3d11.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include "Text.h"

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

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         


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


    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");


    if (ImGui::DockBuilderGetNode(dockspace_id) == NULL)
    {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGuiID dock_main_id = dockspace_id;

        ImGuiID dock_inspector_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);

        ImGuiID dock_hierarchy_group_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.3f, nullptr, &dock_main_id);

        ImGuiID dock_project_id = ImGui::DockBuilderSplitNode(dock_hierarchy_group_id, ImGuiDir_Down, 0.4f, nullptr, &dock_hierarchy_group_id);
        ImGuiID dock_hierarchy_id = dock_hierarchy_group_id;

        ImGuiID dock_game_id = 0;
        ImGuiID dock_scene_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.5f, nullptr, &dock_game_id);

        ImGui::DockBuilderDockWindow("Inspector", dock_inspector_id);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_hierarchy_id);
        ImGui::DockBuilderDockWindow("Project", dock_project_id);
        ImGui::DockBuilderDockWindow("Scene", dock_scene_id);
        ImGui::DockBuilderDockWindow("Game", dock_game_id);

        ImGui::DockBuilderFinish(dockspace_id);
    }



    ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

}



void ImGuiLayer::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
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
