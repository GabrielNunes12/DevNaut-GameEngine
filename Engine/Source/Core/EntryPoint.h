#pragma once

// ── Entry Point ─────────────────────────────────────────────────────────────
// This header defines main(). Include it ONCE in your application's .cpp file
// after defining Nova::CreateApplication().

#ifdef NOVA_PLATFORM_WINDOWS

extern Nova::Application* Nova::CreateApplication();

int main(int argc, char* argv[]) {
    auto* app = Nova::CreateApplication();
    app->Run();
    delete app;
    return 0;
}

#endif
