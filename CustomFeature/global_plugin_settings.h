#pragma once

#ifdef TOOLS_ENABLED

#include<iostream>
#include "scene/gui/dialogs.h"
#include "scene/gui/item_list.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/tree.h"
#include <sys/stat.h>
#include "../editor/editor_plugin_settings.h"

namespace NasK 
{
	
	class GlobalPluginSettings: public VBoxContainer
	{
		GDCLASS(GlobalPluginSettings, VBoxContainer);

	private:
		static constexpr char PLUGIN_FOLDER_NAME[] = u8"global_addons/addons";

		//inline static GlobalPluginSettings *singleton;

		enum {
			BUTTON_PLUGIN_EDIT
		};

		Tree *plugin_list = nullptr;
		bool updating = false;

		void _plugin_activity_changed();
		void _plugin_directory_open();

		static Vector<String> _get_plugins(const String &p_dir);

		inline static String plugin_folder;
		inline static bool init_plugin_folder = false;


	protected:
		void _notification(int p_what);
		static void _bind_methods();

	public:	
		GlobalPluginSettings();
	
		void update_plugins();

		static String get_global_plugin_dir();

		//inline static GlobalPluginSettings *get_singleton() { return GlobalPluginSettings::singleton; }

	};
}

#endif

