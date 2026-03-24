#pragma once

#include <cstdint>
#include <string>

namespace Nova {

    // ── Event Types ─────────────────────────────────────────────────────────
    enum class EventType : uint8_t {
        None = 0,
        // Window
        WindowClose,
        WindowResize,
        // Keyboard
        KeyPressed,
        KeyReleased,
        // Mouse
        MouseMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseScrolled
    };

    // ── Event Category (bitmask) ────────────────────────────────────────────
    enum EventCategory : uint8_t {
        EventCategory_None        = 0,
        EventCategory_Application = 1 << 0,
        EventCategory_Input       = 1 << 1,
        EventCategory_Keyboard    = 1 << 2,
        EventCategory_Mouse       = 1 << 3
    };

    // ── Event ───────────────────────────────────────────────────────────────
    class Event {
    public:
        explicit Event(EventType type) : m_Type(type) {}
        virtual ~Event() = default;

        EventType   GetType()    const { return m_Type; }
        bool        IsHandled()  const { return m_Handled; }
        void        SetHandled(bool handled = true) { m_Handled = handled; }

        const char* GetName() const {
            switch (m_Type) {
                case EventType::WindowClose:          return "WindowClose";
                case EventType::WindowResize:         return "WindowResize";
                case EventType::KeyPressed:           return "KeyPressed";
                case EventType::KeyReleased:          return "KeyReleased";
                case EventType::MouseMoved:           return "MouseMoved";
                case EventType::MouseButtonPressed:   return "MouseButtonPressed";
                case EventType::MouseButtonReleased:  return "MouseButtonReleased";
                case EventType::MouseScrolled:        return "MouseScrolled";
                default:                              return "Unknown";
            }
        }

    protected:
        EventType m_Type = EventType::None;
        bool m_Handled = false;
    };

} // namespace Nova
