#pragma once

#include "Events/Event.h"
#include <functional>

namespace Nova {

    class EventDispatcher {
    public:
        explicit EventDispatcher(Event& event) : m_Event(event) {}

        // Dispatch the event to a handler if the type matches T.
        // Handler signature: bool Func(Event&) — return true if handled.
        template<EventType T>
        bool Dispatch(const std::function<bool(Event&)>& func) {
            if (m_Event.GetType() == T) {
                m_Event.SetHandled(func(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };

} // namespace Nova
