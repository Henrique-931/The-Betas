#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define CELL_W 40
#define CELL_H 60
#define TILE_SIZE 60
#define LINHAS 25
#define COLUNAS 35

#define CHAO 0
#define PAREDE 1
#define PORTA_FECHADA 2

enum { DIR_BAIXO = 0, DIR_CIMA = 1, DIR_ESQUERDA = 2, DIR_DIREITA = 3 };

int mapa[LINHAS][COLUNAS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,2,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1},
    {1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,2,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1},
    {1,2,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,2,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,2,1,0,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
    {1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,2},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void desenhar_mapa(float camera_x, float camera_y, ALLEGRO_BITMAP* tile_parede, ALLEGRO_BITMAP* tile_porta)
{
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            float x = j * TILE_SIZE - camera_x;
            float y = i * TILE_SIZE - camera_y;

            if (mapa[i][j] == PAREDE)
            {
                al_draw_scaled_bitmap(
                    tile_parede,
                    0, 0,
                    al_get_bitmap_width(tile_parede),
                    al_get_bitmap_height(tile_parede),
                    x, y,
                    TILE_SIZE, TILE_SIZE,
                    0
                );
            }
            else if (mapa[i][j] == PORTA_FECHADA)
            {
                al_draw_scaled_bitmap(
                    tile_porta,
                    0, 0,
                    al_get_bitmap_width(tile_porta),
                    al_get_bitmap_height(tile_porta),
                    x, y,
                    TILE_SIZE, TILE_SIZE,
                    0
                );
            }
            else
            {
                al_draw_filled_rectangle(
                    x, y,
                    x + TILE_SIZE, y + TILE_SIZE,
                    al_map_rgb(180, 180, 180)
                );
            }
        }
    }
}

bool pode_andar(float x, float y)
{
    int esquerda = (int)x / TILE_SIZE;
    int direita = (int)(x + CELL_W - 1) / TILE_SIZE;
    int cima = (int)y / TILE_SIZE;
    int baixo = (int)(y + CELL_H - 1) / TILE_SIZE;

    if (esquerda < 0 || direita >= COLUNAS ||
        cima < 0 || baixo >= LINHAS)
    {
        return false;
    }

    if (mapa[cima][esquerda] == PAREDE ||
        mapa[cima][direita] == PAREDE ||
        mapa[baixo][esquerda] == PAREDE ||
        mapa[baixo][direita] == PAREDE)
    {
        return false;
    }

    if (mapa[cima][esquerda] == PORTA_FECHADA ||
        mapa[cima][direita] == PORTA_FECHADA ||
        mapa[baixo][esquerda] == PORTA_FECHADA ||
        mapa[baixo][direita] == PORTA_FECHADA)
    {
        return false;
    }

    return true;
}

void abrir_porta(float personagem_x, float personagem_y)
{
    int coluna = (int)(personagem_x + CELL_W / 2) / TILE_SIZE;
    int linha = (int)(personagem_y + CELL_H / 2) / TILE_SIZE;

    for (int i = linha - 1; i <= linha + 1; i++)
    {
      for (int j = coluna - 1; j <= coluna + 1; j++)
       {
            if (i >= 0 && i < LINHAS &&
                j >= 0 && j < COLUNAS)
            {
              if (mapa[i][j] == PORTA_FECHADA)
                {
                    mapa[i][j] = CHAO;
                    return;
              }
          }
   }
 }
}

int main()
{
    al_init();
    al_init_font_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY* display = al_create_display(1280, 720);

    al_set_window_position(display, 200, 100);
    al_set_window_title(display, "The Betas");

    ALLEGRO_FONT* fonte = al_create_builtin_font();

    ALLEGRO_BITMAP* personagem_sprites =
        al_load_bitmap("personagem_spritesheet.png");

    ALLEGRO_BITMAP* tile_parede =
        al_load_bitmap("tileparede_mapa1.png");

    ALLEGRO_BITMAP* tile_porta =
        al_load_bitmap("porta_mapa.png");

    if (tile_parede == NULL || tile_porta == NULL)
    {
        printf("Erro ao carregar uma das imagens.\n");
        return 1;
    }

    ALLEGRO_TIMER* fps = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* evento = al_create_event_queue();

    al_register_event_source(evento, al_get_display_event_source(display));
    al_register_event_source(evento, al_get_timer_event_source(fps));
    al_register_event_source(evento, al_get_keyboard_event_source());

    al_start_timer(fps);

    float personagem_x = 80;
    float personagem_y = 60;
    float velocidade = 3;

    int direcao = DIR_BAIXO;

    bool andando = false;
    bool rodando = true;
    bool redesenhando = true;

    float camera_x = 0;
    float camera_y = 0;

    while (rodando)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(evento, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            ALLEGRO_KEYBOARD_STATE teclado;
            al_get_keyboard_state(&teclado);

            andando = false;

            float novo_x = personagem_x;
            float novo_y = personagem_y;

            if (al_key_down(&teclado, ALLEGRO_KEY_RIGHT))
            {
                novo_x += velocidade;
                direcao = DIR_DIREITA;
                andando = true;
            }

            if (al_key_down(&teclado, ALLEGRO_KEY_LEFT))
            {
                novo_x -= velocidade;
                direcao = DIR_ESQUERDA;
                andando = true;
            }

            if (pode_andar(novo_x, personagem_y))
            {
                personagem_x = novo_x;
            }

            if (al_key_down(&teclado, ALLEGRO_KEY_UP))
            {
                novo_y -= velocidade;
                direcao = DIR_CIMA;
                andando = true;
            }

            if (al_key_down(&teclado, ALLEGRO_KEY_DOWN))
            {
                novo_y += velocidade;
                direcao = DIR_BAIXO;
                andando = true;
            }

            if (pode_andar(personagem_x, novo_y))
            {
                personagem_y = novo_y;
            }

            if (al_key_down(&teclado, ALLEGRO_KEY_E))
            {
                abrir_porta(personagem_x, personagem_y);
            }

            camera_x = personagem_x + CELL_W / 2 - 1280 / 2;
            camera_y = personagem_y + CELL_H / 2 - 720 / 2;

            if (camera_x < 0)
                camera_x = 0;

            if (camera_y < 0)
                camera_y = 0;

            if (camera_x > COLUNAS * TILE_SIZE - 1280)
                camera_x = COLUNAS * TILE_SIZE - 1280;

            if (camera_y > LINHAS * TILE_SIZE - 720)
                camera_y = LINHAS * TILE_SIZE - 720;

            redesenhando = true;
        }

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            rodando = false;

        if (redesenhando && al_is_event_queue_empty(evento))
        {
            redesenhando = false;

            al_clear_to_color(al_map_rgb(20, 20, 20));

            desenhar_mapa(camera_x, camera_y, tile_parede, tile_porta);

            al_draw_scaled_bitmap(
                personagem_sprites,
                direcao * 52,
                (andando ? 1 : 0) * 96,
                52,
                96,
                personagem_x - camera_x,
                personagem_y - camera_y,
                CELL_W,
                CELL_H,
                0
            );

            al_flip_display();
        }
    }

    al_destroy_bitmap(tile_porta);
    al_destroy_bitmap(tile_parede);
    al_destroy_bitmap(personagem_sprites);
    al_destroy_font(fonte);
    al_destroy_timer(fps);
    al_destroy_display(display);
    al_destroy_event_queue(evento);

    return 0;
}