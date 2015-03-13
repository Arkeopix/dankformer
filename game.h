#ifndef		GAME_H_
#define		GAME_H_

#define  WIDTH  840
#define  HEIGHT  680
#define  FPS 60

enum { UP, DOWN, LEFT, RIGHT, SPACE };
typedef enum {FLOOR, CEILING} collision_event;

typedef struct {
	bool done;
	bool redraw;
	bool is_game_over;
	bool keys[5];
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT ev;

} game_loop;

void init_game_loop(game_loop *gl);

#endif		/* GAME_H_ */
