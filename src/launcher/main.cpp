#include "program_data.h"
#include "dll_format.h"
#include "launcher_window.h"
#include "launcher_top_menu.h"
#include "launcher.h"
#include "settings_editor.h"
#include "startup.h"
#include "launcher_option_editor.h"

int main(int argc, char ** argv) {
	fl_register_images();
	MainData mdata;

	mdata.argc = argc;
	mdata.argv = argv;

	// First init.
	startup(&mdata, false);
	mdata.scale_tile_size = 0;

	// Settings editor.
	mdata.settings_editor = new SettingsEditor(&mdata);

	// Launcher window.
	mdata.win = new LauncherWindow(&mdata, Fl::w() / 2, Fl::h() / 2, "Zombie Friends Launcher");

	// Top menu.
	mdata.top_menu = new Fl_Menu_Bar(
		0,
		0,
		mdata.win->w(),
		mdata.settings.menu.height
	);

	// Launcher option editor.
	mdata.launcher_option_editor = new LauncherOptionEditor(
		&mdata,
		0,
		mdata.top_menu->h() + mdata.top_menu->y(),
		mdata.win->w(),
		mdata.win->h() - mdata.top_menu->h()
	);

	// Add menu items after creating everything just in case something is null.
	add_launcher_menu_items(mdata.top_menu, &mdata);

	mdata.win->end();
	mdata.win->resizable(mdata.win);
	mdata.win->show();

	return Fl::run();
}
