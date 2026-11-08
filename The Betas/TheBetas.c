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
#define QTD_FALAS 9
#define QTD_INSPETORES 4
#define PROF_COLUNAS 7
#define PROF_LINHAS 4
#define DISTANCIA_PROFESSOR 110

enum { DIR_BAIXO, DIR_CIMA, DIR_ESQUERDA, DIR_DIREITA };

typedef struct
{
    char enunciado[100];
    int resposta;
} Pergunta;

typedef struct
{
    float x, y;
    float velocidade;
    int direcao;
    int contador_animacao;
    int destino_linha;
    int destino_coluna;
    bool tem_destino;
    int passo_linha;
    int passo_coluna;
    bool tem_passo;
    bool andando;
} Inspetor;

Pergunta perguntas[QTD_PERGUNTAS] = {
    { "2x + 7 = 3x - 5", 12 },
    { "3(x - 2) + 5 = 2x + 7", 8 },
    { "5(x + 2) - 3x = 4x - 10", 10 },
    { "x/2 + x/3 = 15", 18 },
    { "4(2x - 3) = 3(x + 6) - 5", 5 },
    { "7x - 4 = 2x - 24", -4 },
    { "(2x - 1)/3 = (x + 4)/2", 14 } };

char falas[QTD_FALAS][200] = {
    "Ola, aluno! Eu sou o Professor. Fique por perto: vou te ensinar o basico para abrir as portas deste labirinto.",
    "Uma equacao do 1o grau tem uma letra, o x, chamada de incognita. Resolver a equacao e descobrir quanto vale esse x.",
    "Pense numa balanca: os dois lados do sinal de = precisam ficar equilibrados. Tudo o que voce fizer de um lado, faca do outro.",
    "Regra pratica: o que esta somando de um lado passa subtraindo para o outro lado. O que esta multiplicando passa dividindo.",
    "Exemplo 1: x + 5 = 12. Passo o 5 subtraindo: x = 12 - 5. Entao x = 7.",
    "Exemplo 2: 3x - 4 = 11. Passo o 4 somando: 3x = 15. O 3 multiplica, entao passa dividindo: x = 15 / 3 = 5.",
    "Com x dos dois lados, junte os x de um lado e os numeros do outro. Ex: 5x - 3 = 2x + 9 -> 3x = 12 -> x = 4.",
    "Com parenteses, multiplique primeiro: 3(x - 2) = 3x - 6. Com fracoes, multiplique tudo pelo denominador comum.",
    "Dica final: depois de achar o x, troque ele na equacao para conferir. Se os dois lados derem igual, voce acertou! Boa sorte!" };

int mapa[LINHAS][COLUNAS] = {
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
    { 1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1 },
    { 1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1 },
    { 1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,2,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1 },
    { 1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1 },
    { 1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,2,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1 },
    { 1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1 },
    { 1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1 },
    { 1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1 },
    { 1,2,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1 },
    { 1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1 },
    { 1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,2,1 },
    { 1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1 },
    { 1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1 },
    { 1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1 },
    { 1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,2,1,0,1,1,1,1,1,0,1,0,1 },
    { 1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1 },
    { 1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1 },
    { 1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1 },
    { 1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1 },
    { 1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1 },
    { 1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1 },
    { 1,0,0,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,2 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
};

Inspetor inspetores[QTD_INSPETORES];

int insp_x[7] = { 272, 477, 679, 879, 1081, 1289, 1493 };
int insp_y[4] = { 17, 239, 458, 672 };
int insp_h[4] = { 218, 219, 213, 207 };

void desenhar_mapa(float camera_x, float camera_y, ALLEGRO_BITMAP* tile_parede, ALLEGRO_BITMAP* tile_porta)
{
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            float x = j * TILE_SIZE - camera_x;
            float y = i * TILE_SIZE - camera_y;

            if (mapa[i][j] == PAREDE)
                al_draw_scaled_bitmap(tile_parede, 0, 0, al_get_bitmap_width(tile_parede), al_get_bitmap_height(tile_parede), x, y, TILE_SIZE, TILE_SIZE, 0);
            else if (mapa[i][j] == PORTA_FECHADA)
                al_draw_scaled_bitmap(tile_porta, 0, 0, al_get_bitmap_width(tile_porta), al_get_bitmap_height(tile_porta), x, y, TILE_SIZE, TILE_SIZE, 0);
            else
                al_draw_filled_rectangle(x, y, x + TILE_SIZE, y + TILE_SIZE, al_map_rgb(180, 180, 180));
        }
    }
}

