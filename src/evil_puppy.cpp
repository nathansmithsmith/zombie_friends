#include "evil_puppy.h"
#include "npc_map.h"
#include "player.h"

EvilPuppy::~EvilPuppy() {
	delete path_finder;
}

void EvilPuppy::main_init(MainData * md) {
	mdata = md;
	
	type = NPC_TYPE_EVIL_PUPPY;
	refresh_images();

	// Config.
	always_updated = true;

	path_finder = new Astar::PathFinder(mdata);
	path_finder->set_character(this);

	Astar::PathFinderSettings pathfinder_settings;
	pathfinder_settings.dis_intel_update = 1.0;
	pathfinder_settings.safe_zone_width = 1;
	pathfinder_settings.safe_zone_height = 1;
	pathfinder_settings.character_is_npc = true;
	pathfinder_settings.thread_speed = 1;
	pathfinder_settings.go_around_characters = false;
	pathfinder_settings.try_after_path_failed = false;

	path_finder->set_settings(pathfinder_settings);
	path_finder->start_thread();

	speed = mdata->settings.player_speed / 2.0;

	// Points.
	//load_follow_data_from_file("test_route.nfd");
}

void EvilPuppy::update() {

	if (mdata->player == NULL)
		return;

	handle_hit_data();

	last_call_count++;

	if (should_follow_player)
		path_finder->set_target(mdata->player);
	else
		follow_points();

	update_world_position();
	keep_position();

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

void EvilPuppy::handle_hit_data() {
	int i;
	std::vector<CharacterHitData> hit_data;

	Tile::TileObject curr_tile;
	CommonItem::ItemData curr_item;

	hit_data = get_hit_data();

	if (hit_data.empty())
		return;

	for (i = 0; i < hit_data.size(); i++) {
		curr_tile = mdata->map->tile(hit_data[i].coord.x, hit_data[i].coord.y);
		curr_item = mdata->map->item(hit_data[i].coord.x, hit_data[i].coord.y);

		// Hit already handled.
		if (hit_data[i].hit_handled)
			continue;

		// tile.
		if ((hit_data[i].type & HIT_TILE) == HIT_TILE) {
			hit_data[i].hit_handled = true;

			// Hits wall or something like that.
			if ((hit_data[i].things_hit.tile.type & Tile::TYPE_NO_WALKTHROUGH) == Tile::TYPE_NO_WALKTHROUGH) {
				handle_collision(hit_data[i].coord.x, hit_data[i].coord.y);
			}
		}

		// Character/npc.
		if ((hit_data[i].type & HIT_CHARACTER) == HIT_CHARACTER) {
			hit_data[i].hit_handled = true;
			handle_collision(hit_data[i].things_hit.character);
		}
	}
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
	move_tool_to_location();
}
