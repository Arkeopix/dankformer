#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_tiled.h>
#include <math.h>
#include "game.h"
#include "player.h"

void init_game_loop(game_loop *gl) {
	if (!al_init()) {
		exit(-1);
	}
	if ((gl->display = al_create_display(WIDTH, HEIGHT)) == NULL) {
		exit(-1);
	}
	if (!al_init_image_addon()) {
		exit(-1);
	}
	al_init_primitives_addon();
	al_install_keyboard();
	if ((gl->event_queue = al_create_event_queue()) == NULL) {
		exit(-1);
	}
	if ((gl->timer = al_create_timer(1.0 / FPS)) == NULL) {
		exit(-1);
	}
	al_register_event_source(gl->event_queue, al_get_keyboard_event_source());
	al_register_event_source(gl->event_queue, al_get_timer_event_source(gl->timer));

	gl->done = false;
	gl->redraw = false;
	gl->is_game_over = false;
	memset(&gl->keys[5], false, sizeof(gl->keys));
	srand(time(NULL));
	al_start_timer(gl->timer);
}

bool is_colliding(ALLEGRO_MAP *map, int x, int y) {
	ALLEGRO_MAP_TILE *tile = NULL;
	int tile_x = floor(x / al_get_tile_width(map));
	int tile_y = floor(y / al_get_tile_height(map));
	printf("tile_x = %d, tile_y = %d\n", tile_x, tile_y);
	if ((tile = al_get_single_tile(map, al_get_map_layer(map, "ground"), tile_x, tile_y)) == NULL) {
		return false;
	}
	return true;
}

void collide(player *pl, collision_event event) {
	if (event == FLOOR) {
		pl->velocity_y = 0;
		pl->jumping = false;
	}
	if (event == CEILING) {
		pl->velocity_y = 0;
	}
}

static void process_event_timer(game_loop *gl, player *pl, ALLEGRO_MAP *map, int *map_x, int *map_y, int map_width) {
	gl->redraw = true;
	/* ALLEGRO_MAP_LAYER *layer = al_get_map_layer(map, "ground"); */
	/* ALLEGRO_MAP_TILE *tile = NULL; */
	/* for (int i = 0; i < 43; i++) { */
	/* 	for (int j = 0; j < 34; j++) { */
	/* 		tile = al_get_single_tile(map, layer, i, j); */
	/* 		if (tile != NULL) { */
	/* 			printf("found collision\n"); */
	/* 		} */
	/* 	} */
	/* } */

	if (gl->keys[RIGHT]) {
		if (pl->velocity_x < pl->speed) {
			pl->velocity_x++;
		}
		*map_x += 5;
		if (*map_x > (map_width - WIDTH))
			*map_x = map_width - WIDTH;
	}
	if (gl->keys[LEFT]) {
		if (pl->velocity_x > -pl->speed) {
			pl->velocity_x--;
		}
		*map_x -= 5;
		if (*map_x < 0)
			*map_x = 0;
	}
	if (gl->keys[SPACE]) {
		if (!pl->jumping) {
			pl->jumping = true;
			pl->velocity_y = -10;
		}
	}

	pl->velocity_x *= pl->friction;
	pl->velocity_y += pl->gravity;

	/* collision detection happens here, just before the player pos is updated */

	/* y collision */
	int next_y = pl->y + pl->velocity_y;
	if (pl->velocity_y < 0) {
		if ((!is_colliding(map, pl->x - pl->boundx, next_y - pl->boundy))
			&& (!is_colliding(map, pl->x + pl->boundx - 1, next_y - pl->boundy))) {
			pl->y = next_y;
			pl->jumping = true;
		} else {
			pl->y = next_y + al_get_tile_height(map) - ((next_y - pl->boundy) % al_get_tile_height(map));
			collide(pl, CEILING);
		}
	} else if (pl->velocity_y > 0) {
		if ((!is_colliding(map, pl->x - pl->boundy, next_y + pl->boundy))
			&& (!is_colliding(map, pl->x + pl->boundx - 1, next_y + pl->boundy))) {
			pl->y = next_y;
			pl->jumping = true;
		} else {
			pl->y = next_y - ((next_y + pl->boundy)% al_get_tile_height(map));
			collide(pl, FLOOR);
		}
	}
	
	pl->x += pl->velocity_x;
	//pl->y += pl->velocity_y;

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
	if (pl->y > HEIGHT - pl->boundy) {
		pl->y = HEIGHT - pl->boundy;
		pl->jumping = false;
	}
}

static void process_event_key_down(game_loop *gl, player *pl) {
	switch (gl->ev.keyboard.keycode) {
	case ALLEGRO_KEY_ESCAPE:
		gl->done = true;
		break;
	case ALLEGRO_KEY_UP:
		gl->keys[UP] = true;
		break;
	case ALLEGRO_KEY_DOWN:
		gl->keys[DOWN] = true;
		break;
	case ALLEGRO_KEY_RIGHT:
		gl->keys[RIGHT] = true;
		break;
	case ALLEGRO_KEY_LEFT:
		gl->keys[LEFT] = true;
		break;
	case ALLEGRO_KEY_SPACE:
		gl->keys[SPACE] = true;
		break;
	}
}

static void process_event_key_up(game_loop *gl, player *pl) {
	switch (gl->ev.keyboard.keycode) {
	case ALLEGRO_KEY_UP:
		gl->keys[UP] = false;
		break;
	case ALLEGRO_KEY_DOWN:
		gl->keys[DOWN] = false;
		break;
	case ALLEGRO_KEY_RIGHT:
		gl->keys[RIGHT] = false;
		break;
	case ALLEGRO_KEY_LEFT:
		gl->keys[LEFT] = false;
		break;
	case ALLEGRO_KEY_SPACE:
		gl->keys[SPACE] = false;
		break;
	}
}

static void redraw(game_loop *gl, player *pl, ALLEGRO_MAP *map, int map_x, int map_y) {
	gl->redraw = false;

	if (!gl->is_game_over) {
		al_draw_map_region(map, map_x, map_y, WIDTH, HEIGHT, 0, 0, 0);
		player_draw(pl);
	}
	al_flip_display();
	al_clear_to_color(al_map_rgb(0, 0, 0));
}

int		main(int argc, char *argv[]) {
	game_loop gl;
	player pl;

	ALLEGRO_MAP *map = NULL;
	int map_x = 0, map_y = 0;
	
	init_player(&pl, WIDTH / 2, HEIGHT / 2);
	init_game_loop(&gl);

	map = al_open_map("assets", "untitled1.tmx");
	int map_total_width = al_get_map_width(map) * al_get_tile_width(map);
	int map_total_height = al_get_map_height(map) * al_get_tile_height(map);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_map_region(map, map_x, map_y, WIDTH, HEIGHT, 0, 0, 0);
	al_flip_display();
	
	while (!gl.done) {
		memset(&gl.ev, 0, sizeof(gl.ev));
		al_wait_for_event(gl.event_queue, &gl.ev);
		if (gl.ev.type == ALLEGRO_EVENT_TIMER) {
			process_event_timer(&gl, &pl, map, &map_x, &map_y, map_total_width);
		} else if (gl.ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			process_event_key_down(&gl, &pl);
		} else if (gl.ev.type == ALLEGRO_EVENT_KEY_UP) {
			process_event_key_up(&gl, &pl);
		}
		
		if (gl.redraw && al_is_event_queue_empty(gl.event_queue)) {
			redraw(&gl, &pl, map, map_x, map_y);
		}
	}
	al_destroy_display(gl.display);
	return 0;
}
