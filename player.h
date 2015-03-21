#ifndef		PLAYER_H_
#define		PLAYER_H_

#include <allegro5/allegro_tiled.h>
#include "game.h"

#define NBR_SPELL 6

typedef struct {
	int x;
	int y;
	int speed;
	bool live;
	enum {PRIGHT, PLEFT} dir; /* prefixed with P because RIGHT and LEFT seems to be defined elsewhere */
} fireball;

typedef struct {
	int x;
	int y;
	int speed;
	double velocity_x;
	double velocity_y;
	double friction;
	double gravity;
	bool jumping;
	int bound_x;
	int bound_y;
	fireball spells[NBR_SPELL];
} player;

void		init_player(player *player, const int x, const int y);
void		player_draw(player *player);
void		player_update(game_loop *gl, player *pl);
void		init_fireballs(fireball fb[]);
void		player_spawn_fireball(player *pl);
void		player_update_fireball(player *pl);
void		player_draw_fireball(player *pl);
void		player_collide_fireball(game_loop *gl, player *pl);
#endif		/* !PLAYER_H_*/
