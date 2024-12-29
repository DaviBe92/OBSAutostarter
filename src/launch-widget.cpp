#include "launch-widget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include "config.hpp"
#include "autostart.hpp"

static LaunchWidget *widget = nullptr;

LaunchWidget::LaunchWidget(QWidget *parent) : QDialog(parent)
{
	setWindowTitle("OBS Autostarter");
	setFixedSize(300, 150);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	setModal(true); // Block interaction with parent window

	auto mainLayout = new QVBoxLayout(this);

	// Loadout selection in horizontal layout
	auto loadoutLayout = new QHBoxLayout();
	loadoutTitle = new QLabel("Loadout:", this);
	loadoutCombo = new QComboBox(this);
	loadoutLayout->addWidget(loadoutTitle);
	loadoutLayout->addWidget(loadoutCombo);
	mainLayout->addLayout(loadoutLayout);

	// Buttons
	auto buttonLayout = new QHBoxLayout();
	launchButton = new QPushButton("Launch", this);
	cancelButton = new QPushButton("Skip", this);

	buttonLayout->addWidget(launchButton);
	buttonLayout->addWidget(cancelButton);
	mainLayout->addLayout(buttonLayout);

	connect(launchButton, &QPushButton::clicked, this,
		&LaunchWidget::onLaunchClicked);
	connect(cancelButton, &QPushButton::clicked, this,
		&LaunchWidget::onCancelClicked);
}

void LaunchWidget::onLaunchClicked()
{
	// Save selected loadout as current
	auto &config = PluginConfig::Get();
	config.currentLoadout = loadoutCombo->currentText().toStdString();
	config.Save();
	// Launch the applications
	AutoStarter::LaunchPrograms(loadoutCombo->currentText().toStdString());
	accept();
}

void LaunchWidget::onCancelClicked()
{
	reject();
}

void launch_widget_create()
{
	if (!widget) {
		widget = new LaunchWidget();
		// Load loadouts
		auto &config = PluginConfig::Get();
		for (const auto &loadout : config.loadouts) {
			widget->loadoutCombo->addItem(
				QString::fromStdString(loadout.name));
		}
		// Get the current loadout
		if (!config.currentLoadout.empty()) {
			widget->loadoutCombo->setCurrentText(
				QString::fromStdString(config.currentLoadout));
		}
		widget->show();
	}
}

void launch_widget_destroy()
{
	delete widget;
	widget = nullptr;
}
