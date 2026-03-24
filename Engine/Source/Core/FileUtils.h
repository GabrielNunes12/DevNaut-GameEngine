#pragma once
#include <string>
#include <optional>

namespace Nova {

    class FileUtils {
    public:
        // Opens a dialog to select a file for opening.
        // Returns the path if successful, otherwise nullopt.
        // Filter format: "Nova Scene (*.nova)\0*.nova\0All Files (*.*)\0*.*\0"
        static std::optional<std::string> OpenFile(const char* filter);

        // Opens a dialog to select a location for saving a file.
        // Returns the path if successful, otherwise nullopt.
        static std::optional<std::string> SaveFile(const char* filter);
    };

} // namespace Nova
