#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define CELL_W 37
#define CELL_H 55
#define TILE_SIZE 60
#define LINHAS 25
#define COLUNAS 35

#define CHAO 0
#define PAREDE 1
#define PORTA_FECHADA 2

#define QTD_PERGUNTAS 7

#define PROF_COLUNAS 7
#define PROF_LINHAS 4
#define QTD_FALAS 9
#define DISTANCIA_PROFESSOR 110

enum { DIR_BAIXO = 0, DIR_CIMA = 1, DIR_ESQUERDA = 2, DIR_DIREITA = 3 };

typedef struct
{
    char enunciado[100];
    int resposta;
} Pergunta;

Pergunta perguntas[QTD_PERGUNTAS] = {
    {"2x + 7 = 3x - 5", 12},
    {"3(x - 2) + 5 = 2x + 7", 8},
    {"5(x + 2) - 3x = 4x - 10", 10},
    {"x/2 + x/3 = 15", 18},
    {"4(2x - 3) = 3(x + 6) - 5", 5},
    {"7x - 4 = 2x - 24", -4},
    {"(2x - 1)/3 = (x + 4)/2", 14}
};

char falas[QTD_FALAS][200] = {
    "Ola, aluno! Eu sou o Professor. Fique por perto: vou te ensinar o basico para abrir as portas deste labirinto.",
    "Uma equacao do 1o grau tem uma letra, o x, chamada de incognita. Resolver a equacao e descobrir quanto vale esse x.",
    "Pense numa balanca: os dois lados do sinal de = precisam ficar equilibrados. Tudo o que voce fizer de um lado, faca do outro.",
    "Regra pratica: o que esta somando de um lado passa subtraindo para o outro lado. O que esta multiplicando passa dividindo.",
    "Exemplo 1: x + 5 = 12. Passo o 5 subtraindo: x = 12 - 5. Entao x = 7.",
    "Exemplo 2: 3x - 4 = 11. Passo o 4 somando: 3x = 15. O 3 multiplica, entao passa dividindo: x = 15 / 3 = 5.",
    "Com x dos dois lados, junte os x de um lado e os numeros do outro. Ex: 5x - 3 = 2x + 9 -> 3x = 12 -> x = 4.",
    "Com parenteses, multiplique primeiro: 3(x - 2) = 3x - 6. Com fracoes, multiplique tudo pelo denominador comum.",
    "Dica final: depois de achar o x, troque ele na equacao para conferir. Se os dois lados derem igual, voce acertou! Boa sorte!"
};

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
                    tile_parede, 0, 0, al_get_bitmap_width(tile_parede), al_get_bitmap_height(tile_parede), x, y, TILE_SIZE, TILE_SIZE, 0);
            }
            else if (mapa[i][j] == PORTA_FECHADA)
            {
                al_draw_scaled_bitmap(
                    tile_porta, 0, 0, al_get_bitmap_width(tile_porta), al_get_bitmap_height(tile_porta), x, y, TILE_SIZE, TILE_SIZE, 0);
            }
            else
            {
                al_draw_filled_rectangle(
                    x, y, x + TILE_SIZE, y + TILE_SIZE, al_map_rgb(180, 180, 180));
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

void abrir_porta(int linha, int coluna)
{
    if (linha >= 0 && linha < LINHAS &&
        coluna >= 0 && coluna < COLUNAS)
    {
        if (mapa[linha][coluna] == PORTA_FECHADA)
        {
            mapa[linha][coluna] = CHAO;
        }
    }
}

bool clicou(float mx, float my, float x1, float y1, float x2, float y2)
{
    return (mx >= x1 && mx <= x2 && my >= y1 && my <= y2);
}

void desenhar_botao(ALLEGRO_FONT* fonte, float x1, float y1, float x2, float y2, const char* texto)
{
    al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(70, 70, 90));
    al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(220, 220, 220), 2);
    al_draw_text(fonte, al_map_rgb(255, 255, 255),
        (x1 + x2) / 2, (y1 + y2) / 2 - 4, ALLEGRO_ALIGN_CENTER, texto);
}

bool perto_do_professor(float jogador_x, float jogador_y, float prof_x, float prof_y)
{
    float dx = jogador_x - prof_x;
    float dy = jogador_y - prof_y;

    if (dx < 0)
        dx = -dx;

    if (dy < 0)
        dy = -dy;

    return (dx < DISTANCIA_PROFESSOR && dy < DISTANCIA_PROFESSOR);
}

