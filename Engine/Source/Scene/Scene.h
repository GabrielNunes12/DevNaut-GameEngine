#pragma once

#include "Scene/Entity.h"
#include "Physics/PhysicsWorld.h"
#include <vector>
#include <memory>
#include <string>

namespace Nova {

    class Scene {
    public:
        Scene();
        ~Scene() = default;

        void Clear();

        // Entity management
        Entity* AddEntity(const std::string& name, EntityType type = EntityType::Mesh);
        std::shared_ptr<Entity> RemoveEntity(uint32_t id);
        void RestoreEntity(std::shared_ptr<Entity> entity);
        std::shared_ptr<Entity> GetEntityPtr(uint32_t id);

        Entity* FindByName(const std::string& name);
        Entity* FindById(uint32_t id);

        const std::vector<std::shared_ptr<Entity>>& GetEntities() const { return m_Entities; }

        // Physics
        void OnPhysicsUpdate(float dt);
        PhysicsWorld& GetPhysicsWorld() { return m_PhysicsWorld; }

        void DrawPhysicsDebug(class DebugRenderer& debugRenderer, const DirectX::XMMATRIX& viewProjection);

        // Selection
        Entity* GetSelectedEntity() const { return m_SelectedEntity; }
        void    SetSelectedEntity(Entity* entity) { m_SelectedEntity = entity; }

    private:
        std::vector<std::shared_ptr<Entity>> m_Entities;
        uint32_t m_NextId = 1;
        Entity* m_SelectedEntity = nullptr;

        PhysicsWorld m_PhysicsWorld;
    };

} // namespace Nova
