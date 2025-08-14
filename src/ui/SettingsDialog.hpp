#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#ifdef __cplusplus
#define EXTERNC extern "C"

#include <QDialog>

struct Config;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog();
	~SettingsDialog();

	void ApplyConfig();
	void showEvent(QShowEvent *event);

private slots:
	void add_exclusion_pressed();
	void edit_exclusion_pressed();
	void delete_exclusion_pressed();
	void exceptions_list_selected_changed();
	void button_box_accepted();
	void browse_script_pressed();

private:
	Ui::SettingsDialog *ui;
};
#else
#define EXTERNC
#endif

#endif // SETTINGSDIALOG_H
