#pragma once

namespace Nova {

    // Base Command interface for the Memento Pattern
    class Command {
    public:
        virtual ~Command() = default;
        virtual void Execute() = 0;
        virtual void Undo() = 0;
    };

}
