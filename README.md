# NovaEngine

NovaEngine is a lightweight, modular C++ Game Engine built from the ground up for learning, performance, and extensibility. It features a custom DirectX 11 renderer, an integrated physics engine, and a feature-rich editor.

## 🚀 Key Features

*   **Custom DX11 Renderer**: High-performance rendering backend using modern DirectX 11 APIs.
*   **Physics Engine**: Full integration with **ReactPhysics3D** for rigid-body dynamics and collision detection.
*   **Modular Architecture**: Clean separation between Engine core and Editor tools.
*   **Live Editor**: ImGui-based workspace with scene hierarchy, inspector, and transformation gizmos.
*   **Undo/Redo System**: Full command history for editor operations.
*   **Scene Serialization**: Save and load scenes using a flexible JSON-based format.

## 🏗️ Architecture & Implementation

### Core & OS Abstraction
The engine uses **SDL2** for windowing, input management, and cross-platform event handling. This allows the core logic to remain platform-independent while focusing on high-level engine systems.

### Renderer Implementation
The rendering system is built on **DirectX 11**. 
*   **Context Management**: `DX11Context` handles the device, swap chain, and state management.
*   **Shader System**: Supports custom HLSL shaders with automated constant buffer binding (MVP, Lighting).
*   **Debug Rendering**: A specialized `DebugRenderer` allows for visual debugging of physics colliders and engine primitives.

### Physics System
Integration with **ReactPhysics3D** provides robust 3D physics.
*   **Hybrid Entity-Component Model**: Physics components (`Rigidbody`, `Collider`) are optional members of the `Entity` class, allowing for efficient access without the overhead of a full ECS for smaller projects.
*   **Observer Pattern**: The engine implements a `CollisionObserver` interface, allowing systems to "subscribe" to collision events (Start/End) and trigger overlaps.

### Editor Design Patterns
The Editor is built with extensibility in mind using several key architectural patterns:
*   **Strategy Pattern**: Used for `EditorAction` to decouple UI menu logic from the actual operations (e.g., adding entities, saving files).
*   **Command Pattern**: Every transformation and scene modification is wrapped in a `Command`, enabling a robust Undo/Redo history via `CommandHistory`.
*   **Observer Pattern**: Used for synchronizing state between the scene, the hierarchy, and the inspector.

## 🧠 Design Decisions & Rationale

| Decision | Why? |
| :--- | :--- |
| **DirectX 11** | Chosen for its balance of modern features (Compute shaders, Tessellation) and significantly lower complexity compared to DX12 or Vulkan, making it ideal for a custom engine project. |
| **ReactPhysics3D** | A lightweight, pure C++ physics library with no external dependencies. It's easier to integrate and more portable than heavier alternatives like PhysX or Havok. |
| **Hybrid Object Model** | Instead of a pure ECS (which can be hard to debug and over-engineered for mid-sized projects), we use a hybrid model where Entities are objects but their behavior is driven by optional components. |
| **Pure C++ Scripting** | To ensure maximum performance and type safety, scripting is handled through internal C++ logic modules rather than an external language like Lua or C#. |

## 🛠️ Built With

*   **Language**: C++20
*   **Windowing/Events**: [SDL2](https://github.com/libsdl-org/SDL)
*   **Graphics API**: DirectX 11
*   **Physics**: [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d)
*   **UI**: [Dear ImGui](https://github.com/ocornut/imgui) (Docking branch)
*   **Logging**: [spdlog](https://github.com/gabime/spdlog)
*   **JSON**: [nlohmann_json](https://github.com/nlohmann/json)

## 🗺️ Roadmap: What We Will Build

*   **C++ Scripting System**: A cleaner interface for attaching C++ logic classes to entities.
*   **Advanced Rendering**: Implementation of PBR (Physically Based Rendering), Shadows, and Post-Processing.
*   **Asset Pipeline**: A centralized system for loading and managing textures, models, and audio.
*   **Animation System**: Support for skeletal animation and blending.
*   **Enhanced Physics**: Support for more collider types (Mesh, Heightfield) and constraints (Hinges, Sliders).

## 🔨 Getting Started

1.  **Clone the repository**:
    ```bash
    git clone --recursive https://github.com/GabrielNunes12/DevNaut-GameEngine.git
    ```
2.  **Generate Project Files** (using CMake):
    ```bash
    mkdir build
    cd build
    cmake ..
    ```
3.  **Build and Run**: Open the generated solution (Windows) or use `cmake --build .`
