# ── Nova Editor Executable ───────────────────────────────────────────────────
add_executable(NovaEditor
    Source/main.cpp
)

target_link_libraries(NovaEditor PRIVATE
    NovaEngine
)

if (MSVC)
    target_compile_options(NovaEditor PRIVATE /FS)
endif()

# Copy SDL2 DLL next to executable on Windows
add_custom_command(TARGET NovaEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:NovaEditor>
)
