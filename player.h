#ifndef		PLAYER_H_
#define		PLAYER_H_

typedef struct {
	int x;
	int y;
	int speed;
	double velocity_x;
	double velocity_y;
	double friction;
	double gravity;
	bool jumping;
	int boundy;
	int boundx;
} player;

void		init_player(player *player, const int x, const int y);
void		player_draw(player *player);

#endif		/* !PLAYER_H_*/
