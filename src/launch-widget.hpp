#pragma once

#include <QtWidgets/QDialog>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <obs-module.h>

class LaunchWidget : public QDialog {
	Q_OBJECT

public:
	explicit LaunchWidget(QWidget *parent = nullptr);
	QComboBox *loadoutCombo;

private slots:
	void onLaunchClicked();
	void onCancelClicked();

private:
	QLabel *loadoutTitle;
	QPushButton *launchButton;
	QPushButton *cancelButton;
};

void launch_widget_create();
void launch_widget_destroy();
