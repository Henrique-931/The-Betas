#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

int main() {

	al_init();
	al_init_font_addon();
	al_init_primitives_addon();
	al_install_keyboard();

	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);
	al_set_window_title(display, "The Betas");
	ALLEGRO_FONT* fonte = al_create_builtin_font();
	ALLEGRO_TIMER* fps = al_create_timer(1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE* evento = al_create_event_queue();
	al_register_event_source(evento, al_get_display_event_source(display));
	al_register_event_source(evento, al_get_timer_event_source(fps));
	al_start_timer(fps);

	float personagem_x = 400 - 16;
	float personagem_y = 300 - 24;
	float personagem_largura = 32;
	float personagem_altura = 48;
	float velocidade = 3;

	bool rodando = true;
	bool redesenhando = true;

	while (rodando) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(evento, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			ALLEGRO_KEYBOARD_STATE teclado;
			al_get_keyboard_state(&teclado);

			if (al_key_down(&teclado, ALLEGRO_KEY_RIGHT)) personagem_x += velocidade;
			if (al_key_down(&teclado, ALLEGRO_KEY_LEFT)) personagem_x -= velocidade;
			if (al_key_down(&teclado, ALLEGRO_KEY_UP)) personagem_y -= velocidade;
			if (al_key_down(&teclado, ALLEGRO_KEY_DOWN)) personagem_y += velocidade;

			if (personagem_x < 0) personagem_x = 0;
			if (personagem_y < 0) personagem_y = 0;
			if (personagem_x + personagem_largura > 800) personagem_x = 800 - personagem_largura;
			if (personagem_y + personagem_altura > 600) personagem_y = 600 - personagem_altura;

			redesenhando = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			rodando = false;
		}

		if (redesenhando && al_is_event_queue_empty(evento)) {
			redesenhando = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));


			al_draw_filled_rectangle(personagem_x, personagem_y, personagem_x + personagem_largura, personagem_y + personagem_altura, al_map_rgb(80, 160, 220));

			al_flip_display();
		}
	}

	al_destroy_font(fonte);
	al_destroy_timer(fps);
	al_destroy_display(display);
	al_destroy_event_queue(evento);

	return 0;
}