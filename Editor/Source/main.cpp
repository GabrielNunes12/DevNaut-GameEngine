#include <Nova.h>
#include "EditorAction.h"
#include <SDL.h>
#include "Editor/CommandHistory.h"
#include "Editor/TransformCommand.h"
#include "Editor/EntityCommand.h"
#include "Renderer/DebugRenderer.h"
#include "AgentInspector.h"

// ── HLSL Shaders (embedded) ────────────────────────────────────────────────

static const std::string g_VertexShader = R"(
cbuffer MVPBuffer : register(b0) {
    matrix Model;
    matrix ViewProjection;
};

struct VSInput {
    float3 Position : POSITION;
    float4 Color    : COLOR;
    float3 Normal   : NORMAL;
};

struct PSInput {
    float4 Position : SV_POSITION;
    float4 Color    : COLOR;
    float3 WorldPos : TEXCOORD0;
    float3 Normal   : TEXCOORD1;
};

PSInput VSMain(VSInput input) {
    PSInput output;
    float4 worldPos = mul(float4(input.Position, 1.0), Model);
    output.Position = mul(worldPos, ViewProjection);
    output.Color    = input.Color;
    output.WorldPos = worldPos.xyz;
    
    if (length(input.Normal) > 0.1) {
        output.Normal = normalize(mul(input.Normal, (float3x3)Model));
    } else {
        output.Normal = float3(0, 0, 0);
    }
    return output;
}
)";

static const std::string g_PixelShader = R"(
cbuffer LightBuffer : register(b1) {
    float4 LightPos;    // xyz = pos, w = intensity
    float4 LightColor;  // rgb
    float4 CameraPos;   // xyz
};

struct PSInput {
    float4 Position : SV_POSITION;
    float4 Color    : COLOR;
    float3 WorldPos : TEXCOORD0;
    float3 Normal   : TEXCOORD1;
};

float4 PSMain(PSInput input) : SV_TARGET {
    if (length(input.Normal) < 0.1) {
        return input.Color;
    }
    float3 norm = normalize(input.Normal);
    float ambientStrength = 0.2;
    float3 ambient = ambientStrength * float3(1.0, 1.0, 1.0);
    float3 lightDir = normalize(LightPos.xyz - input.WorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = diff * LightColor.xyz * LightPos.w;
    float3 result = (ambient + diffuse) * input.Color.rgb;
    return float4(result, input.Color.a);
}
)";

struct MVPData {
    DirectX::XMFLOAT4X4 Model;
    DirectX::XMFLOAT4X4 ViewProjection;
};

struct LightData {
    DirectX::XMFLOAT4 LightPos;
    DirectX::XMFLOAT4 LightColor;
    DirectX::XMFLOAT4 CameraPos;
};