int main()
{
    srand(time(NULL));

    al_init();
    al_init_font_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_mouse();

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

    if (personagem_sprites == NULL || tile_parede == NULL || tile_porta == NULL)
    {
        printf("Erro ao carregar uma das imagens.\n");
        return 1;
    }

    ALLEGRO_BITMAP* prof_sprites = al_load_bitmap("spriteprofessor.png");
    bool tem_professor = (prof_sprites != NULL);
    int prof_quadro_w = 1;
    int prof_quadro_h = 1;

    if (tem_professor)
    {
        prof_quadro_w = al_get_bitmap_width(prof_sprites) / PROF_COLUNAS;
        prof_quadro_h = al_get_bitmap_height(prof_sprites) / PROF_LINHAS;
    }
    else
    {
        printf("Nao achei spriteprofessor.png, o jogo vai abrir sem o professor.\n");
    }

    float prof_h = 55;
    float prof_w = prof_h * prof_quadro_w / prof_quadro_h;
    float professor_x = 11 * TILE_SIZE + 6;
    float professor_y = 11 * TILE_SIZE + 5;

    ALLEGRO_TIMER* fps = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* evento = al_create_event_queue();
    al_register_event_source(evento, al_get_display_event_source(display));
    al_register_event_source(evento, al_get_timer_event_source(fps));
    al_register_event_source(evento, al_get_keyboard_event_source());
    al_register_event_source(evento, al_get_mouse_event_source());
    al_start_timer(fps);

    float personagem_x = 80;
    float personagem_y = 60;
    float velocidade = 3;

    int direcao = DIR_BAIXO;

    bool andando = false;
    bool rodando = true;
    bool redesenhando = true;
    bool perguntando = false;

    char resposta[20] = "";
    int tamanho_resposta = 0;
    int pergunta_atual = 0;
    char mensagem[50] = "";

    int porta_linha = -1;
    int porta_coluna = -1;

    float camera_x = 0;
    float camera_y = 0;

    bool conversando = false;
    int pagina_fala = 0;
    int contador_anim = 0;

    bool caderno_aberto = false;
    int pagina_caderno = 0;
    int falas_lidas = 0;

    while (rodando)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(evento, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN && perguntando && !caderno_aberto)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                perguntando = false;
                resposta[0] = '\0';
                tamanho_resposta = 0;
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_N && conversando == false)
            {
                if (caderno_aberto)
                {
                    caderno_aberto = false;
                }
                else
                {
                    caderno_aberto = true;
                    pagina_caderno = falas_lidas - 1;

                    if (pagina_caderno < 0)
                        pagina_caderno = 0;
                }
            }

            if (caderno_aberto)
            {
                if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT && pagina_caderno > 0)
                    pagina_caderno--;

                if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT && pagina_caderno < falas_lidas - 1)
                    pagina_caderno++;

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    caderno_aberto = false;
            }
        }


        if (ev.type == ALLEGRO_EVENT_KEY_DOWN && !perguntando && tem_professor && !caderno_aberto)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_E)
            {
                if (conversando == false)
                {
                    if (perto_do_professor(personagem_x, personagem_y, professor_x, professor_y))
                    {
                        conversando = true;
                        pagina_fala = 0;
                    }
                }
                else
                {
                    pagina_fala++;

                    if (pagina_fala >= QTD_FALAS)
                        conversando = false;
                }
            }

            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                conversando = false;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && perguntando && !caderno_aberto && ev.mouse.button == 1)
        {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            for (int k = 0; k < 12; k++)
            {
                int coluna_botao = k % 3;
                int linha_botao = k / 3;
                float bx = 380 + coluna_botao * 80;
                float by = 290 + linha_botao * 55;

                if (clicou(mx, my, bx, by, bx + 70, by + 45))
                {
                    mensagem[0] = '\0';

                    if (k == 11)
                    {
                        if (tamanho_resposta > 0)
                        {
                            tamanho_resposta--;
                            resposta[tamanho_resposta] = '\0';
                        }
                    }
                    else if (k == 9)
                    {
                        if (tamanho_resposta == 0)
                        {
                            resposta[0] = '-';
                            resposta[1] = '\0';
                            tamanho_resposta = 1;
                        }
                    }
                    else
                    {
                        if (tamanho_resposta < 6)
                        {
                            if (k < 9)
                                resposta[tamanho_resposta] = '1' + k;
                            else
                                resposta[tamanho_resposta] = '0';

                            tamanho_resposta++;
                            resposta[tamanho_resposta] = '\0';
                        }
                    }
                }
            }

            if (clicou(mx, my, 640, 290, 900, 350))
            {
                if (tamanho_resposta > 0 && !(tamanho_resposta == 1 && resposta[0] == '-')) {
                    if (atoi(resposta) == perguntas[pergunta_atual].resposta)
                    {
                        abrir_porta(porta_linha, porta_coluna);
                        perguntando = false;
                    }
                    else
                    {
                        sprintf_s(mensagem, sizeof(mensagem), "Resposta errada");
                    }

                    resposta[0] = '\0';
                    tamanho_resposta = 0;
                }
            }

            if (clicou(mx, my, 640, 370, 900, 430))
            {
                perguntando = false;
                resposta[0] = '\0';
                tamanho_resposta = 0;
            }
        }

        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            ALLEGRO_KEYBOARD_STATE teclado;
            al_get_keyboard_state(&teclado);

            andando = false;

            float novo_x = personagem_x;
            float novo_y = personagem_y;

            if (!perguntando && !caderno_aberto)
            {
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
                                    porta_linha = i;
                                    porta_coluna = j;

                                    perguntando = true;
                                    pergunta_atual = rand() % QTD_PERGUNTAS;

                                    resposta[0] = '\0';
                                    tamanho_resposta = 0;
                                    mensagem[0] = '\0';
                                }
                            }
                        }
                    }
                }
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

            contador_anim++;

            if (conversando && pagina_fala + 1 > falas_lidas)
                falas_lidas = pagina_fala + 1;

            if (contador_anim >= 30)
                contador_anim = 0;

            if (conversando && !perto_do_professor(personagem_x, personagem_y, professor_x, professor_y))
                conversando = false;

            redesenhando = true;
        }

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            rodando = false;

        if (redesenhando && al_is_event_queue_empty(evento))
        {
            redesenhando = false;

            al_clear_to_color(al_map_rgb(20, 20, 20));

            desenhar_mapa(camera_x, camera_y, tile_parede, tile_porta);

            if (tem_professor)
            {
                int quadro_linha = 0;
                int quadro_coluna = 0;

                if (conversando)
                {
                    quadro_linha = 2;

                    if (contador_anim < 15)
                        quadro_coluna = 0;
                    else
                        quadro_coluna = 1;
                }

                al_draw_scaled_bitmap(
                    prof_sprites, quadro_coluna * prof_quadro_w, quadro_linha * prof_quadro_h, prof_quadro_w, prof_quadro_h, professor_x - camera_x, professor_y - camera_y, prof_w, prof_h, 0);

                if (!conversando && perto_do_professor(personagem_x, personagem_y, professor_x, professor_y))
                {
                    al_draw_text(
                        fonte, al_map_rgb(255, 255, 0), professor_x - camera_x + prof_w / 2, professor_y - camera_y - 14, ALLEGRO_ALIGN_CENTER, "[E] Falar");
                }
            }
            else
            {
                al_draw_text(
                    fonte, al_map_rgb(255, 80, 80), 10, 10, ALLEGRO_ALIGN_LEFT, "spriteprofessor.png nao encontrado");
            }

            al_draw_scaled_bitmap(
                personagem_sprites, direcao * 52, (andando ? 1 : 0) * 96, 52, 96, personagem_x - camera_x, personagem_y - camera_y, CELL_W, CELL_H, 0);

            al_draw_text(
                fonte, al_map_rgb(255, 255, 255), 1270, 10, ALLEGRO_ALIGN_RIGHT, "[N] Caderno");


            if (conversando && !perguntando)
            {
                char numero_pagina[20];

                al_draw_filled_rectangle(
                    140, 570, 1140, 700, al_map_rgb(35, 35, 45));

                al_draw_rectangle(
                    140, 570, 1140, 700, al_map_rgb(220, 220, 220), 2);

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 0), 160, 585, ALLEGRO_ALIGN_LEFT, "PROFESSOR");

                al_draw_multiline_text(
                    fonte, al_map_rgb(255, 255, 255), 160, 610, 960, 14, ALLEGRO_ALIGN_LEFT, falas[pagina_fala]);

                sprintf_s(numero_pagina, sizeof(numero_pagina), "%d/%d", pagina_fala + 1, QTD_FALAS);

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200), 160, 680, ALLEGRO_ALIGN_LEFT, numero_pagina);

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200), 1120, 680, ALLEGRO_ALIGN_RIGHT, "[E] Continuar   [ESC] Fechar");
            }

            if (perguntando)
            {
                al_draw_filled_rectangle(
                    0, 0, 1280, 720, al_map_rgba(0, 0, 0, 180));

                al_draw_filled_rectangle(
                    300, 150, 980, 570, al_map_rgb(35, 35, 45));

                al_draw_rectangle(
                    300, 150, 980, 570, al_map_rgb(220, 220, 220), 2);

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255), 640, 170, ALLEGRO_ALIGN_CENTER, "PORTA BLOQUEADA - RESOLVA A EQUACAO");

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 0), 640, 205, ALLEGRO_ALIGN_CENTER, perguntas[pergunta_atual].enunciado);

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200), 640, 225, ALLEGRO_ALIGN_CENTER, "Qual o valor de x?");

                al_draw_filled_rectangle(
                    380, 240, 900, 280, al_map_rgb(15, 15, 20));
                al_draw_rectangle(
                    380, 240, 900, 280, al_map_rgb(180, 180, 180), 2);
                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255), 640, 256, ALLEGRO_ALIGN_CENTER, resposta);

                const char* rotulos[12] = { "1","2","3","4","5","6","7","8","9","-","0","<" };

                for (int k = 0; k < 12; k++)
                {
                    int coluna_botao = k % 3;
                    int linha_botao = k / 3;
                    float bx = 380 + coluna_botao * 80;
                    float by = 290 + linha_botao * 55;

                    desenhar_botao(fonte, bx, by, bx + 70, by + 45, rotulos[k]);
                }

                desenhar_botao(fonte, 640, 290, 900, 350, "CONFIRMAR");
                desenhar_botao(fonte, 640, 370, 900, 430, "FECHAR");

                al_draw_text(
                    fonte, al_map_rgb(255, 80, 80), 640, 530, ALLEGRO_ALIGN_CENTER, mensagem);
            }

            if (caderno_aberto)
            {
                char titulo_nota[40];

                al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(0, 0, 0, 150));

                al_draw_filled_rectangle(240, 100, 1040, 620, al_map_rgb(245, 235, 200));

                al_draw_rectangle(240, 100, 1040, 620, al_map_rgb(110, 70, 30), 4);

                al_draw_text(fonte, al_map_rgb(110, 70, 30), 640, 120, ALLEGRO_ALIGN_CENTER, "CADERNO DE ANOTACOES");

                al_draw_line(280, 145, 1000, 145, al_map_rgb(110, 70, 30), 2);

                if (falas_lidas == 0)
                {
                    al_draw_multiline_text(fonte, al_map_rgb(40, 30, 20), 280, 190, 720, 16, ALLEGRO_ALIGN_LEFT,
                        "Voce ainda nao conversou com o professor. Chegue perto dele e aperte E: tudo o que ele explicar vai aparecer aqui.");
                }
                else
                {
                    sprintf_s(titulo_nota, sizeof(titulo_nota), "Anotacao %d de %d", pagina_caderno + 1, falas_lidas);

                    al_draw_text(fonte, al_map_rgb(150, 40, 40), 280, 165, ALLEGRO_ALIGN_LEFT, titulo_nota);

                    al_draw_multiline_text(fonte, al_map_rgb(40, 30, 20), 280, 200, 720, 16, ALLEGRO_ALIGN_LEFT, falas[pagina_caderno]);
                }

                al_draw_text(fonte, al_map_rgb(110, 70, 30), 640, 590, ALLEGRO_ALIGN_CENTER, "[<] [>] Trocar anotacao   [N] ou [ESC] Fechar");
            }

            al_flip_display();
        }
    }

    if (tem_professor)
        al_destroy_bitmap(prof_sprites);

    al_destroy_bitmap(tile_porta);
    al_destroy_bitmap(tile_parede);
    al_destroy_bitmap(personagem_sprites);
    al_destroy_font(fonte);
    al_destroy_timer(fps);
    al_destroy_display(display);
    al_destroy_event_queue(evento);

    return 0;
}