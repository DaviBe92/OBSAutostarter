#include "settings-widget.hpp"
#include "config.hpp"
#include "autostart.hpp"
#include "constants.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QDialog>
#include <QMessageBox>
#include <QLineEdit>

ProgramListItem::ProgramListItem(const QString &path, bool minimized,
				 QWidget *parent)
	: QWidget(parent), fullPath(path)
{
	// Create layout for the program item row
	auto layout = new QHBoxLayout(this);
	layout->setContentsMargins(5, 0, 5, 0); // Reduce vertical margins

	QFileInfo fileInfo(path);
	pathLabel = new QLabel(fileInfo.fileName(), this);
	pathLabel->setToolTip(path);
	pathLabel->setAlignment(Qt::AlignVCenter);

	auto minimizedLabel = new QLabel("| Minimized?", this);
	minimizedLabel->setAlignment(Qt::AlignVCenter);

	minimizedBox = new QCheckBox(this);
	minimizedBox->setChecked(minimized);

	layout->addWidget(pathLabel);
	layout->addStretch();
	layout->addWidget(minimizedLabel);
	layout->addWidget(minimizedBox);
}

SettingsWidget *settings_instance = nullptr;

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
	// Initialize and set up the main configuration window layout
	settings_instance = this;
	setWindowTitle(QString::fromStdString(Constants::WINDOW_TITLE));
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

	auto mainLayout = new QVBoxLayout(this);

	enableCheckbox = new QCheckBox("Enable Autostarter", this);
	mainLayout->addWidget(enableCheckbox);

    mainLayout->addSpacing(10);

	loadoutLabel = new QLabel("Loadout:", this);
	mainLayout->addWidget(loadoutLabel);

	loadoutLayout = new QHBoxLayout();
	loadoutCombo = new QComboBox(this);
	addLoadoutButton = new QPushButton("Add", this);
	removeLoadoutButton = new QPushButton("Remove", this);
	addLoadoutButton->setMaximumWidth(60);
	removeLoadoutButton->setMaximumWidth(80);

	loadoutLayout->addWidget(loadoutCombo);
	loadoutLayout->addWidget(addLoadoutButton);
	loadoutLayout->addWidget(removeLoadoutButton);
	mainLayout->addLayout(loadoutLayout);

	auto &config = PluginConfig::Get();
	for (const auto &loadout : config.loadouts) {
		loadoutCombo->addItem(QString::fromStdString(loadout.name));
	}

	connect(loadoutCombo,
		QOverload<int>::of(&QComboBox::currentIndexChanged), this,
		&SettingsWidget::UpdateProgramList);
	connect(addLoadoutButton, &QPushButton::clicked, this,
		&SettingsWidget::onAddLoadoutClicked);
	connect(removeLoadoutButton, &QPushButton::clicked, this,
		&SettingsWidget::onRemoveLoadoutClicked);

	programsList = new QListWidget(this);
	mainLayout->addWidget(programsList);

	auto buttonLayout = new QHBoxLayout();
	addProgramButton = new QPushButton("Add", this);
	deleteProgramButton = new QPushButton("Remove", this);
	buttonLayout->addWidget(addProgramButton);
	buttonLayout->addWidget(deleteProgramButton);
	mainLayout->addLayout(buttonLayout);

    mainLayout->addSpacing(10);

	auto checkboxLayout = new QHBoxLayout();
	askToLaunchCheckbox = new QCheckBox("Ask on launch", this);
	autocloseCheckbox = new QCheckBox("Autoclose (only for .exe)", this);
	checkboxLayout->addWidget(askToLaunchCheckbox);
	checkboxLayout->addWidget(autocloseCheckbox);
	mainLayout->addLayout(checkboxLayout);

    mainLayout->addSpacing(10);

	auto launchLayout = new QHBoxLayout();
	launchButton = new QPushButton("Launch Apps", this);
	quitButton = new QPushButton("Quit Apps", this);
	launchLayout->addWidget(launchButton);
	launchLayout->addWidget(quitButton);
	mainLayout->addLayout(launchLayout);

	auto actionLayout = new QHBoxLayout();
	saveButton = new QPushButton("Save", this);
	closeButton = new QPushButton("Close", this);
	actionLayout->addWidget(saveButton);
	actionLayout->addWidget(closeButton);
	mainLayout->addLayout(actionLayout);

	mainLayout->addSpacing(10);

	versionLabel = new QLabel(
		"<a href=\"" + QString::fromStdString(Constants::GITHUB_URL) + "\">" + 
		QString::fromStdString(Constants::VERSION_STRING) + "</a>", this);
	versionLabel->setOpenExternalLinks(true);
	versionLabel->setCursor(Qt::PointingHandCursor);
	mainLayout->addWidget(versionLabel);

	connect(saveButton, &QPushButton::clicked, this,
		&SettingsWidget::onSave);
	connect(closeButton, &QPushButton::clicked, this,
		&SettingsWidget::onClose);
	connect(addProgramButton, &QPushButton::clicked, this,
		&SettingsWidget::onAddProgram);
	connect(deleteProgramButton, &QPushButton::clicked, this,
		&SettingsWidget::onDeleteProgram);
	connect(launchButton, &QPushButton::clicked, this,
		&SettingsWidget::onLaunchApps);
	connect(quitButton, &QPushButton::clicked, this,
		&SettingsWidget::onQuitApps);

	// Load initial values from config
	enableCheckbox->setChecked(config.enabled);
	askToLaunchCheckbox->setChecked(config.askToLaunch);
	autocloseCheckbox->setChecked(config.autoclose);

	if (!config.loadouts.empty()) {
		if (config.currentLoadout.empty()) {
			config.currentLoadout = config.loadouts[0].name;
		}
		loadoutCombo->setCurrentText(
			QString::fromStdString(config.currentLoadout));
		UpdateProgramList();
	}

	resize(Constants::DEFAULT_WINDOW_WIDTH,
	       Constants::DEFAULT_WINDOW_HEIGHT);
}

