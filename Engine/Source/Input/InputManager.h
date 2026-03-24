#pragma once

#include <utility>
#include <cstdint>
#include <cstring>

union SDL_Event;

namespace Nova {

    class InputManager {
    public:
        static void Init();
        static void Update();

        // Call from Window::PollEvents to feed raw SDL events
        static void ProcessEvent(const SDL_Event& event);

        // Keyboard — held state
        static bool IsKeyDown(int scancode);
        static bool IsKeyUp(int scancode);

        // Keyboard — edge detection (single-frame)
        static bool IsKeyJustPressed(int scancode);
        static bool IsKeyJustReleased(int scancode);

        // Mouse — buttons
        static bool IsMouseButtonDown(int button);
        static bool IsMouseButtonJustPressed(int button);

        // Mouse — position & delta
        static std::pair<int, int> GetMousePosition();
        static std::pair<int, int> GetMouseDelta();

        // Mouse — scroll wheel (accumulated per frame)
        static int GetScrollDelta();

    private:
        // Keyboard
        static uint8_t  s_KeyboardState[512];
        static uint8_t  s_PrevKeyboardState[512];

        // Mouse
        static int s_MouseX, s_MouseY;
        static int s_MouseDeltaX, s_MouseDeltaY;
        static uint32_t s_MouseButtons;
        static uint32_t s_PrevMouseButtons;

        // Scroll
        static int s_ScrollDelta;
    };

} // namespace Nova
