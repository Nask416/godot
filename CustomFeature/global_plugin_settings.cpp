#include "global_plugin_settings.h"
#include "../core/os/os.h"
#include "../servers/display_server.h"
#include "../editor/editor_node.h"
#include "../core/io/resource_loader.h"
#include "../scene/resources/packed_scene.h"
#include "../scene/gui/text_edit.h"
#include "../scene/gui/label.h"
#include "../core/variant/variant.h"
#include "../core/io/dir_access.h"
#include "../core/io/file_access.h"
#include "../editor/editor_scale.h"
#include <iostream>
#include <filesystem>



void NasK::GlobalPluginSettings::_bind_methods()
{
}


String NasK::GlobalPluginSettings::get_global_plugin_dir()
{
	return this->plugin_folder;
}


Vector<String> NasK::GlobalPluginSettings::_get_plugins(const String &p_dir) {
	Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_GLOBAL_PLUGIN);
	Error err = da->change_dir(p_dir);
	if (err != OK) {
		return Vector<String>();
	}

	Vector<String> plugins;
	da->list_dir_begin();
	for (String path = da->get_next(); !path.is_empty(); path = da->get_next()) {
		if (path[0] == '.' || !da->current_is_dir()) {
			continue;
		}

		const String full_path = p_dir.path_join(path);
		const String plugin_config = full_path.path_join("plugin.cfg");
		if (FileAccess::exists(plugin_config)) {
			plugins.push_back(plugin_config);
		} else {
			plugins.append_array(_get_plugins(full_path));
		}
	}

	da->list_dir_end();
	return plugins;
}

void NasK::GlobalPluginSettings::_plugin_activity_changed()
{
	if (updating) {
		return;
	}

	TreeItem *ti = plugin_list->get_edited();
	ERR_FAIL_COND(!ti);
	bool active = ti->is_checked(3);
	String name = ti->get_metadata(0);

	EditorNode::get_singleton()->set_addon_plugin_enabled(name, active, true);

	bool is_active = EditorNode::get_singleton()->is_addon_plugin_enabled(name);

	if (is_active != active) {
		updating = true;
		ti->set_checked(3, is_active);
		updating = false;
	}
}

void NasK::GlobalPluginSettings::_plugin_directory_open()
{
	Error err;
	Ref<DirAccess> da = DirAccess::open(this->plugin_folder,&err);

	if (err != OK) {
		err = DirAccess::make_dir_absolute(this->plugin_folder);

		if(err == OK)
		{
			OS::get_singleton()->shell_show_in_file_manager(this->plugin_folder);
			return;
		}
		else
		{
			ERR_FAIL_COND(err != OK);
			return;
		}
	}

	OS::get_singleton()->shell_show_in_file_manager(this->plugin_folder);
}

void NasK::GlobalPluginSettings::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_WM_WINDOW_FOCUS_IN: {
			this->update_plugins();
		} break;

		case Node::NOTIFICATION_READY: {

		} break;
	}
}

