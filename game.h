#ifndef		GAME_H_
#define		GAME_H_

#define  WIDTH  840
#define  HEIGHT  680
#define  FPS 60
#define  NBR_KEYS 6

enum { UP, DOWN, LEFT, RIGHT, SPACE, KEY_A };
typedef enum {FLOOR, CEILING} collision_event;

typedef struct {
	bool done;
	bool redraw;
	bool is_game_over;
	bool keys[NBR_KEYS];
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT ev;
	ALLEGRO_MAP *map;
	int map_x;
	int map_y;
	int total_map_width;
	int total_map_height;
} game_loop;

#endif		/* GAME_H_ */