bool pode_andar(float x, float y)
{
    int esquerda = (int)(x / TILE_SIZE);
    int direita = (int)((x + CELL_W - 1) / TILE_SIZE);
    int cima = (int)(y / TILE_SIZE);
    int baixo = (int)((y + CELL_H - 1) / TILE_SIZE);

    if (esquerda < 0 || direita >= COLUNAS || cima < 0 || baixo >= LINHAS)
        return false;

    for (int linha = cima; linha <= baixo; linha++)
    {
        for (int coluna = esquerda; coluna <= direita; coluna++)
        {
            if (mapa[linha][coluna] == PAREDE || mapa[linha][coluna] == PORTA_FECHADA)
                return false;
        }
    }

    return true;
}

void abrir_porta(int linha, int coluna)
{
    if (mapa[linha][coluna] == PORTA_FECHADA)
        mapa[linha][coluna] = CHAO;
}

bool clicou(float mx, float my, float x1, float y1, float x2, float y2)
{
    return (mx >= x1 && mx <= x2 && my >= y1 && my <= y2);
}

void desenhar_botao(ALLEGRO_FONT* fonte, float x1, float y1, float x2, float y2, const char* texto)
{
    al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(70, 70, 90));
    al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(220, 220, 220), 2);
    al_draw_text(fonte, al_map_rgb(255, 255, 255), (x1 + x2) / 2, (y1 + y2) / 2 - 4, ALLEGRO_ALIGN_CENTER, texto);
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

float centro_x(int coluna)
{
    return coluna * TILE_SIZE + (TILE_SIZE - CELL_W) / 2.0;
}

float centro_y(int linha)
{
    return linha * TILE_SIZE + (TILE_SIZE - CELL_H) / 2.0;
}

bool bateu(float x1, float y1, float x2, float y2)
{
    return (x1 < x2 + CELL_W && x1 + CELL_W > x2 && y1 < y2 + CELL_H && y1 + CELL_H > y2);
}

void reiniciar_inspetores()
{
    int linhas_inicio[QTD_INSPETORES] = { 7, 21, 1, 15 };
    int colunas_inicio[QTD_INSPETORES] = { 5, 29, 30, 4 };
    int direcoes[QTD_INSPETORES] = { DIR_DIREITA, DIR_ESQUERDA, DIR_BAIXO, DIR_CIMA };

    for (int i = 0; i < QTD_INSPETORES; i++)
    {
        inspetores[i].x = centro_x(colunas_inicio[i]);
        inspetores[i].y = centro_y(linhas_inicio[i]);
        inspetores[i].velocidade = 1.5;
        inspetores[i].direcao = direcoes[i];
        inspetores[i].contador_animacao = 0;
        inspetores[i].destino_linha = -1;
        inspetores[i].destino_coluna = -1;
        inspetores[i].tem_destino = false;
        inspetores[i].passo_linha = -1;
        inspetores[i].passo_coluna = -1;
        inspetores[i].tem_passo = false;
        inspetores[i].andando = false;
    }
}

bool escolher_destino(Inspetor* insp)
{
    int coluna = (int)(insp->x + CELL_W / 2) / TILE_SIZE;
    int linha = (int)(insp->y + CELL_H / 2) / TILE_SIZE;

    int fila[LINHAS * COLUNAS];
    int destinos[LINHAS * COLUNAS];
    int visitado[LINHAS][COLUNAS] = { 0 };
    int dl[4] = { 1, -1, 0, 0 };
    int dc[4] = { 0, 0, -1, 1 };
    int inicio = linha * COLUNAS + coluna;
    int frente = 0, tras = 0, qtd = 0;

    fila[tras++] = inicio;
    visitado[linha][coluna] = 1;

    while (frente < tras)
    {
        int atual = fila[frente++];
        int l = atual / COLUNAS;
        int c = atual % COLUNAS;

        if (atual != inicio)
            destinos[qtd++] = atual;

        for (int d = 0; d < 4; d++)
        {
            int nl = l + dl[d];
            int nc = c + dc[d];

            if (nl < 0 || nl >= LINHAS || nc < 0 || nc >= COLUNAS)
                continue;
            if (visitado[nl][nc] == 1 || mapa[nl][nc] != CHAO)
                continue;

            visitado[nl][nc] = 1;
            fila[tras++] = nl * COLUNAS + nc;
        }
    }

    if (qtd == 0)
        return false;

    int escolhido = destinos[rand() % qtd];
    insp->destino_linha = escolhido / COLUNAS;
    insp->destino_coluna = escolhido % COLUNAS;
    insp->tem_destino = true;
    return true;
}

