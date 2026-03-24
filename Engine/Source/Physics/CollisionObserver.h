#pragma once

namespace Nova {

    class Entity;

    class CollisionObserver {
    public:
        virtual ~CollisionObserver() = default;

        // Called when two entities start colliding
        virtual void OnCollisionStarted(Entity* e1, Entity* e2) = 0;

        // Called when two entities stop colliding
        virtual void OnCollisionEnded(Entity* e1, Entity* e2) = 0;

        // Called when a trigger overlap starts
        virtual void OnTriggerStarted(Entity* e1, Entity* e2) = 0;

        // Called when a trigger overlap ends
        virtual void OnTriggerEnded(Entity* e1, Entity* e2) = 0;
    };

} // namespace Nova
