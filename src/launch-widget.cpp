/**
 * @file launch-widget.cpp
 * @brief Implementation of the loadout launch dialog
 * 
 * Implements a modal dialog that appears when OBS starts (if configured)
 * to let users choose which loadout of applications to launch.
 */

#include "launch-widget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include "config.hpp"
#include "autostart.hpp"

static LaunchWidget *widget = nullptr; ///< Singleton instance of the launch dialog

LaunchWidget::LaunchWidget(QWidget *parent) : QDialog(parent)
{
    // Configure dialog properties
    setWindowTitle("OBS Autostarter");
    setFixedSize(300, 150);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setModal(true); // Block interaction with parent window

    // Create and configure layouts
    auto mainLayout = new QVBoxLayout(this);

    // Setup loadout selection controls
    auto loadoutLayout = new QHBoxLayout();
    loadoutTitle = new QLabel("Loadout:", this);
    loadoutCombo = new QComboBox(this);
    loadoutLayout->addWidget(loadoutTitle);
    loadoutLayout->addWidget(loadoutCombo);
    mainLayout->addLayout(loadoutLayout);

    // Setup action buttons
    auto buttonLayout = new QHBoxLayout();
    launchButton = new QPushButton("Launch", this);
    cancelButton = new QPushButton("Skip", this);

    buttonLayout->addWidget(launchButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect button signals to slots
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
    // Create dialog only if it doesn't exist
    if (!widget) {
        widget = new LaunchWidget();
        
        // Populate loadout list from configuration
        auto &config = PluginConfig::Get();
        for (const auto &loadout : config.loadouts) {
            widget->loadoutCombo->addItem(
                QString::fromStdString(loadout.name));
        }

        // Restore previously selected loadout if available
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