bool proximo_passo(Inspetor* insp, int* prox_linha, int* prox_coluna)
{
    int coluna = (int)(insp->x + CELL_W / 2) / TILE_SIZE;
    int linha = (int)(insp->y + CELL_H / 2) / TILE_SIZE;

    int fila[LINHAS * COLUNAS];
    int pai[LINHAS * COLUNAS];
    int dl[4] = { 1, -1, 0, 0 };
    int dc[4] = { 0, 0, -1, 1 };
    int inicio = linha * COLUNAS + coluna;
    int destino = insp->destino_linha * COLUNAS + insp->destino_coluna;
    int frente = 0, tras = 0;

    for (int i = 0; i < LINHAS * COLUNAS; i++)
        pai[i] = -2;

    fila[tras++] = inicio;
    pai[inicio] = -1;

    while (frente < tras && pai[destino] == -2)
    {
        int atual = fila[frente++];
        int l = atual / COLUNAS;
        int c = atual % COLUNAS;

        for (int d = 0; d < 4; d++)
        {
            int nl = l + dl[d];
            int nc = c + dc[d];

            if (nl < 0 || nl >= LINHAS || nc < 0 || nc >= COLUNAS)
                continue;

            int viz = nl * COLUNAS + nc;

            if (pai[viz] != -2 || mapa[nl][nc] != CHAO)
                continue;

            pai[viz] = atual;
            fila[tras++] = viz;
        }
    }

    if (pai[destino] == -2)
        return false;

    int passo = destino;
    while (pai[passo] != inicio && pai[passo] != -1)
        passo = pai[passo];

    if (passo == inicio)
        return false;

    *prox_linha = passo / COLUNAS;
    *prox_coluna = passo % COLUNAS;
    return true;
}

