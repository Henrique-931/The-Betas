#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define CELL_W 52
#define CELL_H 96

enum { DIR_BAIXO = 0, DIR_CIMA = 1, DIR_ESQUERDA = 2, DIR_DIREITA = 3 };

int main() {

	al_init();
	al_init_font_addon();
	al_init_primitives_addon();
	al_init_image_addon();
	al_install_keyboard();

	ALLEGRO_DISPLAY* display = al_create_display(800, 600);
	al_set_window_position(display, 200, 200);
	al_set_window_title(display, "The Betas");
	ALLEGRO_FONT* fonte = al_create_builtin_font();
	ALLEGRO_BITMAP* personagem_sprites = al_load_bitmap("personagem_spritesheet.png");
	ALLEGRO_TIMER* fps = al_create_timer(1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE* evento = al_create_event_queue();
	al_register_event_source(evento, al_get_display_event_source(display));
	al_register_event_source(evento, al_get_timer_event_source(fps));
	al_start_timer(fps);

	float personagem_x = 400 - CELL_W / 2;
	float personagem_y = 300 - CELL_H / 2;
	float velocidade = 3;
	int direcao = DIR_BAIXO;
	bool andando = false;

	bool rodando = true;
	bool redesenhando = true;

	while (rodando) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(evento, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			ALLEGRO_KEYBOARD_STATE teclado;
			al_get_keyboard_state(&teclado);

			andando = false;

			if (al_key_down(&teclado, ALLEGRO_KEY_RIGHT)) {
				personagem_x += velocidade;
				direcao = DIR_DIREITA;
				andando = true;
			}
			if (al_key_down(&teclado, ALLEGRO_KEY_LEFT)) {
				personagem_x -= velocidade;
				direcao = DIR_ESQUERDA;
				andando = true;
			}
			if (al_key_down(&teclado, ALLEGRO_KEY_UP)) {
				personagem_y -= velocidade;
				direcao = DIR_CIMA;
				andando = true;
			}
			if (al_key_down(&teclado, ALLEGRO_KEY_DOWN)) {
				personagem_y += velocidade;
				direcao = DIR_BAIXO;
				andando = true;
			}

			if (personagem_x < 0) personagem_x = 0;
			if (personagem_y < 0) personagem_y = 0;
			if (personagem_x + CELL_W > 800) personagem_x = 800 - CELL_W;
			if (personagem_y + CELL_H > 600) personagem_y = 600 - CELL_H;

			redesenhando = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			rodando = false;
		}

		if (redesenhando && al_is_event_queue_empty(evento)) {
			redesenhando = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));

			al_draw_bitmap_region(personagem_sprites, direcao * CELL_W, (andando ? 1 : 0) * CELL_H, CELL_W, CELL_H, personagem_x, personagem_y, 0);

			al_flip_display();
		}
	}

	al_destroy_bitmap(personagem_sprites);
	al_destroy_font(fonte);
	al_destroy_timer(fps);
	al_destroy_display(display);
	al_destroy_event_queue(evento);

	return 0;
}