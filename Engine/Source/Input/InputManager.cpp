#include "Input/InputManager.h"
#include <SDL.h>

namespace Nova {

    // Static member initialization
    uint8_t InputManager::s_KeyboardState[512] = {};
    uint8_t InputManager::s_PrevKeyboardState[512] = {};
    int InputManager::s_MouseX = 0;
    int InputManager::s_MouseY = 0;
    int InputManager::s_MouseDeltaX = 0;
    int InputManager::s_MouseDeltaY = 0;
    uint32_t InputManager::s_MouseButtons = 0;
    uint32_t InputManager::s_PrevMouseButtons = 0;
    int InputManager::s_ScrollDelta = 0;

    void InputManager::Init() {
        const uint8_t* state = SDL_GetKeyboardState(nullptr);
        if (state) std::memcpy(s_KeyboardState, state, 512);
        std::memset(s_PrevKeyboardState, 0, sizeof(s_PrevKeyboardState));
    }

    void InputManager::Update() {
        // Snapshot current keyboard state into previous before copying new state
        std::memcpy(s_PrevKeyboardState, s_KeyboardState, 512);

        // Copy new SDL keyboard state
        const uint8_t* state = SDL_GetKeyboardState(nullptr);
        if (state) {
            std::memcpy(s_KeyboardState, state, 512);
        }

        // Store previous mouse buttons
        s_PrevMouseButtons = s_MouseButtons;
        s_MouseButtons = SDL_GetMouseState(&s_MouseX, &s_MouseY);
        SDL_GetRelativeMouseState(&s_MouseDeltaX, &s_MouseDeltaY);

        // Scroll delta is accumulated via ProcessEvent and reset each frame
        // (reset happens AFTER reading — so we reset at the START of next frame)
    }

    void InputManager::ProcessEvent(const SDL_Event& event) {
        if (event.type == SDL_MOUSEWHEEL) {
            s_ScrollDelta += event.wheel.y;  // Accumulate scroll within the frame
        }
    }

    // ── Keyboard ────────────────────────────────────────────────────────────

    bool InputManager::IsKeyDown(int scancode) {
        if (!s_KeyboardState) return false;
        return s_KeyboardState[scancode] != 0;
    }

    bool InputManager::IsKeyUp(int scancode) {
        return !IsKeyDown(scancode);
    }

    bool InputManager::IsKeyJustPressed(int scancode) {
        if (!s_KeyboardState) return false;
        return s_KeyboardState[scancode] != 0 && s_PrevKeyboardState[scancode] == 0;
    }

    bool InputManager::IsKeyJustReleased(int scancode) {
        if (!s_KeyboardState) return false;
        return s_KeyboardState[scancode] == 0 && s_PrevKeyboardState[scancode] != 0;
    }

    // ── Mouse ───────────────────────────────────────────────────────────────

    bool InputManager::IsMouseButtonDown(int button) {
        return (s_MouseButtons & SDL_BUTTON(button)) != 0;
    }

    bool InputManager::IsMouseButtonJustPressed(int button) {
        uint32_t mask = SDL_BUTTON(button);
        return (s_MouseButtons & mask) != 0 && (s_PrevMouseButtons & mask) == 0;
    }

    std::pair<int, int> InputManager::GetMousePosition() {
        return { s_MouseX, s_MouseY };
    }

    std::pair<int, int> InputManager::GetMouseDelta() {
        return { s_MouseDeltaX, s_MouseDeltaY };
    }

    int InputManager::GetScrollDelta() {
        int delta = s_ScrollDelta;
        s_ScrollDelta = 0;  // Reset after reading
        return delta;
    }

} // namespace Nova