static Nova::Vertex g_CubeVertices[] = {
    { {-0.5f, -0.5f, -0.5f}, {0.9f, 0.2f, 0.2f, 1.0f}, { 0.0f,  0.0f, -1.0f} },
    { {-0.5f,  0.5f, -0.5f}, {0.9f, 0.3f, 0.3f, 1.0f}, { 0.0f,  0.0f, -1.0f} },
    { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.3f, 0.2f, 1.0f}, { 0.0f,  0.0f, -1.0f} },
    { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.2f, 0.2f, 1.0f}, { 0.0f,  0.0f, -1.0f} },
    { {-0.5f, -0.5f,  0.5f}, {0.2f, 0.3f, 0.9f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
    { { 0.5f, -0.5f,  0.5f}, {0.2f, 0.2f, 1.0f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
    { { 0.5f,  0.5f,  0.5f}, {0.3f, 0.3f, 1.0f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
    { {-0.5f,  0.5f,  0.5f}, {0.2f, 0.4f, 0.9f, 1.0f}, { 0.0f,  0.0f,  1.0f} },
    { {-0.5f,  0.5f, -0.5f}, {0.2f, 0.9f, 0.3f, 1.0f}, { 0.0f,  1.0f,  0.0f} },
    { {-0.5f,  0.5f,  0.5f}, {0.2f, 1.0f, 0.3f, 1.0f}, { 0.0f,  1.0f,  0.0f} },
    { { 0.5f,  0.5f,  0.5f}, {0.3f, 1.0f, 0.2f, 1.0f}, { 0.0f,  1.0f,  0.0f} },
    { { 0.5f,  0.5f, -0.5f}, {0.3f, 0.9f, 0.2f, 1.0f}, { 0.0f,  1.0f,  0.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.9f, 0.9f, 0.2f, 1.0f}, { 0.0f, -1.0f,  0.0f} },
    { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.9f, 0.2f, 1.0f}, { 0.0f, -1.0f,  0.0f} },
    { { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 0.2f, 1.0f}, { 0.0f, -1.0f,  0.0f} },
    { {-0.5f, -0.5f,  0.5f}, {0.9f, 1.0f, 0.2f, 1.0f}, { 0.0f, -1.0f,  0.0f} },
    { { 0.5f, -0.5f, -0.5f}, {0.7f, 0.2f, 0.9f, 1.0f}, { 1.0f,  0.0f,  0.0f} },
    { { 0.5f,  0.5f, -0.5f}, {0.8f, 0.3f, 1.0f, 1.0f}, { 1.0f,  0.0f,  0.0f} },
    { { 0.5f,  0.5f,  0.5f}, {0.7f, 0.3f, 1.0f, 1.0f}, { 1.0f,  0.0f,  0.0f} },
    { { 0.5f, -0.5f,  0.5f}, {0.6f, 0.2f, 0.9f, 1.0f}, { 1.0f,  0.0f,  0.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.2f, 0.8f, 0.9f, 1.0f}, {-1.0f,  0.0f,  0.0f} },
    { {-0.5f, -0.5f,  0.5f}, {0.2f, 0.9f, 1.0f, 1.0f}, {-1.0f,  0.0f,  0.0f} },
    { {-0.5f,  0.5f,  0.5f}, {0.3f, 0.9f, 1.0f, 1.0f}, {-1.0f,  0.0f,  0.0f} },
    { {-0.5f,  0.5f, -0.5f}, {0.3f, 0.8f, 0.9f, 1.0f}, {-1.0f,  0.0f,  0.0f} },
};

static uint32_t g_CubeIndices[] = {
     0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11,
     12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
};

enum class GizmoMode { None, TranslateX, TranslateY, TranslateZ };
enum class TransformMode { Position, Rotation, Scale };

class EditorApp : public Nova::Application {
public:
    EditorApp() : Application("Nova Engine — Editor") {}

    void OnInit() override {
        auto* device = GetRenderer().GetContext().GetDevice();
        m_Shader.CompileFromSource(device, g_VertexShader, g_PixelShader);
        m_VertexBuffer.Create(device, g_CubeVertices, _countof(g_CubeVertices));
        m_IndexBuffer.Create(device, g_CubeIndices, _countof(g_CubeIndices));
        m_MVPBuffer.Create(device);
        m_LightBuffer.Create(device);
        float aspect = static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight());
        m_Camera.SetPerspective(60.0f, aspect, 0.1f, 100.0f);
        m_Camera.SetPosition({ 0.0f, 1.5f, -4.0f });
        m_Camera.LookAt({ 0.0f, 0.0f, 0.0f });
        m_Gizmo.Init(device);
        m_DebugRenderer.Init(device);
        BuildMenu();
        NOVA_LOG_INFO("Editor initialized");
    }

    std::vector<Nova::MenuCategory> m_MenuCategories;

    void BuildMenu() {
        m_MenuCategories.clear();
        // File Menu
        Nova::MenuCategory fileMenu;
        fileMenu.Name = "File";
        fileMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("New Scene", [this]() { m_Scene.Clear(); m_CommandHistory.Clear(); }));
        fileMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Save Scene", [this]() {
            auto path = Nova::FileUtils::SaveFile("Nova Scene (*.nova)\0*.nova\0");
            if (path) { Nova::SceneSerializer serializer(m_Scene); serializer.Serialize(*path); }
        }));
        fileMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Load Scene", [this]() {
            auto path = Nova::FileUtils::OpenFile("Nova Scene (*.nova)\0*.nova\0");
            if (path) { m_Scene.Clear(); m_CommandHistory.Clear(); Nova::SceneSerializer serializer(m_Scene); serializer.Deserialize(*path); }
        }));
        fileMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Exit", [this]() { GetWindow().Close(); }));
        m_MenuCategories.push_back(fileMenu);

        // Add Menu
        Nova::MenuCategory addMenu;
        addMenu.Name = "Add";
        addMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Cube (Box)", [this]() {
            auto* e = m_Scene.AddEntity("Cube", Nova::EntityType::Mesh);
            e->AddCollider(Nova::ColliderType::Box);
            auto ptr = m_Scene.GetEntityPtr(e->GetId());
            m_CommandHistory.AddCommand(std::make_shared<Nova::AddEntityCommand>(m_Scene, ptr));
        }));
        addMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Sphere", [this]() {
            auto* e = m_Scene.AddEntity("Sphere", Nova::EntityType::Mesh);
            e->AddCollider(Nova::ColliderType::Sphere);
            auto ptr = m_Scene.GetEntityPtr(e->GetId());
            m_CommandHistory.AddCommand(std::make_shared<Nova::AddEntityCommand>(m_Scene, ptr));
        }));
        addMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Point Light", [this]() {
            auto* e = m_Scene.AddEntity("Point Light", Nova::EntityType::Light);
            e->GetTransform().Position = { 0.0f, 2.0f, 0.0f };
            auto ptr = m_Scene.GetEntityPtr(e->GetId());
            m_CommandHistory.AddCommand(std::make_shared<Nova::AddEntityCommand>(m_Scene, ptr));
        }));
        addMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Camera", [this]() {
            auto* e = m_Scene.AddEntity("Main Camera", Nova::EntityType::Camera);
            e->GetTransform().Position = { 0.0f, 1.5f, -4.0f };
            auto ptr = m_Scene.GetEntityPtr(e->GetId());
            m_CommandHistory.AddCommand(std::make_shared<Nova::AddEntityCommand>(m_Scene, ptr));
        }));
        addMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Empty GameObject", [this]() {
            auto* e = m_Scene.AddEntity("Empty", Nova::EntityType::Empty);
            auto ptr = m_Scene.GetEntityPtr(e->GetId());
            m_CommandHistory.AddCommand(std::make_shared<Nova::AddEntityCommand>(m_Scene, ptr));
        }));
        m_MenuCategories.push_back(addMenu);

        // AI Menu
        Nova::MenuCategory aiMenu;
        aiMenu.Name = "AI Agents";
        aiMenu.Actions.push_back(std::make_shared<Nova::LambdaAction>("Open Command Center", [this]() { 
            // The panel is always on, but we could add a toggle here
        }));
        m_MenuCategories.push_back(aiMenu);
    }

    void OnUpdate(float dt) override {
        auto [mx, my] = Nova::InputManager::GetMousePosition();
        auto [dx, dy] = Nova::InputManager::GetMouseDelta();
        ImGuiIO& io = ImGui::GetIO();
        bool isFreecam = Nova::InputManager::IsMouseButtonDown(SDL_BUTTON_RIGHT) && !io.WantCaptureMouse;

        if (Nova::InputManager::IsKeyDown(SDL_SCANCODE_LCTRL)) {
            if (Nova::InputManager::IsKeyJustPressed(SDL_SCANCODE_Z)) m_CommandHistory.Undo();
            if (Nova::InputManager::IsKeyJustPressed(SDL_SCANCODE_Y)) m_CommandHistory.Redo();
        }

        if (isFreecam) {
            float pitch = m_Camera.GetPitch() + dy * 0.2f;
            float yaw   = m_Camera.GetYaw() + dx * 0.2f;
            m_Camera.SetRotation(pitch, yaw);
            Nova::Vec3 pos = m_Camera.GetPosition();
            float speed = 5.0f * dt;
            if (Nova::InputManager::IsKeyDown(SDL_SCANCODE_LSHIFT)) speed *= 2.0f;
            Nova::Vec3 fwd = m_Camera.GetForward(); Nova::Vec3 right = m_Camera.GetRight();
            if (Nova::InputManager::IsKeyDown(SDL_SCANCODE_W)) { pos.x += fwd.x * speed; pos.y += fwd.y * speed; pos.z += fwd.z * speed; }
            if (Nova::InputManager::IsKeyDown(SDL_SCANCODE_S)) { pos.x -= fwd.x * speed; pos.y -= fwd.y * speed; pos.z -= fwd.z * speed; }
            if (Nova::InputManager::IsKeyDown(SDL_SCANCODE_D)) { pos.x += right.x * speed; pos.y += right.y * speed; pos.z += right.z * speed; }
            if (Nova::InputManager::IsKeyDown(SDL_SCANCODE_A)) { pos.x -= right.x * speed; pos.y -= right.y * speed; pos.z -= right.z * speed; }
            m_Camera.SetPosition(pos);
        }

        if (!io.WantCaptureMouse && !isFreecam) {
            if (Nova::InputManager::IsKeyJustPressed(SDL_SCANCODE_W)) m_TransformMode = TransformMode::Position;
            if (Nova::InputManager::IsKeyJustPressed(SDL_SCANCODE_R)) m_TransformMode = TransformMode::Rotation;
            if (Nova::InputManager::IsKeyJustPressed(SDL_SCANCODE_T)) m_TransformMode = TransformMode::Scale;
        }

        m_Scene.OnPhysicsUpdate(dt);

        bool isMouseDown = Nova::InputManager::IsMouseButtonDown(SDL_BUTTON_LEFT);
        bool isMouseJustPressed = Nova::InputManager::IsMouseButtonJustPressed(SDL_BUTTON_LEFT);

        if (!io.WantCaptureMouse && !isFreecam) {
            Nova::Entity* selected = m_Scene.GetSelectedEntity();
            int screenW = GetWindow().GetWidth();
            int screenH = GetWindow().GetHeight();

            if (isMouseJustPressed) {
                Nova::Ray ray = Nova::Picking::ScreenToRay(mx, my, screenW, screenH, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix());
                bool gizmoHit = false;
                if (selected) {
                    Nova::Vec3 pos = selected->GetTransform().Position;
                    DirectX::XMVECTOR objPos = DirectX::XMLoadFloat3(&pos);
                    DirectX::XMVECTOR camPos = DirectX::XMLoadFloat3(&m_Camera.GetPosition());
                    float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(camPos, objPos)));
                    float scale = dist * 0.15f;
                    GizmoMode hitMode = GizmoMode::None; float hitOffset = 0.0f; float rT, lT;
                    float threshold = scale * 0.4f;

                    if (m_TransformMode == TransformMode::Rotation) {
                        auto check = [&](DirectX::XMVECTOR normal, GizmoMode mode) {
                            float nd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray.Direction, normal));
                            if (abs(nd) > 1e-5f) {
                                float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(objPos, ray.Origin), normal)) / nd;
                                if (t > 0) {
                                    DirectX::XMVECTOR hitP = DirectX::XMVectorAdd(ray.Origin, DirectX::XMVectorScale(ray.Direction, t));
                                    if (abs(DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(hitP, objPos))) - scale) < threshold) { hitMode = mode; hitOffset = t; }
                                }
                            }
                        };
                        check(DirectX::XMVectorSet(1,0,0,0), GizmoMode::TranslateX);
                        check(DirectX::XMVectorSet(0,1,0,0), GizmoMode::TranslateY);
                        check(DirectX::XMVectorSet(0,0,1,0), GizmoMode::TranslateZ);
                    } else {
                        auto checkAxis = [&](DirectX::XMVECTOR axis, GizmoMode mode) {
                            float d = Nova::Picking::ClosestPointRayLine(ray.Origin, ray.Direction, objPos, axis, rT, lT);
                            if (d < threshold && lT >= 0 && lT <= scale) { hitMode = mode; hitOffset = lT; }
                        };
                        checkAxis(DirectX::XMVectorSet(1,0,0,0), GizmoMode::TranslateX);
                        checkAxis(DirectX::XMVectorSet(0,1,0,0), GizmoMode::TranslateY);
                        checkAxis(DirectX::XMVectorSet(0,0,1,0), GizmoMode::TranslateZ);
                    }
                    if (hitMode != GizmoMode::None) { m_GizmoMode = hitMode; m_GizmoDragOffset = hitOffset; m_InitialTransform = selected->GetTransform(); gizmoHit = true; }
                }
                if (!gizmoHit) {
                    m_Scene.SetSelectedEntity(Nova::Picking::PickEntity(m_Scene, mx, my, screenW, screenH, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix()));
                    m_GizmoMode = GizmoMode::None;
                }
            } else if (isMouseDown && m_GizmoMode != GizmoMode::None && selected) {
                Nova::Ray ray = Nova::Picking::ScreenToRay(mx, my, screenW, screenH, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix());
                DirectX::XMVECTOR startPos = DirectX::XMLoadFloat3(&m_InitialTransform.Position);
                DirectX::XMVECTOR axis;
                if (m_GizmoMode == GizmoMode::TranslateX) axis = DirectX::XMVectorSet(1,0,0,0);
                else if (m_GizmoMode == GizmoMode::TranslateY) axis = DirectX::XMVectorSet(0,1,0,0);
                else axis = DirectX::XMVectorSet(0,0,1,0);

                if (m_TransformMode == TransformMode::Rotation) {
                    float angle = (dx + dy) * 0.01f;
                    if (m_GizmoMode == GizmoMode::TranslateX) selected->GetTransform().Rotation.x += angle;
                    if (m_GizmoMode == GizmoMode::TranslateY) selected->GetTransform().Rotation.y += angle;
                    if (m_GizmoMode == GizmoMode::TranslateZ) selected->GetTransform().Rotation.z += angle;
                } else {
                    float rT, lT;
                    Nova::Picking::ClosestPointRayLine(ray.Origin, ray.Direction, startPos, axis, rT, lT);
                    float delta = lT - m_GizmoDragOffset;
                    if (m_TransformMode == TransformMode::Position) DirectX::XMStoreFloat3(&selected->GetTransform().Position, DirectX::XMVectorAdd(startPos, DirectX::XMVectorScale(axis, delta)));
                    else {
                        if (m_GizmoMode == GizmoMode::TranslateX) selected->GetTransform().Scale.x = m_InitialTransform.Scale.x + delta;
                        if (m_GizmoMode == GizmoMode::TranslateY) selected->GetTransform().Scale.y = m_InitialTransform.Scale.y + delta;
                        if (m_GizmoMode == GizmoMode::TranslateZ) selected->GetTransform().Scale.z = m_InitialTransform.Scale.z + delta;
                    }
                }
            }
        }

        if (!isMouseDown && m_GizmoMode != GizmoMode::None) {
            Nova::Entity* selected = m_Scene.GetSelectedEntity();
            if (selected) m_CommandHistory.AddCommand(std::make_shared<Nova::TransformCommand>(m_Scene, selected->GetId(), m_InitialTransform, selected->GetTransform()));
            m_GizmoMode = GizmoMode::None;
        }

        if (!io.WantCaptureKeyboard && !isFreecam) {
            Nova::Entity* selected = m_Scene.GetSelectedEntity();
            if (selected && Nova::InputManager::IsKeyJustPressed(SDL_SCANCODE_DELETE)) {
                auto ptr = m_Scene.GetEntityPtr(selected->GetId());
                m_CommandHistory.AddCommand(std::make_shared<Nova::DeleteEntityCommand>(m_Scene, ptr));
                m_Scene.RemoveEntity(selected->GetId()); m_Scene.SetSelectedEntity(nullptr);
            }
        }
    }

    void OnRender() override {
        auto* ctx = GetRenderer().GetContext().GetDeviceContext();
        using namespace DirectX;
        XMMATRIX vp = m_Camera.GetViewProjectionMatrix();
        XMVECTOR camPos = XMLoadFloat3(&m_Camera.GetPosition());
        LightData lightData = { {0,5,0,1}, {1,1,1,1}, {0,0,0,0} };
        XMStoreFloat4(&lightData.CameraPos, camPos);
        for (auto& e : m_Scene.GetEntities()) if (e->Type == Nova::EntityType::Light) {
            Nova::Vec3 p = e->GetTransform().Position; lightData.LightPos = {p.x, p.y, p.z, 2.0f}; break;
        }
        m_LightBuffer.Update(ctx, lightData); m_LightBuffer.BindPS(ctx, 1);
        m_Shader.Bind(ctx); m_VertexBuffer.Bind(ctx); m_IndexBuffer.Bind(ctx);
        for (auto& entity : m_Scene.GetEntities()) {
            if (entity->Type != Nova::EntityType::Mesh) continue;
            MVPData mvp; XMStoreFloat4x4(&mvp.Model, XMMatrixTranspose(entity->GetTransform().GetWorldMatrix()));
            XMStoreFloat4x4(&mvp.ViewProjection, XMMatrixTranspose(vp));
            m_MVPBuffer.Update(ctx, mvp); m_MVPBuffer.BindVS(ctx, 0);
            ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            ctx->DrawIndexed(m_IndexBuffer.GetCount(), 0, 0);
        }
        if (m_ShowPhysicsDebug) {
            ctx->ClearDepthStencilView(GetRenderer().GetContext().GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);
            m_Scene.DrawPhysicsDebug(m_DebugRenderer, vp); m_DebugRenderer.Flush(ctx, vp);
        }
        if (auto* selected = m_Scene.GetSelectedEntity()) {
            ctx->ClearDepthStencilView(GetRenderer().GetContext().GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);
            float dist = XMVectorGetX(XMVector3Length(XMVectorSubtract(camPos, XMLoadFloat3(&selected->GetTransform().Position))));
            float scale = dist * 0.15f;
            int modeIdx = (int)m_TransformMode; m_Gizmo.Bind(ctx, modeIdx);
            MVPData mvp; 
            XMMATRIX gizmoModel = XMMatrixScaling(scale, scale, scale) * XMMatrixTranslation(selected->GetTransform().Position.x, selected->GetTransform().Position.y, selected->GetTransform().Position.z);
            XMStoreFloat4x4(&mvp.Model, XMMatrixTranspose(gizmoModel));
            XMStoreFloat4x4(&mvp.ViewProjection, XMMatrixTranspose(vp));
            m_MVPBuffer.Update(ctx, mvp); m_MVPBuffer.BindVS(ctx, 0);
            ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            ctx->DrawIndexed(m_Gizmo.GetIndexCount(modeIdx), 0, 0);
        }
    }

    void OnImGuiRender() override {
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
        if (ImGui::BeginMainMenuBar()) {
            for (auto& category : m_MenuCategories) {
                if (ImGui::BeginMenu(category.Name.c_str())) {
                    for (auto& action : category.Actions) if (ImGui::MenuItem(action->GetLabel())) action->Execute();
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMainMenuBar();
        }

        m_AgentInspector.OnImGuiRender();

        ImGui::Begin("Hierarchy");
        for (auto& e : m_Scene.GetEntities()) {
            bool isSelected = (e.get() == m_Scene.GetSelectedEntity());
            if (ImGui::Selectable(e->GetName().c_str(), isSelected)) m_Scene.SetSelectedEntity(e.get());
        }
        ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsDebug);
        ImGui::End();
        ImGui::Begin("Inspector");
        if (auto* s = m_Scene.GetSelectedEntity()) {
            auto& t = s->GetTransform();
            ImGui::DragFloat3("Position", &t.Position.x, 0.05f);
            float rot[3] = { t.Rotation.x * 57.29f, t.Rotation.y * 57.29f, t.Rotation.z * 57.29f };
            if (ImGui::DragFloat3("Rotation", rot, 1.0f)) { t.Rotation = { rot[0]/57.29f, rot[1]/57.29f, rot[2]/57.29f }; }
            ImGui::DragFloat3("Scale", &t.Scale.x, 0.05f);
            if (auto* rb = s->GetRigidbody()) {
                const char* types[] = { "Static", "Kinematic", "Dynamic" }; int type = (int)rb->Type;
                if (ImGui::Combo("Body Type", &type, types, 3)) rb->Type = (Nova::RigidBodyType)type;
                if (ImGui::Button("Remove Rigidbody")) s->RemoveRigidbody();
            } else if (ImGui::Button("Add Rigidbody")) s->AddRigidbody();
            if (auto* col = s->GetCollider()) {
                const char* types[] = { "None", "Box", "Sphere" }; int type = (int)col->Type;
                if (ImGui::Combo("Collider Type", &type, types, 3)) { col->Type = (Nova::ColliderType)type; col->ColliderPtr = nullptr; }
                if (ImGui::Button("Remove Collider")) s->RemoveCollider();
            } else if (ImGui::Button("Add Collider")) s->AddCollider(Nova::ColliderType::Box);
        }
        ImGui::End();
    }

private:
    Nova::Scene            m_Scene;
    Nova::Shader           m_Shader;
    Nova::VertexBuffer     m_VertexBuffer;
    Nova::IndexBuffer      m_IndexBuffer;
    Nova::ConstantBuffer<MVPData> m_MVPBuffer;
    Nova::ConstantBuffer<LightData> m_LightBuffer;
    Nova::Camera           m_Camera;
    Nova::Entity*          m_CubeEntity = nullptr;
    Nova::Gizmo            m_Gizmo;
    Nova::CommandHistory   m_CommandHistory;
    Nova::DebugRenderer    m_DebugRenderer;
    Nova::AgentInspector   m_AgentInspector;

    GizmoMode      m_GizmoMode = GizmoMode::None;
    TransformMode  m_TransformMode = TransformMode::Position;
    Nova::Transform m_InitialTransform;
    float          m_GizmoDragOffset = 0.0f;
    bool           m_ShowPhysicsDebug = true;
};

Nova::Application* Nova::CreateApplication() { return new EditorApp(); }
