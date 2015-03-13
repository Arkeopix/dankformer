#ifndef		PLAYER_H_
#define		PLAYER_H_

#include <allegro5/allegro_tiled.h>
#include "game.h"

typedef struct {
	int x;
	int y;
	int speed;
	double velocity_x;
	double velocity_y;
	double friction;
	double gravity;
	bool jumping;
	int bound_y;
	int bound_x;
} player;

void		init_player(player *player, const int x, const int y);
void		player_draw(player *player);
void		player_update(game_loop *gl, player *pl);

#endif		/* !PLAYER_H_*/
