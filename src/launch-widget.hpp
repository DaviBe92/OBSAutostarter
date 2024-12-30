#pragma once

/**
 * @file launch-widget.hpp
 * @brief Dialog widget for selecting and launching application loadouts
 * 
 * Provides a modal dialog that allows users to select which loadout of applications
 * to launch when OBS starts. The dialog includes a dropdown for loadout selection
 * and options to either launch the selected loadout or skip launching entirely.
 */

#include <QtWidgets/QDialog>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <obs-module.h>

/**
 * @brief Dialog widget for loadout selection and launching
 * 
 * Presents a modal dialog with a combobox for loadout selection and
 * Launch/Skip buttons for user interaction.
 */
class LaunchWidget : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs the launch dialog
     * @param parent Parent widget, typically the main OBS window
     */
    explicit LaunchWidget(QWidget *parent = nullptr);
    QComboBox *loadoutCombo; ///< Dropdown for loadout selection

private slots:
    /**
     * @brief Handles the Launch button click
     * Saves the selected loadout and launches its applications
     */
    void onLaunchClicked();

    /**
     * @brief Handles the Skip button click
     * Closes the dialog without launching anything
     */
    void onCancelClicked();

private:
    QLabel *loadoutTitle;
    QPushButton *launchButton;
    QPushButton *cancelButton;
};

/**
 * @brief Creates and shows the launch dialog
 * Populates the loadout list and restores the last selected loadout
 */
void launch_widget_create();

/**
 * @brief Destroys the launch dialog and frees resources
 */
void launch_widget_destroy();
