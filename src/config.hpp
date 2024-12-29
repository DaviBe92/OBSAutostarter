#pragma once
#include <string>
#include <vector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

struct Program {
	std::string path;
	std::string executable;
};

struct Loadout {
	std::string name;
	std::vector<Program> programs;
};

struct PluginConfig {
	bool enabled = false;
	std::string currentLoadout;
	std::vector<Loadout> loadouts;
	bool askToLaunch = true;
	bool autoclose = false;

	static PluginConfig &Get();
	void Save();
	void Load();

	bool AddLoadout(const std::string &name);
	void RemoveLoadout(const std::string &name);
	Loadout *GetLoadout(const std::string &name);
	void InitDefaultLoadout();

private:
	static PluginConfig instance;
	QString GetConfigPath();
	QJsonObject ToJson() const;
	void FromJson(const QJsonObject &json);
};
