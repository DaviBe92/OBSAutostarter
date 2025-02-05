#pragma once
#include <string>
#include <vector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

/**
 * @brief Represents a program that can be launched by the plugin.
 */
struct Program {
    std::string path;        ///< Full path to the program directory
    std::string executable;  ///< Name of the executable file
    bool minimized = false;  ///< Whether to start the program minimized
};

/**
 * @brief Represents a collection of programs that can be launched together.
 */
struct Loadout {
    std::string name;              ///< Unique name of the loadout
    std::vector<Program> programs; ///< List of programs in this loadout
};

/**
 * @brief Manages plugin configurations and loadouts using a singleton pattern.
 * 
 * This class handles saving and loading of plugin configuration, including
 * multiple loadouts of programs that can be launched when OBS starts.
 */
class PluginConfig {
public:
    bool enabled = false;           ///< Whether the plugin is currently enabled
    std::string currentLoadout;     ///< Name of the currently selected loadout
    std::vector<Loadout> loadouts;  ///< List of all available loadouts
    bool askToLaunch = true;        ///< Whether to ask before launching programs
    bool autoclose = false;         ///< Whether to close programs when OBS exits

    /**
     * @brief Retrieves the singleton instance of PluginConfig.
     * @return Reference to the singleton instance.
     */
    static PluginConfig& Get();
    
    /**
     * @brief Saves current configuration to disk in JSON format.
     */
    void Save();

    /**
     * @brief Loads configuration from disk, creates default if none exists.
     */
    void Load();
    
    /**
     * @brief Adds a new loadout with the specified name.
     * @param name The name for the new loadout
     * @return true if added successfully, false if name already exists
     */
    bool AddLoadout(const std::string &name);

    /**
     * @brief Removes a loadout with the specified name.
     * @param name The name of the loadout to remove
     */
    void RemoveLoadout(const std::string &name);

    /**
     * @brief Finds and returns a loadout by name.
     * @param name The name of the loadout to find
     * @return Pointer to the loadout if found, nullptr otherwise
     */
    Loadout *GetLoadout(const std::string &name);

    /**
     * @brief Creates a default loadout configuration.
     */
    void InitDefaultLoadout();

private:
    PluginConfig() = default;
    QString GetConfigPath();
    QJsonObject ToJson() const;
    void FromJson(const QJsonObject &json);

    // Delete copy and move operations
    PluginConfig(const PluginConfig&) = delete;
    PluginConfig& operator=(const PluginConfig&) = delete;
    PluginConfig(PluginConfig&&) = delete;
    PluginConfig& operator=(PluginConfig&&) = delete;
};
