#include "config.hpp"
#include <obs-module.h>
#include <util/platform.h>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

PluginConfig PluginConfig::instance;

PluginConfig &PluginConfig::Get()
{
	return instance;
}

QString PluginConfig::GetConfigPath()
{
	char *config_dir = obs_module_config_path("");
	if (!config_dir)
		return QString();

	QString path = QString::fromUtf8(config_dir);
	bfree(config_dir);

	QDir().mkpath(path);
	return path + "config.json";
}

QJsonObject PluginConfig::ToJson() const
{
	QJsonObject json;
	json["enabled"] = enabled;
	json["currentLoadout"] = QString::fromStdString(currentLoadout);
	json["askToLaunch"] = askToLaunch;
	json["autoclose"] = autoclose;

	QJsonArray loadoutsArray;
	for (const auto &loadout : loadouts) {
		QJsonObject loadoutObj;
		loadoutObj["name"] = QString::fromStdString(loadout.name);

		QJsonArray programsArray;
		for (const auto &program : loadout.programs) {
			QJsonObject programObj;
			programObj["path"] =
				QString::fromStdString(program.path);
			programObj["executable"] =
				QString::fromStdString(program.executable);
			programObj["minimized"] = program.minimized;
			programsArray.append(programObj);
		}
		loadoutObj["programs"] = programsArray;

		loadoutsArray.append(loadoutObj);
	}
	json["loadouts"] = loadoutsArray;

	return json;
}

void PluginConfig::FromJson(const QJsonObject &json)
{
	enabled = json["enabled"].toBool(false);
	currentLoadout = json["currentLoadout"].toString().toStdString();
	askToLaunch = json["askToLaunch"].toBool(true);
	autoclose = json["autoclose"].toBool(false);

	loadouts.clear();
	QJsonArray loadoutsArray = json["loadouts"].toArray();
	for (const auto &loadoutVal : loadoutsArray) {
		QJsonObject loadoutObj = loadoutVal.toObject();
		Loadout loadout;
		loadout.name = loadoutObj["name"].toString().toStdString();

		QJsonArray programsArray = loadoutObj["programs"].toArray();
		for (const auto &programVal : programsArray) {
			QJsonObject programObj = programVal.toObject();
			Program program;
			program.path =
				programObj["path"].toString().toStdString();
			program.executable = programObj["executable"]
						     .toString()
						     .toStdString();
			program.minimized = programObj["minimized"].toBool(false);
			loadout.programs.push_back(program);
		}

		loadouts.push_back(loadout);
	}
}

void PluginConfig::Save()
{
	QString configPath = GetConfigPath();
	QFile file(configPath);
	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}

	QJsonObject json = ToJson();
	QJsonDocument doc(json);
	file.write(doc.toJson(QJsonDocument::Indented));
}

void PluginConfig::Load()
{
	QString configPath = GetConfigPath();
	QFile file(configPath);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	QByteArray data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (doc.isObject()) {
		FromJson(doc.object());
	}
}

bool PluginConfig::AddLoadout(const std::string &name)
{
	Loadout loadout;
	loadout.name = name;
	// Check if loadout with the same name already exists
	if (std::find_if(loadouts.begin(), loadouts.end(),
			 [&name](const Loadout &l) {
				 return l.name == name;
			 }) != loadouts.end()) {
		return false;
	}
	loadouts.push_back(loadout);
	return true;
}

void PluginConfig::RemoveLoadout(const std::string &name)
{
	loadouts.erase(std::remove_if(loadouts.begin(), loadouts.end(),
				      [&name](const Loadout &l) {
					      return l.name == name;
				      }),
		       loadouts.end());
}

Loadout *PluginConfig::GetLoadout(const std::string &name)
{
	auto it = std::find_if(loadouts.begin(), loadouts.end(),
			       [&name](const Loadout &l) {
				       return l.name == name;
			       });
	return it != loadouts.end() ? &(*it) : nullptr;
}

void PluginConfig::InitDefaultLoadout()
{
	Loadout loadout;
	loadout.name = "Default";
	loadouts.push_back(loadout);
}
