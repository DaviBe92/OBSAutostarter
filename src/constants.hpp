#pragma once
#include <string>

/**
 * @brief Provides plugin-wide constants such as version, author, and UI labels.
 */
namespace Constants {
    // Version information
    inline const std::string VERSION = "2.0";
    inline const std::string AUTHOR = "Davi Be";
    inline const std::string VERSION_STRING = "Version " + VERSION + " by " + AUTHOR;

    // Social links
    inline const std::string GITHUB_URL = "https://github.com/DaviBe92/Autostarter";

    // Window properties
    inline const int DEFAULT_WINDOW_WIDTH = 400;
    inline const int DEFAULT_WINDOW_HEIGHT = 550;

    // UI text
    inline const std::string WINDOW_TITLE = "Autostarter Settings";
}