void mover_inspetor(Inspetor* insp)
{
    insp->andando = false;
    insp->contador_animacao++;

    if (insp->contador_animacao >= 42)
        insp->contador_animacao = 0;

    int linha = (int)(insp->y + CELL_H / 2) / TILE_SIZE;
    int coluna = (int)(insp->x + CELL_W / 2) / TILE_SIZE;

    if (insp->tem_destino && linha == insp->destino_linha && coluna == insp->destino_coluna && !insp->tem_passo)
        insp->tem_destino = false;

    if (!insp->tem_destino)
    {
        if (!escolher_destino(insp))
            return;
    }

    if (!insp->tem_passo)
    {
        if (!proximo_passo(insp, &insp->passo_linha, &insp->passo_coluna))
        {
            insp->tem_destino = false;
            return;
        }
        insp->tem_passo = true;
    }

    float alvo_x = centro_x(insp->passo_coluna);
    float alvo_y = centro_y(insp->passo_linha);
    float dx = alvo_x - insp->x;
    float dy = alvo_y - insp->y;

    if (dx > 0.5)
    {
        insp->direcao = DIR_DIREITA;
        float novo_x = insp->x + insp->velocidade;
        if (novo_x > alvo_x)
            novo_x = alvo_x;
        if (pode_andar(novo_x, insp->y))
            insp->x = novo_x;
        else
            insp->tem_passo = false;
    }
    else if (dx < -0.5)
    {
        insp->direcao = DIR_ESQUERDA;
        float novo_x = insp->x - insp->velocidade;
        if (novo_x < alvo_x)
            novo_x = alvo_x;
        if (pode_andar(novo_x, insp->y))
            insp->x = novo_x;
        else
            insp->tem_passo = false;
    }
    else if (dy > 0.5)
    {
        insp->direcao = DIR_BAIXO;
        float novo_y = insp->y + insp->velocidade;
        if (novo_y > alvo_y)
            novo_y = alvo_y;
        if (pode_andar(insp->x, novo_y))
            insp->y = novo_y;
        else
            insp->tem_passo = false;
    }
    else if (dy < -0.5)
    {
        insp->direcao = DIR_CIMA;
        float novo_y = insp->y - insp->velocidade;
        if (novo_y < alvo_y)
            novo_y = alvo_y;
        if (pode_andar(insp->x, novo_y))
            insp->y = novo_y;
        else
            insp->tem_passo = false;
    }

    if (insp->x == alvo_x && insp->y == alvo_y)
        insp->tem_passo = false;

    insp->andando = true;
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

    ALLEGRO_BITMAP* personagem_sprites = al_load_bitmap("personagem_spritesheet.png");
    ALLEGRO_BITMAP* tile_parede = al_load_bitmap("tileparede_mapa1.png");
    ALLEGRO_BITMAP* tile_porta = al_load_bitmap("porta_mapa.png");
    ALLEGRO_BITMAP* inspetor_sprites = al_load_bitmap("Sprite_inspetor.png");
    ALLEGRO_BITMAP* prof_sprites = al_load_bitmap("spriteprofessor.png");

    if (personagem_sprites == NULL || tile_parede == NULL || tile_porta == NULL || inspetor_sprites == NULL)
    {
        printf("Erro ao carregar uma das imagens.\n");
        return 1;
    }

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

    reiniciar_inspetores();

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
                float bx = 380 + (k % 3) * 80;
                float by = 290 + (k / 3) * 55;

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
                    else if (tamanho_resposta < 6)
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

            if (clicou(mx, my, 640, 290, 900, 350))
            {
                if (tamanho_resposta > 0 && !(tamanho_resposta == 1 && resposta[0] == '-'))
                {
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
                    personagem_x = novo_x;

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
                    personagem_y = novo_y;

                if (al_key_down(&teclado, ALLEGRO_KEY_E))
                {
                    int coluna = (int)(personagem_x + CELL_W / 2) / TILE_SIZE;
                    int linha = (int)(personagem_y + CELL_H / 2) / TILE_SIZE;

                    for (int i = linha - 1; i <= linha + 1; i++)
                    {
                        for (int j = coluna - 1; j <= coluna + 1; j++)
                        {
                            if (i >= 0 && i < LINHAS && j >= 0 && j < COLUNAS)
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

            if (!perguntando && !caderno_aberto && !conversando)
            {
                for (int i = 0; i < QTD_INSPETORES; i++)
                    mover_inspetor(&inspetores[i]);

                bool capturado = false;
                for (int i = 0; i < QTD_INSPETORES; i++)
                {
                    if (bateu(personagem_x, personagem_y, inspetores[i].x, inspetores[i].y))
                        capturado = true;
                }

                if (capturado)
                {
                    personagem_x = 80;
                    personagem_y = 60;
                    reiniciar_inspetores();

                    conversando = false;
                    perguntando = false;
                    caderno_aberto = false;
                    resposta[0] = '\0';
                    tamanho_resposta = 0;
                    mensagem[0] = '\0';
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
            if (contador_anim >= 30)
                contador_anim = 0;

            if (conversando && pagina_fala + 1 > falas_lidas)
                falas_lidas = pagina_fala + 1;

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

                al_draw_scaled_bitmap(prof_sprites, quadro_coluna * prof_quadro_w, quadro_linha * prof_quadro_h, prof_quadro_w, prof_quadro_h, professor_x - camera_x, professor_y - camera_y, prof_w, prof_h, 0);

                if (!conversando && perto_do_professor(personagem_x, personagem_y, professor_x, professor_y))
                    al_draw_text(fonte, al_map_rgb(255, 255, 0), professor_x - camera_x + prof_w / 2, professor_y - camera_y - 14, ALLEGRO_ALIGN_CENTER, "[E] Falar");
            }
            else
            {
                al_draw_text(fonte, al_map_rgb(255, 80, 80), 10, 10, ALLEGRO_ALIGN_LEFT, "spriteprofessor.png nao encontrado");
            }

            for (int i = 0; i < QTD_INSPETORES; i++)
            {
                int linha = inspetores[i].direcao;
                int coluna = 0;

                if (inspetores[i].andando)
                    coluna = (inspetores[i].contador_animacao / 6) % 7;

                float altura = insp_h[linha] * CELL_H / 218.0;

                al_draw_scaled_bitmap(inspetor_sprites, insp_x[coluna] - 75, insp_y[linha], 150, insp_h[linha], inspetores[i].x - camera_x, inspetores[i].y - camera_y + CELL_H - altura, CELL_W, altura, 0);
            }

            al_draw_scaled_bitmap(personagem_sprites, direcao * 52, (andando ? 1 : 0) * 96, 52, 96, personagem_x - camera_x, personagem_y - camera_y, CELL_W, CELL_H, 0);

            al_draw_text(fonte, al_map_rgb(255, 255, 255), 1270, 10, ALLEGRO_ALIGN_RIGHT, "[N] Caderno");

            if (conversando && !perguntando)
            {
                char numero_pagina[20];

                al_draw_filled_rectangle(140, 570, 1140, 700, al_map_rgb(35, 35, 45));
                al_draw_rectangle(140, 570, 1140, 700, al_map_rgb(220, 220, 220), 2);
                al_draw_text(fonte, al_map_rgb(255, 255, 0), 160, 585, ALLEGRO_ALIGN_LEFT, "PROFESSOR");
                al_draw_multiline_text(fonte, al_map_rgb(255, 255, 255), 160, 610, 960, 14, ALLEGRO_ALIGN_LEFT, falas[pagina_fala]);

                sprintf_s(numero_pagina, sizeof(numero_pagina), "%d/%d", pagina_fala + 1, QTD_FALAS);

                al_draw_text(fonte, al_map_rgb(200, 200, 200), 160, 680, ALLEGRO_ALIGN_LEFT, numero_pagina);b  
                al_draw_text(fonte, al_map_rgb(200, 200, 200), 1120, 680, ALLEGRO_ALIGN_RIGHT, "[E] Continuar   [ESC] Fechar");
            }

            if (perguntando)
            {
                al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(0, 0, 0, 180));
                al_draw_filled_rectangle(300, 150, 980, 570, al_map_rgb(35, 35, 45));
                al_draw_rectangle(300, 150, 980, 570, al_map_rgb(220, 220, 220), 2);

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 640, 170, ALLEGRO_ALIGN_CENTER, "PORTA BLOQUEADA - RESOLVA A EQUACAO");
                al_draw_text(fonte, al_map_rgb(255, 255, 0), 640, 205, ALLEGRO_ALIGN_CENTER, perguntas[pergunta_atual].enunciado);
                al_draw_text(fonte, al_map_rgb(200, 200, 200), 640, 225, ALLEGRO_ALIGN_CENTER, "Qual o valor de x?");

                al_draw_filled_rectangle(380, 240, 900, 280, al_map_rgb(15, 15, 20));
                al_draw_rectangle(380, 240, 900, 280, al_map_rgb(180, 180, 180), 2);
                al_draw_text(fonte, al_map_rgb(255, 255, 255), 640, 256, ALLEGRO_ALIGN_CENTER, resposta);

                const char* rotulos[12] = { "1","2","3","4","5","6","7","8","9","-","0","<" };

                for (int k = 0; k < 12; k++)
                {
                    float bx = 380 + (k % 3) * 80;
                    float by = 290 + (k / 3) * 55;
                    desenhar_botao(fonte, bx, by, bx + 70, by + 45, rotulos[k]);
                }

                desenhar_botao(fonte, 640, 290, 900, 350, "CONFIRMAR");
                desenhar_botao(fonte, 640, 370, 900, 430, "FECHAR");

                al_draw_text(fonte, al_map_rgb(255, 80, 80), 640, 530, ALLEGRO_ALIGN_CENTER, mensagem);
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

    al_destroy_bitmap(inspetor_sprites);
    al_destroy_bitmap(tile_porta);
    al_destroy_bitmap(tile_parede);
    al_destroy_bitmap(personagem_sprites);
    al_destroy_font(fonte);
    al_destroy_timer(fps);
    al_destroy_display(display);
    al_destroy_event_queue(evento);

    return 0;
}