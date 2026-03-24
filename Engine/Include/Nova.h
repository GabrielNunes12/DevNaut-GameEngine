#pragma once

// ── Nova Engine Public API ──────────────────────────────────────────────────
// Include this single header to access the full engine API.

#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Core/FileUtils.h"

#include "Events/Event.h"
#include "Events/EventDispatcher.h"

#include "Input/InputManager.h"

#include "Math/NovaMath.h"

#include "Renderer/Renderer.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/ConstantBuffer.h"

#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/Picking.h"
#include "Scene/SceneSerializer.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/Rigidbody.h"
#include "Physics/Collider.h"
#include "Physics/CollisionObserver.h"

#include "Editor/Gizmo.h"
#include "UI/ImGuiLayer.h"
#include <imgui.h>

// ── Entry Point ─────────────────────────────────────────────────────────────
#include "Core/EntryPoint.h"
