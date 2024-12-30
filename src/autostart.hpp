#pragma once
#include <vector>
#include <string>
#include "config.hpp"

// Forward declare Windows types
using HANDLE = void*;

/**
 * @brief Manages launch and termination of external processes.
 */
class AutoStarter {
public:
    /**
     * @brief Launch all programs from the provided loadout.
     * @param loadoutName The name of the loadout to launch. If empty, uses the current plug-in loadout.
     * @return true on complete success, false if any program failed to launch.
     */
    static bool LaunchPrograms(const std::string &loadoutName = "");

    /**
     * @brief Terminate all previously launched processes.
     * @return true on success, false if any process failed to quit.
     */
    static bool QuitPrograms();

    /**
     * @brief Remove process handles from internal tracking. Used after quitting programs.
     */
    static void ClearProcesses();

private:
    static std::vector<HANDLE> launchedProcesses;

    /**
     * @brief Launch an individual program.
     * @param program Program data containing path, executable, minimized flag.
     * @return true if successfully launched, false on failure.
     */
    static bool LaunchProgram(const Program &program);

    /**
     * @brief Attempt to quit a specific process.
     * @param process Windows process handle.
     * @return true on success, false otherwise.
     */
    static bool QuitProcess(HANDLE process);

    /**
     * @brief Check if a process with the given executable name is already running.
     * @param executableName The filename (e.g., "notepad.exe").
     * @return true if at least one matching process is found, false otherwise.
     */
    static bool IsProcessRunning(const std::string& executableName);
};
