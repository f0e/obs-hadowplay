#pragma once

#define CONFIG_AUTOREPLAY_ENABLED "AutoReplayEnabled"
#define CONFIG_AUTOREPLY_BUFFER_STOP_DELAY "AutoReplayBufferStopDelay"
#define CONFIG_AUTOREPLY_RESET_ON_SAVE "AutoReplayResetOnSave"
#define CONFIG_PLAY_NOTIF_SOUND "PlayNotifSound"
#define CONFIG_SHOW_DESKTOP_NOTIF "ShowDesktopNotif"
#define CONFIG_ENABLE_AUTO_ORGANISATION "EnableAutoOrganisation"
#define CONFIG_INCLUDE_SCREENSHOTS "IncludeScreenshots"
#define CONFIG_FOLDER_NAME_AS_PREFIX "FolderNameAsPrefix"
#define CONFIG_EXCLUSION_ITEM_STRING "AppName"
#define CONFIG_EXCLUSIONS "Exclusions"
#define CONFIG_POST_SAVE_SCRIPT "PostSaveScript"
#define CONFIG_POST_SAVE_SCRIPT_PATH "PostSaveScriptPath"

#include <string>
#include <vector>
#include <atomic>
#include <obs.h>

struct Config {

	static Config &Inst();

	Config();

	void Load(obs_data_t *load_data);
	void Save(obs_data_t *save_data);

	void SetDefaults(obs_data_t *data);

	bool m_auto_replay_buffer = true;
	int m_auto_replay_buffer_stop_delay = 0;
	bool m_restart_replay_buffer_on_save = false;
	bool m_enable_auto_organisation = true;
	bool m_folder_name_as_prefix = false;
	bool m_include_screenshots = true;
	bool m_play_notif_sound = true;
	bool m_show_desktop_notif = true;
	bool m_post_save_script = false;
	std::string m_post_save_script_path;
	std::vector<std::string> m_exclusions = std::vector<std::string>();
};
