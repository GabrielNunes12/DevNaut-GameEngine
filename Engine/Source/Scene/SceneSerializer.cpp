#include "Scene/SceneSerializer.h"
#include "Core/Logger.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

namespace Nova {

    SceneSerializer::SceneSerializer(Scene& scene) : m_Scene(scene) {}

    void SceneSerializer::Serialize(const std::string& filepath) {
        json j;
        j["SceneName"] = "Untitled";

        auto& entities = m_Scene.GetEntities();
        j["Entities"] = json::array();

        for (auto& entityPtr : entities) {
            Entity* entity = entityPtr.get();
            json e;
            e["Name"] = entity->GetName();
            e["ID"] = entity->GetId();
            e["Type"] = (int)entity->Type;

            // Transform
            const Transform& t = entity->GetTransform();
            e["Transform"] = {
                {"Position", {t.Position.x, t.Position.y, t.Position.z}},
                {"Rotation", {t.Rotation.x, t.Rotation.y, t.Rotation.z}},
                {"Scale",    {t.Scale.x,    t.Scale.y,    t.Scale.z}}
            };

            // Rigidbody
            RigidbodyComponent* rb = entity->GetRigidbody();
            if (rb) {
                e["Rigidbody"] = {
                    {"Type", (int)rb->Type},
                    {"Mass", rb->Mass}
                };
            }

            // Collider
            ColliderComponent* col = entity->GetCollider();
            if (col) {
                json c;
                c["Type"] = (int)col->Type;
                if (col->Type == ColliderType::Box) {
                    c["BoxExtents"] = {col->BoxExtents[0], col->BoxExtents[1], col->BoxExtents[2]};
                } else if (col->Type == ColliderType::Sphere) {
                    c["SphereRadius"] = col->SphereRadius;
                }
                e["Collider"] = c;
            }

            j["Entities"].push_back(e);
        }

        std::ofstream out(filepath);
        out << std::setw(4) << j << std::endl;
        NOVA_ENGINE_INFO("Scene saved to: {}", filepath);
    }

    bool SceneSerializer::Deserialize(const std::string& filepath) {
        std::ifstream in(filepath);
        if (!in.is_open()) return false;

        json j;
        in >> j;

        // Clear current scene
        // (For now, let's just add to it or clear it if the user wants)
        // m_Scene.Clear(); // Need a Clear method

        for (auto& eJson : j["Entities"]) {
            std::string name = eJson["Name"];
            EntityType type = (EntityType)eJson["Type"];
            
            Entity* entity = m_Scene.AddEntity(name, type);
            
            // Transform
            auto& tJson = eJson["Transform"];
            entity->GetTransform().Position = {tJson["Position"][0], tJson["Position"][1], tJson["Position"][2]};
            entity->GetTransform().Rotation = {tJson["Rotation"][0], tJson["Rotation"][1], tJson["Rotation"][2]};
            entity->GetTransform().Scale    = {tJson["Scale"][0],    tJson["Scale"][1],    tJson["Scale"][2]};

            // Rigidbody
            if (eJson.contains("Rigidbody")) {
                auto& rbJson = eJson["Rigidbody"];
                RigidbodyComponent* rb = entity->AddRigidbody();
                rb->Type = (RigidBodyType)rbJson["Type"];
                rb->Mass = rbJson["Mass"];
            }

            // Collider
            if (eJson.contains("Collider")) {
                auto& colJson = eJson["Collider"];
                ColliderType colType = (ColliderType)colJson["Type"];
                ColliderComponent* col = entity->AddCollider(colType);
                if (colType == ColliderType::Box) {
                    col->BoxExtents[0] = colJson["BoxExtents"][0];
                    col->BoxExtents[1] = colJson["BoxExtents"][1];
                    col->BoxExtents[2] = colJson["BoxExtents"][2];
                } else if (colType == ColliderType::Sphere) {
                    col->SphereRadius = colJson["SphereRadius"];
                }
            }
        }

        NOVA_ENGINE_INFO("Scene loaded from: {}", filepath);
        return true;
    }

} // namespace Nova