void NasK::GlobalPluginSettings::update_plugins()
{
	this->plugin_list->clear();
	updating = true;
	TreeItem *root = this->plugin_list->create_item();

	Vector<String> plugins = this->_get_plugins("gpd://");
	plugins.sort();

	for (int i = 0; i < plugins.size(); i++) {
		Ref<ConfigFile> cf;
		cf.instantiate();
		const String path = plugins[i];

		Error err2 = cf->load(path);

		if (err2 != OK) {
			WARN_PRINT("Can't load plugin config: " + path);
		} else {
			bool key_missing = false;

			if (!cf->has_section_key("plugin", "name")) {
				WARN_PRINT("Plugin config misses \"plugin/name\" key: " + path);
				key_missing = true;
			}
			if (!cf->has_section_key("plugin", "author")) {
				WARN_PRINT("Plugin config misses \"plugin/author\" key: " + path);
				key_missing = true;
			}
			if (!cf->has_section_key("plugin", "version")) {
				WARN_PRINT("Plugin config misses \"plugin/version\" key: " + path);
				key_missing = true;
			}
			if (!cf->has_section_key("plugin", "description")) {
				WARN_PRINT("Plugin config misses \"plugin/description\" key: " + path);
				key_missing = true;
			}
			if (!cf->has_section_key("plugin", "script")) {
				WARN_PRINT("Plugin config misses \"plugin/script\" key: " + path);
				key_missing = true;
			}

			if (!key_missing) {
				String name = cf->get_value("plugin", "name");
				String author = cf->get_value("plugin", "author");
				String version = cf->get_value("plugin", "version");
				String description = cf->get_value("plugin", "description");
				String scr = cf->get_value("plugin", "script");

				const PackedInt32Array boundaries = TS->string_get_word_breaks(description, "", 80);
				String wrapped_description;

				for (int j = 0; j < boundaries.size(); j += 2) {
					const int start = boundaries[j];
					const int end = boundaries[j + 1];
					wrapped_description += "\n" + description.substr(start, end - start + 1).rstrip("\n");
				}

				TreeItem *item = plugin_list->create_item(root);
				item->set_text(0, name);
				item->set_tooltip_text(0, TTR("Name:") + " " + name + "\n" + TTR("Path:") + " " + path + "\n" + TTR("Main Script:") + " " + scr + "\n" + TTR("Description:") + " " + wrapped_description);
				item->set_metadata(0, path);
				item->set_text(1, version);
				item->set_metadata(1, scr);
				item->set_text(2, author);
				item->set_metadata(2, description);
				item->set_cell_mode(3, TreeItem::CELL_MODE_CHECK);
				item->set_text(3, TTR("Enable"));
				bool is_active = EditorNode::get_singleton()->is_addon_plugin_enabled(path);
				item->set_checked(3, is_active);
				item->set_editable(3, true);
				if(!scr.ends_with(".gd"))
				{
					//C#スクリプトなら有効にできないようにブロック
					item->set_editable(3, false);
					item->set_text(3, U"使用不可");
				}
			}
		}
	}

	updating = false;
}

NasK::GlobalPluginSettings::GlobalPluginSettings()
{

	NasK::GlobalPluginSettings::singleton = this;

	String exe_file = *OS::get_singleton()->get_executable_path().rsplit("/").end().operator--();
	
	this->plugin_folder = OS::get_singleton()->get_executable_path().replace(exe_file,"") + this->PLUGIN_FOLDER_NAME;


	HBoxContainer *title_hb = memnew(HBoxContainer);
	Label *l = memnew(Label(TTR("Installed Plugins:")));
	l->set_theme_type_variation("HeaderSmall");
	title_hb->add_child(l);
	title_hb->add_spacer();
	Button *create_plugin = memnew(Button);
	create_plugin->set_text(String(U"フォルダーを開く"));
	create_plugin->connect("pressed", callable_mp(this, &NasK::GlobalPluginSettings::_plugin_directory_open));
	title_hb->add_child(create_plugin);
	add_child(title_hb);

	plugin_list = memnew(Tree);
	plugin_list->set_v_size_flags(SIZE_EXPAND_FILL);
	plugin_list->set_columns(4);
	plugin_list->set_column_titles_visible(true);
	plugin_list->set_column_title(0, TTR("Name"));
	plugin_list->set_column_title(1, TTR("Version"));
	plugin_list->set_column_title(2, TTR("Author"));
	plugin_list->set_column_title(3, TTR("Status"));
	//plugin_list->set_column_title(4, TTR("Edit"));
	plugin_list->set_column_expand(0, true);
	plugin_list->set_column_clip_content(0, true);
	plugin_list->set_column_expand(1, false);
	plugin_list->set_column_clip_content(1, true);
	plugin_list->set_column_expand(2, false);
	plugin_list->set_column_clip_content(2, true);
	plugin_list->set_column_expand(3, false);
	plugin_list->set_column_clip_content(3, true);
	//plugin_list->set_column_expand(4, false);
	//plugin_list->set_column_clip_content(4, true);
	plugin_list->set_column_custom_minimum_width(1, 100 * EDSCALE);
	plugin_list->set_column_custom_minimum_width(2, 250 * EDSCALE);
	plugin_list->set_column_custom_minimum_width(3, 100 * EDSCALE);
	//plugin_list->set_column_custom_minimum_width(4, 40 * EDSCALE);
	plugin_list->set_hide_root(true);
	plugin_list->connect("item_edited", callable_mp(this, &NasK::GlobalPluginSettings::_plugin_activity_changed), CONNECT_DEFERRED);

	VBoxContainer *mc = memnew(VBoxContainer);

	mc->add_child(plugin_list);
	mc->set_v_size_flags(SIZE_EXPAND_FILL);
	mc->set_h_size_flags(SIZE_EXPAND_FILL);

	add_child(mc);

}

