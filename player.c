#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_tiled.h>
#include <math.h>
#include "game.h"
#include "player.h"

static bool _player_is_colliding(ALLEGRO_MAP *map, int x, int y) {
	ALLEGRO_MAP_TILE *tile = NULL;
	int tile_x = floor(x / al_get_tile_width(map));
	int tile_y = floor(y / al_get_tile_height(map));
	if ((tile = al_get_single_tile(map, al_get_map_layer(map, "ground"), tile_x, tile_y)) == NULL) {
		return false;
	}
	return true;
}

static void _player_collide(player *pl, collision_event event) {
	if (event == FLOOR) {
		pl->velocity_y = 0;
		pl->jumping = false;
	}
	if (event == CEILING) {
		pl->velocity_y = 0;
	}
}


void	player_collide_fireball(game_loop *gl, player *pl) {
	for (int i = 0; i < NBR_SPELL; i++) {
		if (pl->spells[i].live) {
			if (_player_is_colliding(gl->map, pl->spells[i].x, pl->spells[i].y)) {
				pl->spells[i].live = false;
			}
		}
	}
}

void	player_draw_fireball(player *pl) {
	for (int i = 0; i < NBR_SPELL; i++) {
		al_draw_filled_circle(pl->spells[i].x, pl->spells[i].y, 3, al_map_rgb(255,0,0));
	}
}

void 	player_update_fireball(player *pl) {
	for (int i = 0; i < NBR_SPELL; i++) {
		if (pl->spells[i].live) {
			if (pl->spells[i].dir == PRIGHT) {
				pl->spells[i].x += pl->spells[i].speed;
			} else {
				pl->spells[i].x -= pl->spells[i].speed;
			}
			if (pl->spells[i].x > WIDTH
				|| pl->spells[i].x < 0) { 
				pl->spells[i].live = false;
			}
		}
	}
}

void	player_spawn_fireball(player *pl) {
	for (int i = 0; i < NBR_SPELL; i++) {
		if (!pl->spells[i].live) {
			pl->spells[i].x = pl->x;
			pl->spells[i].y = pl->y;
			pl->spells[i].live = true;
			if (pl->velocity_x > 0) {
				pl->spells[i].dir = PRIGHT;
			}
			else {
				pl->spells[i].dir = PLEFT;
			}
			break; /* we break out of the loop because we only want to cast one fireball */
		}
	}
}

void	init_fireballs(fireball fb[]) {
	for (int i = 0; i < NBR_SPELL; i++) {
		fb[i].x = 0;
		fb[i].y = 0;
		fb[i].speed = 10;
		fb[i].live = false;
	}
}

/* 
 * update the player position, taking care of collisions and shits 
 */

void	player_update(game_loop *gl, player *pl) {
	/* 
	 * check direction and update velocity acordingly, 
	 * move the camera with the player
	 */
	if (gl->keys[RIGHT]) {
		if (pl->velocity_x < pl->speed) {
			pl->velocity_x++;
		}
		gl->map_x += 5;
		if (gl->map_x > (gl->total_map_width - WIDTH))
			gl->map_x = gl->total_map_width - WIDTH;
	}
	if (gl->keys[LEFT]) {
		if (pl->velocity_x > -pl->speed) {
			pl->velocity_x--;
		}
		gl->map_x -= 5;
		if (gl->map_x < 0)
			gl->map_x = 0;
	}
	if (gl->keys[SPACE]) {
		if (!pl->jumping) {
			pl->jumping = true;
			pl->velocity_y = -10;
		}
	}

	/*
	 * add a little magic to velocity x and y
	 * friction is for a deceleration effect
	 * gravity is for gravity...
	 */
	pl->velocity_x *= pl->friction;
	pl->velocity_y += pl->gravity;
	
	/* y collision */
	int next_y = pl->y + pl->velocity_y;
	if (pl->velocity_y < 0) { /* if player is ascending */
		if ((!_player_is_colliding(gl->map, pl->x - pl->bound_x, next_y - pl->bound_y))
			&& (!_player_is_colliding(gl->map, pl->x + pl->bound_x - 1, next_y - pl->bound_y))) {
			pl->y = next_y;
			pl->jumping = true;
		} else {
			pl->y = next_y + al_get_tile_height(gl->map) - ((next_y - pl->bound_y) % al_get_tile_height(gl->map));
			_player_collide(pl, CEILING);
		}
	} else if (pl->velocity_y > 0) { /* if player is falling */
		if ((!_player_is_colliding(gl->map, pl->x - pl->bound_y, next_y + pl->bound_y))
			&& (!_player_is_colliding(gl->map, pl->x + pl->bound_x - 1, next_y + pl->bound_y))) {
			pl->y = next_y;
			pl->jumping = true;
		} else {
			pl->y = next_y - ((next_y + pl->bound_y) % al_get_tile_height(gl->map));
			_player_collide(pl, FLOOR);
		}
	}

	/* x collision */
	int next_x = pl->x + pl->velocity_x;
	if (pl->velocity_x > 0) { /* if player is going right */
		if ((!_player_is_colliding(gl->map, next_x + pl->bound_x, pl->y - pl->bound_y))
			&& (!_player_is_colliding(gl->map, next_x + pl->bound_x, pl->y - pl->bound_y - 1))) {
			pl->x = next_x;
		} else {
			pl->x = next_x - ((next_x + pl->bound_x) % al_get_tile_width(gl->map));
		}
	} else if (pl->velocity_x < 0) { /* if player id going left */
		if ((!_player_is_colliding(gl->map, next_x - pl->bound_x, pl->y - pl->bound_y))
			&& (!_player_is_colliding(gl->map, next_x - pl->bound_x, pl->y + pl->bound_y - 1))) {
			pl->x = next_x;
		} else {
			pl->x = next_x + al_get_tile_width(gl->map) - ((next_x - pl->bound_x) % al_get_tile_width(gl->map));
		}
	}

	/*
	 * bounds checking, player must not go out of the map
	 */
	if (pl->x > WIDTH) {
		pl->x = WIDTH;
	}
	if (pl->x < 0) {
		pl->x = 0;
	}
	if (pl->y > HEIGHT) {
		pl->y = HEIGHT;	
	}
	if (pl->y < 0)  {
		pl->y = 0;
	}
	if (pl->y > HEIGHT - pl->bound_y) {
		pl->y = HEIGHT - pl->bound_y;
		pl->jumping = false;
	}
}

void	player_draw(player *player) {
	al_draw_filled_circle(player->x, player->y, 10, al_map_rgb(255, 0, 255));
}

void init_player(player *player, const int x, const int y) {
	player->x = x;
	player->y = y;
	player->speed = 8;
	player->velocity_x = 0;
	player->velocity_y = 0;
	player->friction = 0.9;
	player->gravity = 0.3;
	player->jumping = false;
	player->bound_y = 5;
	player->bound_x = 5;
	init_fireballs(player->spells);
}

