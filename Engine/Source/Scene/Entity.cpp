#include "Scene/Entity.h"
#include "Core/Logger.h"

namespace Nova {

    void Entity::OnCollision(Entity* other) {
        if (other) {
            NOVA_ENGINE_INFO("Collision: Entity '{}' hit '{}'", m_Name, other->GetName());
        } else {
            NOVA_ENGINE_INFO("Collision: Entity '{}' hit world/static object", m_Name);
        }
    }

} // namespace Nova
