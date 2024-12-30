#pragma once
#include <string>

namespace Constants {
    // Version information
    inline const std::string VERSION = "2.0-beta";
    inline const std::string AUTHOR = "Davi Be";
    inline const std::string VERSION_STRING = "Version " + VERSION + " by " + AUTHOR;

    // Social links
    inline const std::string GITHUB_URL = "https://github.com/DaviBe92/OBSAutostarter";
    inline const std::string DISCORD_URL = "https://discord.gg/VmrQQBpPSK";

    // Window properties
    inline const int DEFAULT_WINDOW_WIDTH = 400;
    inline const int DEFAULT_WINDOW_HEIGHT = 550;

    // UI text
    inline const std::string WINDOW_TITLE = "Autostarter Settings";
    inline const std::string SOCIAL_LABEL = "For more Info:";
    inline const std::string GITHUB_TOOLTIP = "For documentation check the GitHub page";
    inline const std::string DISCORD_TOOLTIP = "Join the Discord server";
}
