#include "Core/FileUtils.h"
#include <windows.h>
#include <commdlg.h>
#include "Core/Application.h"

namespace Nova {

    std::optional<std::string> FileUtils::OpenFile(const char* filter) {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = (HWND)Application::Get().GetWindow().GetNativeHandle();
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE) {
            return std::string(ofn.lpstrFile);
        }
        return std::nullopt;
    }

    std::optional<std::string> FileUtils::SaveFile(const char* filter) {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = (HWND)Application::Get().GetWindow().GetNativeHandle();
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // Default extension
        ofn.lpstrDefExt = "nova";

        if (GetSaveFileNameA(&ofn) == TRUE) {
            return std::string(ofn.lpstrFile);
        }
        return std::nullopt;
    }

} // namespace Nova
