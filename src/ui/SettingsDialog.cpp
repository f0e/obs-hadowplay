#include "SettingsDialog.hpp"
#include "ui_SettingsDialog.h"

#include <obs-frontend-api.h>
#include <obs-module.h>

#include <util/config-file.h>
#include "plugin-support.h"
#include "plugin-platform-helpers.hpp"
#include "config/config.hpp"

SettingsDialog::SettingsDialog() : QDialog(nullptr), ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	// Remove the ? button on dialogs on Windows
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// Set the warning icon
	QSize pixmapSize = QSize(16, 16);
	QIcon icon = style()->standardIcon(QStyle::SP_MessageBoxWarning);
	ui->reset_on_save_info_icon->setPixmap(icon.pixmap(pixmapSize));

	connect(ui->button_box, &QDialogButtonBox::accepted, this,
		&SettingsDialog::button_box_accepted);

	connect(ui->add_exception_button, &QPushButton::pressed, this,
		&SettingsDialog::add_exclusion_pressed);

	connect(ui->edit_exception_button, &QPushButton::pressed, this,
		&SettingsDialog::edit_exclusion_pressed);

	connect(ui->delete_exception_button, &QPushButton::pressed, this,
		&SettingsDialog::delete_exclusion_pressed);

	connect(ui->browse_script_button, &QPushButton::pressed, this,
		&SettingsDialog::browse_script_pressed);

	connect(ui->enable_post_save_script_checkbox, &QCheckBox::stateChanged,
		[this](int state) {
			bool enabled = state == Qt::Checked;
			ui->post_save_script_path->setEnabled(enabled);
			ui->browse_script_button->setEnabled(enabled);
		});

	connect(ui->exceptions_list, &QListWidget::itemSelectionChanged, this,
		&SettingsDialog::exceptions_list_selected_changed);

	exceptions_list_selected_changed();
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::showEvent(QShowEvent *event)
{
	UNUSED_PARAMETER(event);

	ui->automatic_replay_checkbox->setChecked(
		Config::Inst().m_auto_replay_buffer);

	ui->buffer_stop_delay_spinbox->setValue(
		Config::Inst().m_auto_replay_buffer_stop_delay);

	ui->reset_buffer_on_save_checkbox->setChecked(
		Config::Inst().m_restart_replay_buffer_on_save);

	ui->enable_automatic_organisation_checkbox->setChecked(
		Config::Inst().m_enable_auto_organisation);

	ui->include_screenshots_checkbox->setChecked(
		Config::Inst().m_include_screenshots);

	ui->folder_name_as_prefix_checkbox->setChecked(
		Config::Inst().m_folder_name_as_prefix);

	ui->play_notification_sound_checkbox->setChecked(
		Config::Inst().m_play_notif_sound);

	ui->show_desktop_notif_checkbox->setChecked(
		Config::Inst().m_show_desktop_notif);

	bool post_save_script_enabled = Config::Inst().m_post_save_script;

	ui->enable_post_save_script_checkbox->setChecked(
		post_save_script_enabled);
	ui->post_save_script_path->setEnabled(post_save_script_enabled);
	ui->browse_script_button->setEnabled(post_save_script_enabled);

	ui->post_save_script_path->setText(
		QString::fromStdString(Config::Inst().m_post_save_script_path));

	ui->exceptions_list->clear();
	for (size_t i = 0; i < Config::Inst().m_exclusions.size(); ++i) {
		this->ui->exceptions_list->addItem(
			QString::fromStdString(Config::Inst().m_exclusions[i]));
	}
}

extern void obs_hadowplay_replay_buffer_stop();
void SettingsDialog::ApplyConfig()
{
	Config::Inst().m_auto_replay_buffer =
		this->ui->automatic_replay_checkbox->isChecked();

	Config::Inst().m_auto_replay_buffer_stop_delay =
		this->ui->buffer_stop_delay_spinbox->value();

	Config::Inst().m_restart_replay_buffer_on_save =
		this->ui->reset_buffer_on_save_checkbox->isChecked();

	Config::Inst().m_enable_auto_organisation =
		this->ui->enable_automatic_organisation_checkbox->isChecked();

	Config::Inst().m_include_screenshots =
		this->ui->include_screenshots_checkbox->isChecked();

	Config::Inst().m_folder_name_as_prefix =
		this->ui->folder_name_as_prefix_checkbox->isChecked();

	Config::Inst().m_play_notif_sound =
		this->ui->play_notification_sound_checkbox->isChecked();

	Config::Inst().m_show_desktop_notif =
		this->ui->show_desktop_notif_checkbox->isChecked();

	Config::Inst().m_post_save_script =
		this->ui->enable_post_save_script_checkbox->isChecked();

	Config::Inst().m_post_save_script_path =
		this->ui->post_save_script_path->text().toStdString();

	int count = this->ui->exceptions_list->count();

	Config::Inst().m_exclusions.clear();
	for (int i = 0; i < count; ++i) {
		auto item = this->ui->exceptions_list->item(i);
		Config::Inst().m_exclusions.push_back(
			obs_hadowplay_strip_executable_extension(
				item->text().toStdString()));
	}

	obs_hadowplay_replay_buffer_stop();
}

#include <QInputDialog>

void SettingsDialog::add_exclusion_pressed()
{
	bool ok;
	QString text = QInputDialog::getText(
		this, obs_module_text("OBS.Hadowplay.AddExclusion"),
		obs_module_text("OBSHadowplay.Settings.ProcessName"),
		QLineEdit::Normal, QString(), &ok);

	if (ok && text.isEmpty() == false) {
		this->ui->exceptions_list->addItem(text);
	}
}

void SettingsDialog::edit_exclusion_pressed()
{
	if (this->ui->exceptions_list->currentItem() == nullptr)
		return;

	bool ok;
	QString text = QInputDialog::getText(
		this, obs_module_text("OBS.Hadowplay.EditExclusion"),
		obs_module_text("OBSHadowplay.Settings.ProcessName"),
		QLineEdit::Normal,
		this->ui->exceptions_list->currentItem()->text(), &ok);

	if (ok && text.isEmpty() == false) {
		this->ui->exceptions_list->currentItem()->setText(text);
	}
}

void SettingsDialog::delete_exclusion_pressed()
{
	qDeleteAll(this->ui->exceptions_list->selectedItems());
}

void SettingsDialog::exceptions_list_selected_changed()
{
	this->ui->edit_exception_button->setDisabled(
		this->ui->exceptions_list->currentItem() == nullptr);
}

void SettingsDialog::button_box_accepted()
{
	this->ApplyConfig();
}

#include <QFileDialog>

void SettingsDialog::browse_script_pressed()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,
		obs_module_text("OBSHadowplay.Settings.PostSaveScript.Browse"),
		QString(),
#ifdef _WIN32
		obs_module_text("OBSHadowplay.Settings.PostSaveScript.Filter") +
			QString(" (*.bat *.cmd *.exe);;") +
			obs_module_text("All Files") + QString(" (*.*)"));
#else
		obs_module_text("OBSHadowplay.Settings.PostSaveScript.Filter") +
			QString(" (*.sh);;") + obs_module_text("All Files") +
			QString(" (*.*)"));
#endif

	if (!fileName.isEmpty()) {
		ui->post_save_script_path->setText(fileName);
	}
}
