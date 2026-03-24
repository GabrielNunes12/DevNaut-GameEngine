#pragma once
#include "Command.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

namespace Nova {
    
    // Command pattern for modifying Entity Transforms via the Gizmo or Inspector
    class TransformCommand : public Command {
    public:
        TransformCommand(Scene& scene, uint32_t entityId, const Transform& oldTransform, const Transform& newTransform)
            : m_Scene(scene), m_EntityId(entityId), m_OldTransform(oldTransform), m_NewTransform(newTransform) {}

        void Execute() override {
            if (Entity* ent = m_Scene.FindById(m_EntityId)) {
                ent->GetTransform() = m_NewTransform;
            }
        }

        void Undo() override {
            if (Entity* ent = m_Scene.FindById(m_EntityId)) {
                ent->GetTransform() = m_OldTransform;
            }
        }

    private:
        Scene& m_Scene;
        uint32_t m_EntityId;
        Transform m_OldTransform;
        Transform m_NewTransform;
    };

}
