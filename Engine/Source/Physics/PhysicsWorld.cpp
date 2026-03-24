#include "Physics/PhysicsWorld.h"
#include "Scene/Entity.h"
#include "Core/Logger.h"
#include <algorithm>

namespace Nova {

    PhysicsWorld::PhysicsWorld() : m_EventListener(this) {
    }

    PhysicsWorld::~PhysicsWorld() {
        Shutdown();
    }

    void PhysicsWorld::Init() {
        m_World = m_PhysicsCommon.createPhysicsWorld();
        m_World->setGravity(reactphysics3d::Vector3(0, -9.81f, 0));
        
        // Register event listener
        m_World->setEventListener(&m_EventListener);

        // Enable debug rendering for all items
        m_World->setIsDebugRenderingEnabled(true);
        m_World->getDebugRenderer().setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
        m_World->getDebugRenderer().setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_AABB, false);
    }

    void PhysicsWorld::NovaEventListener::onContact(const reactphysics3d::CollisionCallback::CallbackData& callbackData) {
        for (uint32_t p = 0; p < callbackData.getNbContactPairs(); p++) {
            reactphysics3d::CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);
            reactphysics3d::CollisionCallback::ContactPair::EventType eventType = contactPair.getEventType();

            Entity* entity1 = (Entity*)contactPair.getBody1()->getUserData();
            Entity* entity2 = (Entity*)contactPair.getBody2()->getUserData();

            // Notify Entities
            if (eventType == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStart) {
                if (entity1) entity1->OnCollision(entity2);
                if (entity2) entity2->OnCollision(entity1);
            }

            // Notify Observers
            for (auto observer : m_PhysicsWorld->m_Observers) {
                if (eventType == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStart) {
                    observer->OnCollisionStarted(entity1, entity2);
                } else if (eventType == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactExit) {
                    observer->OnCollisionEnded(entity1, entity2);
                }
            }
        }
    }

    void PhysicsWorld::NovaEventListener::onTrigger(const reactphysics3d::OverlapCallback::CallbackData& callbackData) {
        for (uint32_t p = 0; p < callbackData.getNbOverlappingPairs(); p++) {
            reactphysics3d::OverlapCallback::OverlapPair overlapPair = callbackData.getOverlappingPair(p);
            reactphysics3d::OverlapCallback::OverlapPair::EventType eventType = overlapPair.getEventType();

            Entity* entity1 = (Entity*)overlapPair.getBody1()->getUserData();
            Entity* entity2 = (Entity*)overlapPair.getBody2()->getUserData();

            // Notify Entities
            if (eventType == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart) {
                if (entity1) entity1->OnCollision(entity2);
                if (entity2) entity2->OnCollision(entity1);
            }

            // Notify Observers
            for (auto observer : m_PhysicsWorld->m_Observers) {
                if (eventType == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart) {
                    observer->OnTriggerStarted(entity1, entity2);
                } else if (eventType == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapExit) {
                    observer->OnTriggerEnded(entity1, entity2);
                }
            }
        }
    }

    void PhysicsWorld::AddObserver(CollisionObserver* observer) {
        m_Observers.push_back(observer);
    }

    void PhysicsWorld::RemoveObserver(CollisionObserver* observer) {
        auto it = std::find(m_Observers.begin(), m_Observers.end(), observer);
        if (it != m_Observers.end()) m_Observers.erase(it);
    }

    void PhysicsWorld::StepSimulation(float dt) {
        if (!m_World) return;

        m_Accumulator += dt;
        while (m_Accumulator >= m_TimeStep) {
            m_World->update(m_TimeStep);
            m_Accumulator -= m_TimeStep;
        }
    }

    void PhysicsWorld::Shutdown() {
        if (m_World) {
            m_PhysicsCommon.destroyPhysicsWorld(m_World);
            m_World = nullptr;
        }
    }

    void PhysicsWorld::SetDebugRenderingEnabled(bool enabled) {
        if (m_World) m_World->setIsDebugRenderingEnabled(enabled);
    }

    const reactphysics3d::DebugRenderer& PhysicsWorld::GetRP3DDebugRenderer() {
        return m_World->getDebugRenderer();
    }

}