SettingsWidget::~SettingsWidget()
{
	settings_instance = nullptr;
}

void SettingsWidget::ShowSettings()
{
	if (!settings_instance) {
		auto widget = new SettingsWidget();
		widget->setAttribute(Qt::WA_DeleteOnClose);
		widget->show();
		widget->raise();
		widget->activateWindow();
	} else {
		settings_instance->show();
		settings_instance->raise();
		settings_instance->activateWindow();
	}
}

void SettingsWidget::onSave()
{
	auto &config = PluginConfig::Get();
	config.enabled = enableCheckbox->isChecked();
	config.askToLaunch = askToLaunchCheckbox->isChecked();
	config.autoclose = autocloseCheckbox->isChecked();
	config.currentLoadout = loadoutCombo->currentText().toStdString();

	if (Loadout *loadout = config.GetLoadout(
		    loadoutCombo->currentText().toStdString())) {
		for (int i = 0; i < programsList->count(); i++) {
			auto item = programsList->item(i);
			auto widget = qobject_cast<ProgramListItem *>(
				programsList->itemWidget(item));
			if (widget) {
				QFileInfo fileInfo(widget->getPath());
				for (auto &program : loadout->programs) {
					if (program.path ==
						    fileInfo.absolutePath()
							    .toStdString() &&
					    program.executable ==
						    fileInfo.fileName()
							    .toStdString()) {
						program.minimized =
							widget->isMinimized();
						break;
					}
				}
			}
		}
	}

	config.Save();
	close();
}

void SettingsWidget::onClose()
{
	close();
}

void SettingsWidget::onAddProgram()
{
	// Let user select a file, then create a new program entry in the current loadout
    QString filename = QFileDialog::getOpenFileName(this, "Select Program", "", "All Files (*.*)");
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filename);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        QMessageBox::warning(this, "Error", "Selected file does not exist or is not readable");
        return;
    }

    auto &config = PluginConfig::Get();
    if (Loadout *loadout = config.GetLoadout(loadoutCombo->currentText().toStdString())) {
        Program program;
        program.path = fileInfo.absolutePath().toStdString();
        program.executable = fileInfo.fileName().toStdString();
        program.minimized = false;
        loadout->programs.push_back(program);

        auto item = new QListWidgetItem(programsList);
        auto widget = new ProgramListItem(filename, false, programsList);
        item->setSizeHint(QSize(item->sizeHint().width(), 30)); // Set consistent height
        programsList->setItemWidget(item, widget);
    } else {
        QMessageBox::warning(this, "Error", "No loadout selected");
    }
}

