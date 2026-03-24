#include "Scene/Scene.h"
#include "Renderer/DebugRenderer.h"
#include "Core/Logger.h"
#include <algorithm>
#include <reactphysics3d/reactphysics3d.h>

namespace Nova {

    Scene::Scene() {
        m_PhysicsWorld.Init();
    }

    void Scene::Clear() {
        // Destroy all physics bodies
        for (auto& entity : m_Entities) {
            RigidbodyComponent* rb = entity->GetRigidbody();
            if (rb && rb->Body) {
                m_PhysicsWorld.GetWorld()->destroyRigidBody(rb->Body);
                rb->Body = nullptr;
            }
        }
        m_Entities.clear();
        m_SelectedEntity = nullptr;
        m_NextId = 1;
        NOVA_ENGINE_INFO("Scene cleared");
    }

    Entity* Scene::AddEntity(const std::string& name, EntityType type) {
        auto entity = std::make_shared<Entity>(m_NextId++, name, type);
        m_Entities.push_back(entity);
        NOVA_ENGINE_INFO("Scene: Added entity '{}' (id={}, type={})", name, entity->GetId(), (int)type);
        return entity.get();
    }

    std::shared_ptr<Entity> Scene::RemoveEntity(uint32_t id) {
        auto it = std::find_if(m_Entities.begin(), m_Entities.end(),
            [id](const std::shared_ptr<Entity>& e) { return e->GetId() == id; });

        if (it != m_Entities.end()) {
            auto ptr = *it;
            // Clear selection if removing selected entity
            if (m_SelectedEntity && m_SelectedEntity->GetId() == id) {
                m_SelectedEntity = nullptr;
            }
            // Destroy physics body if it exists
            RigidbodyComponent* rb = ptr->GetRigidbody();
            if (rb && rb->Body) {
                m_PhysicsWorld.GetWorld()->destroyRigidBody(rb->Body);
                rb->Body = nullptr;
            }

            m_Entities.erase(it);
            return ptr;
        }
        return nullptr;
    }

    void Scene::RestoreEntity(std::shared_ptr<Entity> entity) {
        if (entity) m_Entities.push_back(entity);
    }

    std::shared_ptr<Entity> Scene::GetEntityPtr(uint32_t id) {
        auto it = std::find_if(m_Entities.begin(), m_Entities.end(),
            [id](const std::shared_ptr<Entity>& e) { return e->GetId() == id; });
        return it != m_Entities.end() ? *it : nullptr;
    }

    Entity* Scene::FindByName(const std::string& name) {
        for (auto& e : m_Entities) {
            if (e->GetName() == name) return e.get();
        }
        return nullptr;
    }

    Entity* Scene::FindById(uint32_t id) {
        for (auto& e : m_Entities) {
            if (e->GetId() == id) return e.get();
        }
        return nullptr;
    }

    void Scene::OnPhysicsUpdate(float dt) {
        // First, reconcile physics bodies
        for (auto& entity : m_Entities) {
            RigidbodyComponent* rb = entity->GetRigidbody();
            ColliderComponent* col = entity->GetCollider();

            if (rb || col) {
                // Determine if we need a body. For now, RigidbodyComponent is required for physics.
                if (rb) {
                    if (!rb->Body) {
                        const Transform& t = entity->GetTransform();
                        reactphysics3d::Vector3 pos(t.Position.x, t.Position.y, t.Position.z);
                        reactphysics3d::Quaternion rot = reactphysics3d::Quaternion::identity();
                        reactphysics3d::Transform pt(pos, rot);

                        rb->Body = m_PhysicsWorld.GetWorld()->createRigidBody(pt);
                        rb->Body->setUserData(entity.get());
                        NOVA_ENGINE_INFO("Created Physics Body for entity '{}'", entity->GetName());
                    }

                    // Always sync type and mass
                    reactphysics3d::BodyType expectedType = reactphysics3d::BodyType::DYNAMIC;
                    if (rb->Type == RigidBodyType::Static) expectedType = reactphysics3d::BodyType::STATIC;
                    else if (rb->Type == RigidBodyType::Kinematic) expectedType = reactphysics3d::BodyType::KINEMATIC;

                    if (rb->Body->getType() != expectedType) {
                        rb->Body->setType(expectedType);
                    }
                    if (rb->Body->getMass() != rb->Mass) {
                        rb->Body->setMass(rb->Mass);
                    }

                    // Handle Colliders
                    if (col && !col->ColliderPtr) {
                        if (col->Type == ColliderType::Box) {
                            reactphysics3d::Vector3 extents(col->BoxExtents[0], col->BoxExtents[1], col->BoxExtents[2]);
                            col->ShapePtr = m_PhysicsWorld.GetPhysicsCommon().createBoxShape(extents);
                        } else if (col->Type == ColliderType::Sphere) {
                            col->ShapePtr = m_PhysicsWorld.GetPhysicsCommon().createSphereShape(col->SphereRadius);
                        }

                        if (col->ShapePtr) {
                            col->ColliderPtr = rb->Body->addCollider(col->ShapePtr, reactphysics3d::Transform::identity());
                            NOVA_ENGINE_INFO("Created Collider for entity '{}'", entity->GetName());
                        }
                    } else if (!col && rb->Body->getNbColliders() > 0) {
                        // TODO: Remove colliders if component removed
                    }
                }
            }
        }

        m_PhysicsWorld.StepSimulation(dt);

        // Sync physics bodies back to entity transforms
        for (auto& entity : m_Entities) {
            RigidbodyComponent* rb = entity->GetRigidbody();
            if (rb && rb->Body) {
                if (rb->Type == RigidBodyType::Dynamic) {
                    const reactphysics3d::Transform& pt = rb->Body->getTransform();
                    const reactphysics3d::Vector3& pos = pt.getPosition();
                    entity->GetTransform().Position = { pos.x, pos.y, pos.z };
                } else {
                    // Static or Kinematic: Push transform from Editor to Physics
                    const Transform& t = entity->GetTransform();
                    reactphysics3d::Vector3 pos(t.Position.x, t.Position.y, t.Position.z);
                    reactphysics3d::Quaternion rot = reactphysics3d::Quaternion::identity();
                    rb->Body->setTransform(reactphysics3d::Transform(pos, rot));
                }
            }
        }
    }

    void Scene::DrawPhysicsDebug(DebugRenderer& debugRenderer, const DirectX::XMMATRIX& viewProjection) {
        const reactphysics3d::DebugRenderer& rd = m_PhysicsWorld.GetRP3DDebugRenderer();
        
        // Lines
        uint32_t numLines = rd.getNbLines();
        if (numLines > 0) {
            const reactphysics3d::DebugRenderer::DebugLine* lines = rd.getLinesArray();
            for (uint32_t i = 0; i < numLines; ++i) {
                const auto& l = lines[i];
                debugRenderer.DrawLine(
                    { l.point1.x, l.point1.y, l.point1.z },
                    { l.point2.x, l.point2.y, l.point2.z },
                    l.color1
                );
            }
        }

        // Triangles (could be used for faces, but lines are enough for wireframe)
        // ...

        // We don't flush here, let the caller flush after all scenes/debug passes
    }

} // namespace Nova
