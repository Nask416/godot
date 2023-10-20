#include"PluginGDScript.h"
#include "../core/os/os.h"
#include "../servers/display_server.h"
#include "../editor/editor_node.h"
#include "../core/io/resource_loader.h"
#include "../scene/resources/packed_scene.h"
#include "../scene/gui/text_edit.h"
#include "../scene/gui/label.h"
#include <iostream>
#include <filesystem>



void NasK::PluginGDScriptManager::_bind_methods()
{
}

void NasK::PluginGDScriptManager::close_event()
{
	this->hide();
}


bool NasK::PluginGDScriptManager::check_file_exists(const String & path_str)
{
	try
	{
		std::filesystem::path path(path_str.utf8().get_data());
		return std::filesystem::exists(path);
	}
	catch (...)
	{
		return false;
	}
}


bool NasK::PluginGDScriptManager::check_and_create_directory(const String & path_str, bool create_flag)
{
	try
	{
		std::filesystem::path path(path_str.utf8().get_data());
		if (!std::filesystem::exists(path)){
			if(create_flag)std::filesystem::create_directory(path);
		}
	}
	catch (...)
	{
		return false;
	}


	return true;
}

__int64 NasK::PluginGDScriptManager::get_timestamp(const String & path)
{	
	struct _stat64 state;
	if(_stat64(path.utf8().get_data(), &state) == 0)
	{
		return state.st_mtime;
	}
	else
	{
		return 0;
	}
}

bool NasK::PluginGDScriptManager::scene_load(const String & path)
{
	Error err = OK;
	
	Ref<PackedScene> new_scene = ResourceLoader::load(path,"",ResourceFormatLoader::CACHE_MODE_IGNORE,&err);
	
	if(err == OK)
	{
		//現在のsceneを削除
		{
			int count = this->get_child_count();

			for(int i = 0; i < count; i++)
			{
				this->remove_child(this->get_child(i));
			}
		}
		this->root_scene = new_scene;
		this->root_control = (Control*)new_scene.ptr()->instantiate();
		this->root_control->set_visible(true);
		this->set_size(this->root_control->get_size());
		this->root_control->set_position(Point2(0,0));
		this->add_child(this->root_control);
		return true;
	}
	else
	{
		return false;
	}
	

}

NasK::PluginGDScriptManager::PluginGDScriptManager()
{
	this->set_flag(Window::FLAG_RESIZE_DISABLED,true);
	this->set_flag(Window::FLAG_ALWAYS_ON_TOP,true);
	this->set_clamp_to_embedder(true);
	this->set_visible(false);
	this->set_exclusive(true);

	this->set_title(String(L"PluginGDScriptマネージャー"));
	this->connect(u8"close_requested",callable_mp(this,&NasK::PluginGDScriptManager::close_event));
	
	
	std::filesystem::path editor_path(OS::get_singleton()->get_executable_path().utf8().get_data()); 
	String root_folder = String(editor_path.parent_path().c_str()) + u8"/" + this->ROOT_SCENE_FOLDER_NAME;
	this->root_scene_path = root_folder + u8"/" + this->ROOT_SCENE_FILE_NAME;
	
	if(!this->check_and_create_directory(root_folder, true))
	{
		//フォルダ生成エラー
		return;
	}
	
	if(!this->check_file_exists(this->root_scene_path))
	{
		//ルートシーンファイルが存在しない場合のエラー処理
		return;
	}

	//ファイルのタイムスタンプ取得
	this->root_scene_time_stamp = this->get_timestamp(this->root_scene_path);
	//シーンのロード
	
	if(!this->scene_load(this->root_scene_path))
	{
		
		//ルートシーン読み込み失敗のエラー処理
		return;
	}
	
	
	
	OS::get_singleton()->printerr(u8"てすとだよ");

}

void NasK::PluginGDScriptManager::show()
{
	//auto screen_size = DisplayServer::get_singleton()->screen_get_size();
	auto editer_position = EditorNode::get_singleton()->get_window()->get_position();
	auto editer_size = EditorNode::get_singleton()->get_window()->get_size();

	this->set_position(Point2i(editer_position.x + ((editer_size.width * 0.5) - (this->get_size().width * 0.5)),
								editer_position.y + ((editer_size.height * 0.5) - (this->get_size().height * 0.5))));
	
	if(!this->check_file_exists(this->root_scene_path))
	{
		//ルートシーンファイルが存在しない場合のエラー処理
		return;
	}

	__int64 time = this->get_timestamp(this->root_scene_path);


	if(time != this->root_scene_time_stamp)
	{
		//ルートシーンが更新されていた場合再読み込み
		if(!this->scene_load(this->root_scene_path))
		{
			//ルートシーン読み込み失敗のエラー処理
			return;
		}
		this->root_scene_time_stamp = time;
	}
	
	((Window*)this)->show();
        
}



