#pragma once

#include<iostream>
#include "scene/gui/dialogs.h"
#include "scene/gui/item_list.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/tree.h"
#include <sys/stat.h>
namespace NasK 
{
	
	class PluginGDScriptManager : public Window
	{
	private:
		static constexpr char ROOT_SCENE_FILE_NAME[] = u8"test.tscn";
		static constexpr char ROOT_SCENE_FOLDER_NAME[] = u8"PluginGDSCript";

		GDCLASS(PluginGDScriptManager, Window);
		Ref<PackedScene> root_scene;
		String root_scene_path;
		Control* root_control;
		__int64 root_scene_time_stamp;

		static void _bind_methods();

		
		void close_event();
		bool check_file_exists(const String& path_str);
		bool check_and_create_directory(const String& path_str,bool create_flag = false);
		__int64 get_timestamp(const String& path);
		bool scene_load(const String& path);



	public:	
		PluginGDScriptManager();

	
		void show();


	};
}



