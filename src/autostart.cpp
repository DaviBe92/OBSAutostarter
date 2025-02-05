// Reads the autostart config and starts the programs listed in it
#include <windows.h>
#include <TlHelp32.h>
#include "autostart.hpp"
#include "config.hpp"
#include <QString>
#include <obs-module.h>
#include <algorithm>

class ScopedHandle {
    HANDLE handle;
public:
    explicit ScopedHandle(HANDLE h) : handle(h) {}
    ~ScopedHandle() { if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle); }
    HANDLE get() const { return handle; }
    // Prevent copying
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
};

std::vector<HANDLE> AutoStarter::launchedProcesses;

/**
 * @brief Launch all programs defined in a specific loadout.
 */
bool AutoStarter::LaunchPrograms(const std::string &loadoutName)
{
	auto &config = PluginConfig::Get();
	std::string targetLoadout = loadoutName.empty() ? config.currentLoadout
							: loadoutName;

	Loadout *loadout = config.GetLoadout(targetLoadout);
	if (!loadout) {
		blog(LOG_WARNING, "Loadout '%s' not found",
		     targetLoadout.c_str());
		return false;
	}

	bool success = true;
	for (const auto &program : loadout->programs) {
		if (!LaunchProgram(program)) {
			blog(LOG_WARNING, "Failed to launch program: %s/%s",
			     program.path.c_str(), program.executable.c_str());
			success = false;
		}
	}

	return success;
}

/**
 * @brief Checks if any process with the given executable name is running.
 */
bool AutoStarter::IsProcessRunning(const std::string& executableName)
{
    ScopedHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    if (snapshot.get() == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(pe32);

    if (!Process32FirstW(snapshot.get(), &pe32)) {
        return false;
    }

    std::wstring searchName = QString::fromStdString(executableName).toStdWString();
    // Convert to lowercase for case-insensitive comparison
    std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);

    do {
        std::wstring processName = pe32.szExeFile;
        std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
        
        if (processName == searchName) {
            return true;
        }
    } while (Process32NextW(snapshot.get(), &pe32));

    return false;
}

/**
 * @brief Attempts to launch a single program, either as .exe or via ShellExecute for other file types.
 */
bool AutoStarter::LaunchProgram(const Program &program)
{
    // Check if program is already running
    if (IsProcessRunning(program.executable)) {
        blog(LOG_INFO, "Program '%s' is already running, skipping launch",
             program.executable.c_str());
        return true;
    }

	QString fullPath =
		QString::fromStdString(program.path + "/" + program.executable);

	// Check if the program is a exe or a file to open
	std::string extension;
	size_t dotPos = program.executable.find_last_of('.');
	if (dotPos != std::string::npos) {
		extension = program.executable.substr(dotPos);
	}

	if (!extension.empty() && extension != ".exe") {
		// For non-exe files, use ShellExecute
		HINSTANCE result = ShellExecuteA(NULL, "open",
						 fullPath.toStdString().c_str(),
						 NULL, program.path.c_str(),
						 SW_SHOWNORMAL);
		if ((intptr_t)result > 32) {
			blog(LOG_INFO, "Successfully opened file: %s",
			     program.executable.c_str());
			return true;
		}
		blog(LOG_WARNING, "Failed to open file '%s', error code: %d",
		     program.executable.c_str(), (intptr_t)result);
		return false;
	}

    STARTUPINFO si = {sizeof(STARTUPINFO)};

    // check if the minimized flag is set
    if (program.minimized) {
        si.dwFlags = STARTF_USESHOWWINDOW;  // Minimize the window
        si.wShowWindow = SW_SHOWMINIMIZED;  // Minimize the window
    }
    PROCESS_INFORMATION pi;

	std::wstring commandLine = fullPath.toStdWString();

	if (CreateProcess(NULL, // No module name (use command line)
			  (LPWSTR)commandLine.c_str(), // Command line
			  NULL,  // Process handle not inheritable
			  NULL,  // Thread handle not inheritable
			  FALSE, // Set handle inheritance to FALSE
			  0,     // No creation flags
			  NULL,  // Use parent's environment block
			  (LPCWSTR)QString::fromStdString(program.path)
				  .toStdWString()
				  .c_str(), // Use provided path
			  &si,              // Pointer to STARTUPINFO structure
			  &pi) // Pointer to PROCESS_INFORMATION structure
	) {
		CloseHandle(
			pi.hThread); // Close thread handle as we don't need it
		launchedProcesses.push_back(
			pi.hProcess); // Store process handle
		blog(LOG_INFO, "Successfully launched: %s (handle: %p)",
		     program.executable.c_str(), pi.hProcess);
		return true;
	}

	blog(LOG_WARNING, "Failed to launch process '%s', error code: %d",
	     program.executable.c_str(), GetLastError());
	return false;
}

/**
 * @brief Quits all programs previously launched by AutoStarter.
 */
bool AutoStarter::QuitPrograms()
{
	bool success = true;

	for (HANDLE process : launchedProcesses) {
		if (!QuitProcess(process)) {
			success = false;
		}
	}

	ClearProcesses();
	return success;
}

/**
 * @brief Terminates a specific process handle.
 */
bool AutoStarter::QuitProcess(HANDLE process)
{
	if (process == NULL || process == INVALID_HANDLE_VALUE)
		return false;

	// First try to close gracefully
	if (TerminateProcess(process, 0)) {
		CloseHandle(process);
		return true;
	}

	blog(LOG_WARNING,
	     "Failed to terminate process (handle: %p), error code: %d",
	     process, GetLastError());
	CloseHandle(process);
	return false;
}

/**
 * @brief Clears out the internal list of process handles and closes them.
 */
void AutoStarter::ClearProcesses()
{
	for (HANDLE process : launchedProcesses) {
		if (process != NULL && process != INVALID_HANDLE_VALUE) {
			CloseHandle(process);
		}
	}
	launchedProcesses.clear();
}
