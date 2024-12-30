#pragma once
#include <vector>
#include <Windows.h>
#include "config.hpp"

class AutoStarter {
public:
	static bool LaunchPrograms(const std::string &loadoutName = "");
	static bool QuitPrograms();
	static void ClearProcesses();

private:
	static std::vector<HANDLE> launchedProcesses;
	static bool LaunchProgram(const Program &program);
	static bool QuitProcess(HANDLE process);
	static bool IsProcessRunning(const std::string& executableName);
};
