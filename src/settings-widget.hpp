#pragma once
#include <QtWidgets/QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>

class SettingsWidget : public QWidget {
	Q_OBJECT
public:
	explicit SettingsWidget(QWidget *parent = nullptr);
	~SettingsWidget();
	static void
	ShowSettings();

private:
	QLabel *titleLabel;
	QCheckBox *enableCheckbox;
	QComboBox *loadoutCombo;
	QListWidget *programsList;
	QPushButton *addProgramButton;
	QPushButton *deleteProgramButton;
	QCheckBox *askToLaunchCheckbox;
	QCheckBox *autocloseCheckbox;
	QPushButton *saveButton;
	QPushButton *closeButton;
	QPushButton *launchButton;
	QLabel *versionLabel;
	QPushButton *quitButton;
	QLabel *socialLabel;
	QLabel *githubLink;
	QLabel *discordLink;
	QLabel *loadoutLabel;
	QHBoxLayout *loadoutLayout;
	QPushButton *addLoadoutButton;
	QPushButton *removeLoadoutButton;

	void UpdateProgramList();

private slots:
	void onSave();
	void onClose();
	void onAddProgram();
	void onDeleteProgram();
	void onLaunchApps();
	void onQuitApps();
	void onAddLoadoutClicked();
	void onRemoveLoadoutClicked();
};

extern SettingsWidget *settings_instance;
