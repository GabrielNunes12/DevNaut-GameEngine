#include "UI/ImGuiLayer.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "Renderer/DX11Context.h"
#include <SDL.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_dx11.h>

namespace Nova {

    void ImGuiLayer::Init(Window& window, DX11Context& context) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Multi-viewport / platform windows

        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplSDL2_InitForD3D(window.GetSDLWindow());
        ImGui_ImplDX11_Init(context.GetDevice(), context.GetDeviceContext());

        NOVA_ENGINE_INFO("ImGui initialized");
    }

    void ImGuiLayer::Shutdown() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        NOVA_ENGINE_INFO("ImGui shutdown");
    }

    void ImGuiLayer::BeginFrame() {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::EndFrame() {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ImGuiLayer::ProcessEvent(const SDL_Event& event) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

} // namespace Nova
