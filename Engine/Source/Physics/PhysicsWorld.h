#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include <vector>
#include "Physics/CollisionObserver.h"

namespace Nova {
    class PhysicsWorld {
    public:
        PhysicsWorld();
        ~PhysicsWorld();

        void Init();
        void StepSimulation(float dt);
        void Shutdown();

        reactphysics3d::PhysicsCommon& GetPhysicsCommon() { return m_PhysicsCommon; }
        reactphysics3d::PhysicsWorld* GetWorld() { return m_World; }

        void SetDebugRenderingEnabled(bool enabled);
        const reactphysics3d::DebugRenderer& GetRP3DDebugRenderer();

        void AddObserver(CollisionObserver* observer);
        void RemoveObserver(CollisionObserver* observer);

    private:
        class NovaEventListener : public reactphysics3d::EventListener {
        public:
            NovaEventListener(PhysicsWorld* world) : m_PhysicsWorld(world) {}
            virtual void onContact(const reactphysics3d::CollisionCallback::CallbackData& callbackData) override;
            virtual void onTrigger(const reactphysics3d::OverlapCallback::CallbackData& callbackData) override;
        private:
            PhysicsWorld* m_PhysicsWorld;
        };

        reactphysics3d::PhysicsCommon m_PhysicsCommon;
        reactphysics3d::PhysicsWorld* m_World = nullptr;
        NovaEventListener m_EventListener;
        std::vector<CollisionObserver*> m_Observers;
        
        float m_Accumulator = 0.0f;
        const float m_TimeStep = 1.0f / 60.0f;
    };
}
