#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

int main() {

	al_init();
	al_init_font_addon();
	al_init_primitives_addon();

	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);
	al_set_window_title(display, "The Betas");
	ALLEGRO_FONT* fonte = al_create_builtin_font();
	ALLEGRO_TIMER* fps = al_create_timer(1.0 / 60.0);
	ALLEGRO_EVENT_QUEUE* evento = al_create_event_queue();
	al_register_event_source(evento, al_get_display_event_source(display));
	al_register_event_source(evento, al_get_timer_event_source(fps));
	al_start_timer(fps);

	bool rodando = true;
	bool redesenhando = true;

	while (rodando) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(evento, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redesenhando = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			rodando = false;
		}

		if (redesenhando && al_is_event_queue_empty(evento)) {
			redesenhando = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));


			al_draw_filled_rectangle(0, 0, 10, 10, al_map_rgb(255, 0, 0));
			al_draw_rectangle(20, 20, 50, 50, al_map_rgb(255, 255, 255), 2);

			al_flip_display();
		}
	}

	al_destroy_font(fonte);
	al_destroy_timer(fps);
	al_destroy_display(display);
	al_destroy_event_queue(evento);

	return 0;
}