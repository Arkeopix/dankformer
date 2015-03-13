#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "player.h"

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
	player->boundy = 5;
	player->boundx = 5;
}
