#pragma once
#include "Command.h"
#include "Scene/Scene.h"
#include <memory>

namespace Nova {

    class DeleteEntityCommand : public Command {
    public:
        DeleteEntityCommand(Scene& scene, std::shared_ptr<Entity> entity)
            : m_Scene(scene), m_Entity(entity) {}

        void Execute() override {
            m_Scene.RemoveEntity(m_Entity->GetId());
        }

        void Undo() override {
            m_Scene.RestoreEntity(m_Entity);
            m_Scene.SetSelectedEntity(m_Entity.get());
        }

    private:
        Scene& m_Scene;
        std::shared_ptr<Entity> m_Entity;
    };

    class AddEntityCommand : public Command {
    public:
        AddEntityCommand(Scene& scene, std::shared_ptr<Entity> entity)
            : m_Scene(scene), m_Entity(entity) {}

        void Execute() override {
            m_Scene.RestoreEntity(m_Entity);
            m_Scene.SetSelectedEntity(m_Entity.get());
        }

        void Undo() override {
            m_Scene.RemoveEntity(m_Entity->GetId());
        }

    private:
        Scene& m_Scene;
        std::shared_ptr<Entity> m_Entity;
    };

}
