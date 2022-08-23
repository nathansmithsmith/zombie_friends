#include "evil_puppy.h"
#include "npc_map.h"
#include "player.h"

void EvilPuppy::main_init(MainData * md) {
	mdata = md;
	
	refresh_images();

	// Config.
	always_updated = true;
}

void EvilPuppy::update() {
	last_call_count++;

	follow_other_char(mdata->player, 2);

	float a_speed = (float)mdata->settings.player_speed * mdata->settings.scale;

	if (dir.right)
		world_x += a_speed;
	if (dir.left)
		world_x -= a_speed;
	if (dir.up)
		world_y -= a_speed;
	if (dir.down)
		world_y += a_speed;

	keep_pos();

	// Slow stuff down.
	if (last_call_count < (int)roundf(mdata->settings.update_fps 
				* mdata->settings.player_update_speed))
		return;
	else
		last_call_count = 0;

	// Update frames.
	if (is_moving()) {
		if (dir.right || (!dir.left && facing_right())) {

			if (frame == EVIL_PUPPY_RIGHT + 1)
				frame = EVIL_PUPPY_RIGHT + 2;
			else
				frame = EVIL_PUPPY_RIGHT + 1;

		} else if (dir.left || (!dir.right && facing_left())) {

			if (frame == EVIL_PUPPY_LEFT + 1)
				frame = EVIL_PUPPY_LEFT + 2;
			else
				frame = EVIL_PUPPY_LEFT + 1;
		}
	} else {
		if (facing_right())
			frame = EVIL_PUPPY_RIGHT;
		else
			frame = EVIL_PUPPY_LEFT;
	}

}

void EvilPuppy::handle_items() {
}

void EvilPuppy::go_right() {
	CommonTool * tl = get_current_tool();

	if (frame == EVIL_PUPPY_LEFT)
		frame = EVIL_PUPPY_RIGHT;

	dir.right = true;
	dir.left = false;

	if (tl == NULL)
		return;

	tl->set_on_side();
	tl->move_to_location();
}

void EvilPuppy::go_left() {
	CommonTool * tl = get_current_tool();

	if (frame == EVIL_PUPPY_RIGHT)
		frame = EVIL_PUPPY_LEFT;

	dir.right = false;
	dir.left = true;

	if (tl == NULL)
		return;

	tl->set_on_side();
	tl->move_to_location();
}

bool EvilPuppy::facing_right() {
	return frame < EVIL_PUPPY_LEFT;
}

bool EvilPuppy::facing_left() {
	return frame >= EVIL_PUPPY_LEFT;
}

void EvilPuppy::refresh_images() {
	images(gameTools::copy_image_list(mdata->scaled_images.npc.evil_puppy));
	refresh_tool_images();
}

void EvilPuppy::keep_pos() {
	x((world_x * mdata->scale_tile_size) + mdata->map->offset_x());
	y((world_y * mdata->scale_tile_size) + mdata->map->offset_y());
}
