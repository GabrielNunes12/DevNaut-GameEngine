# NovaEngine

NovaEngine is a lightweight, modular C++ Game Engine built from the ground up for learning, performance, and extensibility. It features a custom DirectX 11 renderer, an integrated physics engine, a multi-provider AI Agent system, and a feature-rich editor.

## 🚀 Key Features

*   **AI Agent System**: Integrated support for multiple LLM providers (Gemini 2.5, OpenAI, Claude, LocalLLM) with a registry of 25+ specialized industry roles.
*   **Custom DX11 Renderer**: High-performance rendering backend using modern DirectX 11 APIs.
*   **Physics Engine**: Full integration with **ReactPhysics3D** for rigid-body dynamics and collision detection.
*   **Modular Architecture**: Clean separation between Engine core and Editor tools.
*   **Live Editor**: ImGui-based workspace with scene hierarchy, inspector, and transformation gizmos.
*   **Undo/Redo System**: Full command history for editor operations.
*   **Scene Serialization**: Save and load scenes using a flexible JSON-based format.

## 🏗️ Architecture & Implementation

### AI Agent Infrastructure
The engine features a robust AI system designed for real-time editor assistance and automated content generation.
*   **Multi-Provider Strategy**: Built on an extensible `IAgentProvider` interface, allowing seamless switching between Google Gemini, OpenAI, Claude, and Local LLMs (via LM Studio/Ollama).
*   **WinHTTP Networking**: Uses the modern **Windows HTTP Services (WinHTTP)** stack for secure (TLS 1.2/1.3) and high-performance API communication, ensuring reliability across different proxy environments.
*   **Role Registry**: Over 25+ specialized system prompts for roles such as **Level Designer**, **Lead Programmer**, **Technical Artist**, and **QA Engineer**, tailored specifically for game development workflows.

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
| **WinHTTP Stack** | Migrated from WinINet to WinHTTP to resolve strict security requirements (GFE) and provide better control over TLS protocols and header formatting for modern LLM APIs. |
| **DirectX 11** | Chosen for its balance of modern features (Compute shaders, Tessellation) and significantly lower complexity compared to DX12 or Vulkan, making it ideal for a custom engine project. |
| **ReactPhysics3D** | A lightweight, pure C++ physics library with no external dependencies. It's easier to integrate and more portable than heavier alternatives like PhysX or Havok. |
| **Hybrid Object Model** | Instead of a pure ECS (which can be hard to debug and over-engineered for mid-sized projects), we use a hybrid model where Entities are objects but their behavior is driven by optional components. |
| **AI Role Abstraction** | By decoupling the Agent's identity (Role) from its implementation (Provider), the engine allows users to swap models (e.g., Gemini to Claude) while maintaining the specialized context of the task. |

## 🛠️ Built With

*   **Language**: C++20
*   **Windowing/Events**: [SDL2](https://github.com/libsdl-org/SDL)
*   **Graphics API**: DirectX 11
*   **Networking**: [WinHTTP](https://docs.microsoft.com/en-us/windows/win32/winhttp/about-winhttp)
*   **Physics**: [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d)
*   **UI**: [Dear ImGui](https://github.com/ocornut/imgui) (Docking branch)
*   **Logging**: [spdlog](https://github.com/gabime/spdlog)
*   **JSON**: [nlohmann_json](https://github.com/nlohmann/json)

## 🗺️ Roadmap: What We Will Build

*   **AI-Driven Level Builder**: Using agents to automatically spawn and position entities based on natural language prompts.
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
