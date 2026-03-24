# ── Nova Engine Library ──────────────────────────────────────────────────────
file(GLOB_RECURSE ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)
list(APPEND ENGINE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/FileUtils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Physics/PhysicsWorld.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/DebugRenderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scene/SceneSerializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Scene/Entity.cpp"
)

file(GLOB_RECURSE ENGINE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h"
)

add_library(NovaEngine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(NovaEngine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

# ── Dependencies ─────────────────────────────────────────────────────────────
target_link_libraries(NovaEngine PUBLIC
    SDL2::SDL2
    SDL2::SDL2main
    spdlog::spdlog
    ImGui
    reactphysics3d
    nlohmann_json::nlohmann_json
    d3d11
    dxgi
    d3dcompiler
    comdlg32
)

# ── Preprocessor ─────────────────────────────────────────────────────────────
target_compile_definitions(NovaEngine PUBLIC
    NOVA_PLATFORM_WINDOWS
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

if (MSVC)
    target_compile_options(NovaEngine PUBLIC /FS)
endif()