void SettingsWidget::onDeleteProgram()
{
	// Remove the currently selected program from the list
    auto item = programsList->currentItem();
    if (!item) {
        return;
    }

    auto widget = qobject_cast<ProgramListItem*>(programsList->itemWidget(item));
    if (!widget) {
        return;
    }

    QFileInfo fileInfo(widget->getPath());
    auto &config = PluginConfig::Get();
    if (Loadout *loadout = config.GetLoadout(loadoutCombo->currentText().toStdString())) {
        auto &programs = loadout->programs;
        programs.erase(
            std::remove_if(
                programs.begin(), programs.end(),
                [&fileInfo](const Program &p) {
                    return (p.path == fileInfo.absolutePath().toStdString() &&
                           p.executable == fileInfo.fileName().toStdString());
                }),
            programs.end());
    }
    delete item;
}

void SettingsWidget::onLaunchApps()
{
	// Trigger launching apps in the selected loadout
	AutoStarter::LaunchPrograms(loadoutCombo->currentText().toStdString());
}

void SettingsWidget::onQuitApps()
{
	// Trigger quitting launched apps
	AutoStarter::QuitPrograms();
}

void SettingsWidget::UpdateProgramList()
{
	programsList->clear();
	auto &config = PluginConfig::Get();
	if (Loadout *loadout = config.GetLoadout(
		    loadoutCombo->currentText().toStdString())) {
		for (const auto &program : loadout->programs) {
			QString fullPath = QString::fromStdString(
				program.path + "/" + program.executable);
			auto item = new QListWidgetItem(programsList);
			auto widget = new ProgramListItem(
				fullPath, program.minimized, programsList);
			item->setSizeHint(QSize(item->sizeHint().width(),
						30)); // Set consistent height
			programsList->setItemWidget(item, widget);
		}
	}
}

void SettingsWidget::onAddLoadoutClicked()
{
	QDialog dialog(this);
	dialog.setWindowTitle("Add new Loadout");
	dialog.setModal(true);

	auto layout = new QVBoxLayout(&dialog);
	auto nameField = new QLineEdit(&dialog);
	auto label = new QLabel("Enter loadout name:", &dialog);
	auto errorLabel = new QLabel(&dialog);
	errorLabel->setStyleSheet("color: red;");
	errorLabel->hide();

	layout->addWidget(label);
	layout->addWidget(nameField);
	layout->addWidget(errorLabel);

	auto buttonBox = new QHBoxLayout();
	auto addButton = new QPushButton("Add", &dialog);
	auto cancelButton = new QPushButton("Cancel", &dialog);
	buttonBox->addWidget(addButton);
	buttonBox->addWidget(cancelButton);
	layout->addLayout(buttonBox);

	connect(nameField, &QLineEdit::returnPressed, addButton,
		&QPushButton::click);

	connect(addButton, &QPushButton::clicked, &dialog, [&]() {
		if (nameField->text().isEmpty()) {
			errorLabel->setText("Name cannot be empty");
			errorLabel->show();
			return;
		}

		auto &config = PluginConfig::Get();
		if (config.AddLoadout(nameField->text().toStdString())) {
			loadoutCombo->addItem(nameField->text());
			loadoutCombo->setCurrentText(nameField->text());
			dialog.accept();
		} else {
			errorLabel->setText(
				"A loadout with this name already exists");
			errorLabel->show();
		}
	});

	connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

	dialog.exec();
}

void SettingsWidget::onRemoveLoadoutClicked()
{
	QString currentLoadout = loadoutCombo->currentText();
	if (currentLoadout.isEmpty())
		return;

	QMessageBox confirm(this);
	confirm.setWindowTitle("Remove Loadout");
	confirm.setText("Do you really want to remove the current loadout?");
	confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	confirm.setDefaultButton(QMessageBox::Cancel);

	if (confirm.exec() == QMessageBox::Yes) {
		auto &config = PluginConfig::Get();
		config.RemoveLoadout(currentLoadout.toStdString());
		loadoutCombo->removeItem(loadoutCombo->currentIndex());
		// If no loadouts left, create a default one
		if (config.loadouts.empty()) {
			config.InitDefaultLoadout();
			loadoutCombo->addItem(QString::fromStdString(
				config.loadouts[0].name));
		}
	}
}
