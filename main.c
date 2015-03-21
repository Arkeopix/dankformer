#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_tiled.h>
#include "game.h"
#include "player.h"

static void init_game_loop(game_loop *gl) {
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

	gl->map = al_open_map("assets", "untitled1.tmx");
	gl->map_x = 0;
	gl->map_y = 0;
	gl->total_map_width = al_get_map_width(gl->map) * al_get_tile_width(gl->map);
	gl->total_map_height = al_get_map_height(gl->map) * al_get_tile_height(gl->map);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_map_region(gl->map, gl->map_x, gl->map_y, WIDTH, HEIGHT, 0, 0, 0);
	al_flip_display();
	
	gl->done = false;
	gl->redraw = false;
	gl->is_game_over = false;
	memset(&gl->keys, false, sizeof(gl->keys));
	srand(time(NULL));
	al_start_timer(gl->timer);
}

static void process_event_timer(game_loop *gl, player *pl) {
	gl->redraw = true;
	
	player_update(gl, pl);
	player_update_fireball(pl);
	player_collide_fireball(gl, pl);
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
	case ALLEGRO_KEY_A:
		gl->keys[KEY_A] = true;
		player_spawn_fireball(pl);
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
	case ALLEGRO_KEY_A:
		gl->keys[KEY_A] = false;
		break;
	}
}

static void redraw(game_loop *gl, player *pl) {
	gl->redraw = false;

	if (!gl->is_game_over) {
		al_draw_map_region(gl->map, gl->map_x, gl->map_y, WIDTH, HEIGHT, 0, 0, 0);
		player_draw(pl);
		player_draw_fireball(pl);
	}
	al_flip_display();
	al_clear_to_color(al_map_rgb(0, 0, 0));
}

int		main(int argc, char *argv[]) {
	game_loop gl;
	player pl;

	init_player(&pl, WIDTH / 2, HEIGHT / 2);
	init_game_loop(&gl);
	
	while (!gl.done) {
		memset(&gl.ev, 0, sizeof(gl.ev));
		al_wait_for_event(gl.event_queue, &gl.ev);
		if (gl.ev.type == ALLEGRO_EVENT_TIMER) {
			process_event_timer(&gl, &pl);
		} else if (gl.ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			process_event_key_down(&gl, &pl);
		} else if (gl.ev.type == ALLEGRO_EVENT_KEY_UP) {
			process_event_key_up(&gl, &pl);
		}
		
		if (gl.redraw && al_is_event_queue_empty(gl.event_queue)) {
			redraw(&gl, &pl);
		}
	}
	al_destroy_display(gl.display);
	return 0;
}
