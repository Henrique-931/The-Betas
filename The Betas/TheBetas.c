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
#define ESTANTE 3
#define QUADRO 4
#define QTD_PERGUNTAS 7
#define QTD_FALAS 9
#define QTD_INSPETORES 4
#define PROF_COLUNAS 7
#define PROF_LINHAS 4
#define DISTANCIA_PROFESSOR 110
#define PONTOS_PORTA 100
#define TEMPO_LIMITE 300
#define PONTOS_SEGUNDO 5
#define QTD_MAPAS 3
#define TEMA_PEDRA 0
#define TEMA_BIBLIOTECA 1
#define TEMA_LABORATORIO 2

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

typedef struct
{
    int (*dados)[COLUNAS];
    int saida_linha, saida_coluna;
    int insp_linha[QTD_INSPETORES];
    int insp_coluna[QTD_INSPETORES];
    int insp_dir[QTD_INSPETORES];
    float prof_x, prof_y;
    int tema;
} InfoMapa;

typedef struct
{
    ALLEGRO_BITMAP* parede;
    ALLEGRO_BITMAP* porta;
    ALLEGRO_BITMAP* porta_saida;
    ALLEGRO_BITMAP* chao;
    ALLEGRO_BITMAP* estante;
    ALLEGRO_BITMAP* quadro;
    ALLEGRO_BITMAP* chao2;
} Tema;

Pergunta perguntas[QTD_MAPAS][QTD_PERGUNTAS] = {
{
    { "2x + 7 = 3x - 5", 12 },
    { "3(x - 2) + 5 = 2x + 7", 8 },
    { "5(x + 2) - 3x = 4x - 10", 10 },
    { "6x - 9 = 2x + 15", 6 },
    { "4(2x - 3) = 3(x + 6) - 5", 5 },
    { "7x - 4 = 2x - 24", -4 },
    { "2(x + 3) = x + 11", 5 } },
{
    { "x^2 - 5x + 6 = 0  (calcule o delta)", 1 },
    { "3x^2 - 3x - 10 = 0  (calcule o delta)", 129 },
    { "x^2 + 4x + 5 = 0  (calcule o delta)", -4 },
    { "2x^2 + 5x + 2 = 0 (calcule o delta)", 9 },
    { "x^2 - 3x + 2 para x = 4", 6 },
    { "2x^2 - 5x + 1 para x = -2", 19 },
    { "x^2 + 4x - 12 para x = 3", 9 } },
{
    { "25% de 200", 50 },
    { "40% de 50", 20 },
    { "Produto de 80 reais com aumento de 25%: novo preco?", 100 },
    { "Camisa de 120 reais com desconto de 15%: preco final?", 102 },
    { "log base 2 de 32", 5 },
    { "log base 3 de 81", 4 },
    { "log base 10 de 1000", 3 } } };

char falas[QTD_MAPAS][QTD_FALAS][200] = {
{
    "Ola, aluno! Eu sou o Professor. Fique por perto: vou te ensinar o basico para abrir as portas deste labirinto.",
    "Uma equacao do 1o grau tem uma letra, o x, chamada de incognita. Resolver a equacao e descobrir quanto vale esse x.",
    "Pense numa balanca: os dois lados do sinal de = precisam ficar equilibrados. Tudo o que voce fizer de um lado, faca do outro.",
    "Regra pratica: o que esta somando de um lado passa subtraindo para o outro lado. O que esta multiplicando passa dividindo.",
    "Exemplo 1: x + 5 = 12. Passo o 5 subtraindo: x = 12 - 5. Entao x = 7.",
    "Exemplo 2: 3x - 4 = 11. Passo o 4 somando: 3x = 15. O 3 multiplica, entao passa dividindo: x = 15 / 3 = 5.",
    "Com x dos dois lados, junte os x de um lado e os numeros do outro. Ex: 5x - 3 = 2x + 9 -> 3x = 12 -> x = 4.",
    "Com parenteses, multiplique primeiro: 3(x - 2) = 3x - 6. Com fracoes, multiplique tudo pelo denominador comum.",
    "Dica final: depois de achar o x, troque ele na equacao para conferir. Se os dois lados derem igual, voce acertou! Boa sorte!" },
{
    "Bem-vindo a biblioteca! Agora as portas pedem equacoes do 2o grau. Eu sou o Professor e vou te mostrar o que voce precisa saber.",
    "Equacao do 2o grau: ax^2 + bx + c = 0, com a diferente de 0. Os numeros a, b e c sao os coeficientes. Ex: x^2 - 5x + 6 = 0 tem a = 1, b = -5 e c = 6.",
    "Para achar a, b e c, deixe tudo de um lado e zero do outro. Ex: x^2 = 4x + 21 vira x^2 - 4x - 21 = 0, entao a = 1, b = -4 e c = -21. Atencao ao sinal!",
    "O delta e calculado assim: delta = b^2 - 4ac. Primeiro eleve o b ao quadrado, depois calcule 4 vezes a vezes c e subtraia um do outro.",
    "Ex: x^2 - 3x - 10 = 0. a = 1, b = -3, c = -10. Delta = (-3)^2 - 4(1)(-10) = 9 + 40 = 49. Cuidado: (-3)^2 = 9, nao -9, e menos com menos vira mais!",
    "Delta positivo: 2 raizes. Delta zero: 1 raiz. Delta negativo: nenhuma raiz real. Ex: x^2 + 4x + 5 = 0 -> delta = 16 - 20 = -4, entao nao tem raiz real.",
    "Outras portas pedem so para substituir o x. Ex: y = x^2 - 3x + 2 para x = 4. Troque o x por 4: 4^2 - 3(4) + 2 = 16 - 12 + 2 = 6.",
    "Com x negativo, use parenteses! Ex: y = 2x^2 - 5x + 1 para x = -2 -> 2(-2)^2 - 5(-2) + 1 = 2(4) + 10 + 1 = 19. Lembre: (-2)^2 = 4.",
    "Dica final: va com calma, faca a conta passo a passo e confira os sinais. Se a porta pedir o delta, use b^2 - 4ac. Se pedir para substituir, troque o x. Boa sorte!" },
{
    "Bem-vindo ao laboratorio! Aqui as portas misturam dois assuntos: porcentagem e logaritmo. Vou te ensinar os dois, comecando pela porcentagem.",
    "Porcentagem significa 'por cem'. 25% e o mesmo que 25/100 = 0,25. Para achar uma porcentagem de um numero, multiplique. Ex: 25% de 200 = 0,25 x 200 = 50.",
    "Truque de cabeca: 10% e dividir por 10, 50% e a metade, 25% e a metade da metade. Ex: 40% de 50 -> 10% de 50 = 5, entao 40% = 4 x 5 = 20.",
    "Aumento: calcule a porcentagem e SOME. Ex: 80 com aumento de 25%: 25% de 80 = 20, entao 80 + 20 = 100.",
    "Desconto: calcule a porcentagem e SUBTRAIA. Ex: 120 com desconto de 15%: 10% = 12, 5% = 6, 15% = 18. Preco final: 120 - 18 = 102.",
    "Agora o logaritmo! log base a de b e o expoente que voce poe no a para chegar no b. Ex: log base 2 de 8 = 3, porque 2^3 = 8.",
    "Para resolver, pergunte: a que numero devo elevar a base para chegar la? Ex: log base 10 de 1000 = 3, porque 10 x 10 x 10 = 1000.",
    "Mais um: log base 3 de 81. Faca as potencias de 3: 3, 9, 27, 81. Sao 4 passos, entao o logaritmo e 4. Para log base 2 de 32: 2, 4, 8, 16, 32 -> 5.",
    "Dica final: porcentagem de aumento soma, desconto subtrai. No log, a base e o numero que voce multiplica por ele mesmo. Conte os passos! Boa sorte!" } };

int mapa1[LINHAS][COLUNAS] = {
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
    { 1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1 },
    { 1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1 },
    { 1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1 },
    { 1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1 },
    { 1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1 },
    { 1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1 },
    { 1,0,0,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,2 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
};

int mapa2[LINHAS][COLUNAS] = {
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,0,1,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1 },
    { 1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,2,1,1,1,1,1,0,1 },
    { 1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,4,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1 },
    { 1,1,1,1,1,1,1,1,1,0,3,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1 },
    { 1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1 },
    { 1,0,1,2,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,3,1,0,1,0,4,0,1,0,1,0,3,1,1,0,1 },
    { 1,0,3,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1 },
    { 1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1 },
    { 1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,3,0,0,0,0,0,1,0,0,0,0,0,0,0,4,0,1 },
    { 1,0,1,4,1,0,1,0,1,1,1,1,3,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1 },
    { 1,0,1,0,0,0,1,0,0,0,0,0,1,0,4,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,3,0,0,0,1 },
    { 1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1 },
    { 1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1 },
    { 1,2,1,0,1,0,1,1,1,0,1,3,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1 },
    { 1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1 },
    { 1,0,1,1,3,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1 },
    { 1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1 },
    { 1,1,1,1,1,0,1,1,4,0,1,0,1,1,1,0,1,3,1,0,1,0,1,1,1,0,3,1,1,1,1,1,1,0,1 },
    { 1,0,0,0,1,0,0,0,1,0,4,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,2,0,0,0,1,0,0,0,1 },
    { 1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1 },
    { 1,0,1,0,0,0,0,0,1,0,0,0,0,0,3,0,0,0,0,0,1,0,1,0,0,0,2,0,1,0,1,0,0,0,1 },
    { 1,0,1,0,1,1,1,1,1,1,1,2,3,0,1,0,1,1,1,1,1,0,1,1,1,1,3,0,1,0,1,1,1,0,1 },
    { 1,0,2,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,2 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
};

int mapa3[LINHAS][COLUNAS] = {
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,2,0,0,0,1,0,0,0,1 },
    { 1,3,1,0,1,1,3,1,1,0,1,1,3,0,1,0,3,1,1,0,3,3,1,0,3,3,3,3,1,0,3,0,1,3,1 },
    { 1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1 },
    { 1,0,1,3,3,3,1,0,1,0,3,0,1,3,1,1,1,0,1,3,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1 },
    { 1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,2,0,1 },
    { 1,3,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,3,1,0,1,1,1,0,1,0,3,3,1 },
    { 1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1 },
    { 1,0,3,3,1,0,1,3,3,0,3,0,1,0,1,1,1,0,1,0,1,1,3,0,1,0,3,0,1,0,1,0,1,0,1 },
    { 1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,2,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1 },
    { 1,0,1,0,1,3,1,0,1,0,1,0,3,1,1,0,1,0,3,1,1,0,1,1,3,0,1,0,1,0,1,1,3,0,1 },
    { 1,0,1,0,1,0,2,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1 },
    { 1,0,1,0,1,0,1,3,1,0,1,1,1,0,1,0,3,0,1,0,1,0,3,3,1,0,1,0,3,0,1,0,1,3,1 },
    { 1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1 },
    { 1,0,3,0,1,0,1,0,1,0,3,3,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,3,0,1,0,1 },
    { 1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1 },
    { 1,0,1,0,1,0,3,0,1,1,1,0,3,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,3,1,1,1,1,0,1 },
    { 1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1 },
    { 1,0,1,0,1,0,1,1,1,0,1,1,1,1,3,0,1,0,1,3,3,1,1,0,1,0,1,3,1,1,1,0,1,0,1 },
    { 1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,2,0,1 },
    { 1,0,1,1,1,1,3,1,1,0,1,0,1,1,1,1,1,2,1,0,3,0,1,0,1,0,1,0,1,0,1,3,3,0,1 },
    { 1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1 },
    { 1,0,3,0,1,3,1,1,1,0,1,0,1,0,1,1,1,0,1,3,3,0,1,0,3,1,3,2,1,3,1,0,3,0,1 },
    { 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,2 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
};

int mapa[LINHAS][COLUNAS];

InfoMapa mapas[QTD_MAPAS] = {
    {
        mapa1, 23, 34,
        { 7, 21, 1, 15 }, { 5, 29, 30, 4 }, { DIR_DIREITA, DIR_ESQUERDA, DIR_BAIXO, DIR_CIMA },
        11 * TILE_SIZE + 6, 11 * TILE_SIZE + 5,
        TEMA_PEDRA
    },
    {
        mapa2, 23, 34,
        { 23, 19, 13, 23 }, { 5, 17, 11, 25 }, { DIR_DIREITA, DIR_CIMA, DIR_BAIXO, DIR_ESQUERDA },
        4 * TILE_SIZE + 6, 1 * TILE_SIZE + 5,
        TEMA_BIBLIOTECA
    },
    {
        mapa3, 23, 34,
        { 9, 13, 19, 23 }, { 23, 31, 17, 9 }, { DIR_BAIXO, DIR_ESQUERDA, DIR_CIMA, DIR_DIREITA },
        3 * TILE_SIZE + 6, 1 * TILE_SIZE + 5,
        TEMA_LABORATORIO
    }
};

int mapa_atual = 0;
int saida_linha = 23;
int saida_coluna = 34;
float professor_x = 0;
float professor_y = 0;

int mapa_original[LINHAS][COLUNAS];

Inspetor inspetores[QTD_INSPETORES];

int insp_x[7] = { 272, 477, 679, 879, 1081, 1289, 1493 };
int insp_y[4] = { 17, 239, 458, 672 };
int insp_h[4] = { 218, 219, 213, 207 };

void desenhar_tile(ALLEGRO_BITMAP* img, float x, float y)
{
    al_draw_scaled_bitmap(img, 0, 0, al_get_bitmap_width(img), al_get_bitmap_height(img), x, y, TILE_SIZE, TILE_SIZE, 0);
}

void desenhar_mapa(float camera_x, float camera_y, const Tema* t)
{
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            float x = j * TILE_SIZE - camera_x;
            float y = i * TILE_SIZE - camera_y;
            int tipo = mapa[i][j];

            if (tipo == PAREDE || tipo == ESTANTE || tipo == QUADRO)
            {
                desenhar_tile(t->parede, x, y);

                if (tipo == ESTANTE)
                    desenhar_tile(t->estante, x, y);
                else if (tipo == QUADRO)
                    desenhar_tile(t->quadro, x, y);
            }
            else if (tipo == PORTA_FECHADA)
            {
                if (i == saida_linha && j == saida_coluna)
                {
                    if (t->porta_saida != NULL)
                        desenhar_tile(t->porta_saida, x, y);
                    else
                        al_draw_tinted_scaled_bitmap(t->porta, al_map_rgb(70, 130, 255), 0, 0, al_get_bitmap_width(t->porta), al_get_bitmap_height(t->porta), x, y, TILE_SIZE, TILE_SIZE, 0);
                }
                else
                {
                    desenhar_tile(t->porta, x, y);
                }
            }
            else
            {
                ALLEGRO_BITMAP* img_chao = t->chao;

                if (t->chao2 != NULL && (i * 7 + j * 3) % 5 == 0)
                    img_chao = t->chao2;

                if (img_chao != NULL)
                    desenhar_tile(img_chao, x, y);
                else
                    al_draw_filled_rectangle(x, y, x + TILE_SIZE, y + TILE_SIZE, al_map_rgb(180, 180, 180));
            }
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
            if (mapa[linha][coluna] != CHAO)
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

void guardar_mapa()
{
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
            mapa_original[i][j] = mapa[i][j];
    }
}

void reiniciar_mapa()
{
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
            mapa[i][j] = mapa_original[i][j];
    }
}

bool fechar_porta_aleatoria()
{
    int candidatas[LINHAS * COLUNAS];
    int qtd = 0;

    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            if (mapa_original[i][j] == PORTA_FECHADA && mapa[i][j] == CHAO)
                candidatas[qtd++] = i * COLUNAS + j;
        }
    }

    if (qtd == 0)
        return false;

    int escolhida = candidatas[rand() % qtd];
    mapa[escolhida / COLUNAS][escolhida % COLUNAS] = PORTA_FECHADA;
    return true;
}

int contar_portas_total()
{
    int total = 0;

    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            if (mapa_original[i][j] == PORTA_FECHADA)
                total++;
        }
    }

    return total;
}

int contar_portas_abertas()
{
    int abertas = 0;

    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            if (mapa_original[i][j] == PORTA_FECHADA && mapa[i][j] == CHAO)
                abertas++;
        }
    }

    return abertas;
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

void desenhar_botao_menu(ALLEGRO_FONT* fonte, float x1, float y1, float x2, float y2, const char* texto, bool selecionado)
{
    if (selecionado)
    {
        al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(110, 110, 170));
        al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(255, 220, 60), 3);
        al_draw_text(fonte, al_map_rgb(255, 220, 60), (x1 + x2) / 2, (y1 + y2) / 2 - 4, ALLEGRO_ALIGN_CENTER, texto);
    }
    else
    {
        al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(70, 70, 90));
        al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(220, 220, 220), 2);
        al_draw_text(fonte, al_map_rgb(255, 255, 255), (x1 + x2) / 2, (y1 + y2) / 2 - 4, ALLEGRO_ALIGN_CENTER, texto);
    }
}

void desenhar_texto_grande(ALLEGRO_FONT* fonte, ALLEGRO_COLOR cor, float x, float y, int escala, const char* texto)
{
    int largura = al_get_text_width(fonte, texto);
    int altura = al_get_font_line_height(fonte);
    ALLEGRO_BITMAP* destino = al_get_target_bitmap();
    ALLEGRO_BITMAP* temporario = al_create_bitmap(largura, altura);

    al_set_target_bitmap(temporario);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_text(fonte, cor, 0, 0, ALLEGRO_ALIGN_LEFT, texto);
    al_set_target_bitmap(destino);

    al_draw_scaled_bitmap(temporario, 0, 0, largura, altura, x - largura * escala / 2, y, largura * escala, altura * escala, 0);
    al_destroy_bitmap(temporario);
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
    InfoMapa* m = &mapas[mapa_atual];

    for (int i = 0; i < QTD_INSPETORES; i++)
    {
        inspetores[i].x = centro_x(m->insp_coluna[i]);
        inspetores[i].y = centro_y(m->insp_linha[i]);
        inspetores[i].velocidade = 1.5;
        inspetores[i].direcao = m->insp_dir[i];
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

void carregar_mapa(int indice)
{
    mapa_atual = indice;

    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
            mapa[i][j] = mapas[indice].dados[i][j];
    }

    guardar_mapa();
    saida_linha = mapas[indice].saida_linha;
    saida_coluna = mapas[indice].saida_coluna;
    professor_x = mapas[indice].prof_x;
    professor_y = mapas[indice].prof_y;
    reiniciar_inspetores();
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

    ALLEGRO_BITMAP* personagem_sprites = al_load_bitmap("personagem_sprite.png");
    ALLEGRO_BITMAP* tile_parede = al_load_bitmap("tileparede_mapa1.png");
    ALLEGRO_BITMAP* tile_porta = al_load_bitmap("porta_mapa.png");
    ALLEGRO_BITMAP* inspetor_sprites = al_load_bitmap("Sprite_inspetor.png");
    ALLEGRO_BITMAP* prof_sprites = al_load_bitmap("spriteprofessor.png");
    ALLEGRO_BITMAP* tile_porta_saida = al_load_bitmap("porta_saida.png");

    if (!personagem_sprites) { printf("Erro: personagem_sprite.png\n"); return -1; }
    if (!inspetor_sprites) { printf("Erro: Sprite_inspetor.png\n"); return -1; }
    if (!tile_parede) { printf("Erro: tileparede_mapa1.png\n"); return -1; }
    if (!tile_porta) { printf("Erro: porta_mapa.png\n"); return -1; }

    ALLEGRO_BITMAP* esc_parede = al_load_bitmap("parede_escola.png");
    ALLEGRO_BITMAP* esc_porta = al_load_bitmap("porta_escola.png");
    ALLEGRO_BITMAP* esc_chao = al_load_bitmap("chao_biblioteca.png");
    ALLEGRO_BITMAP* esc_estante = al_load_bitmap("estante_biblioteca.png");
    ALLEGRO_BITMAP* esc_quadro = al_load_bitmap("deco_quadro.png");

    if (!esc_parede) { printf("Erro: parede_escola.png\n"); return -1; }
    if (!esc_porta) { printf("Erro: porta_escola.png\n"); return -1; }
    if (!esc_chao) { printf("Erro: chao_biblioteca.png\n"); return -1; }
    if (!esc_estante) { printf("Erro: estante_biblioteca.png\n"); return -1; }
    if (!esc_quadro) { printf("Erro: deco_quadro.png\n"); return -1; }

    ALLEGRO_BITMAP* lab_parede = al_load_bitmap("parede_laboratorio.png");
    ALLEGRO_BITMAP* lab_bancada = al_load_bitmap("bancada_laboratorio.png");
    ALLEGRO_BITMAP* lab_porta = al_load_bitmap("porta_laboratorio.png");
    ALLEGRO_BITMAP* lab_porta_saida = al_load_bitmap("porta_saida_laboratorio.png");
    ALLEGRO_BITMAP* lab_chao = al_load_bitmap("chao_laboratorio.png");
    ALLEGRO_BITMAP* lab_chao2 = al_load_bitmap("chao_laboratorio2.png");

    if (!lab_parede) { printf("Erro: parede_laboratorio.png\n"); return -1; }
    if (!lab_bancada) { printf("Erro: bancada_laboratorio.png\n"); return -1; }
    if (!lab_porta) { printf("Erro: porta_laboratorio.png\n"); return -1; }
    if (!lab_porta_saida) { printf("Erro: porta_saida_laboratorio.png\n"); return -1; }
    if (!lab_chao) { printf("Erro: chao_laboratorio.png\n"); return -1; }
    if (!lab_chao2) { printf("Erro: chao_laboratorio2.png\n"); return -1; }

    Tema tema_pedra = { tile_parede, tile_porta, tile_porta_saida, NULL, NULL, NULL, NULL };
    Tema tema_biblioteca = { esc_parede, esc_porta, NULL, esc_chao, esc_estante, esc_quadro, NULL };
    Tema tema_laboratorio = { lab_parede, lab_porta, lab_porta_saida, lab_chao, lab_bancada, NULL, lab_chao2 };

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

    carregar_mapa(0);

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
    int anim_andar = 0;
    bool no_menu = true;
    bool ajuda_aberta = false;
    int opcao_menu = 0;
    int contador_menu = 0;
    bool fim_de_jogo = false;
    int opcao_fim = 0;
    int contador_fim = 0;
    int aviso_porta = 0;
    int aviso_tempo = 0;
    int pontos = 0;
    int tempo_jogo = 0;
    int portas_abertas = 0;
    int bonus_tempo = 0;
    int pontos_acumulados = 0;
    int aviso_mapa = 0;

    while (rodando)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(evento, &ev);

        if (no_menu)
        {
            bool confirmou = false;

            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                rodando = false;

            if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                if (ajuda_aberta)
                {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE || ev.keyboard.keycode == ALLEGRO_KEY_ENTER)
                        ajuda_aberta = false;
                }
                else
                {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP)
                        opcao_menu = (opcao_menu + 2) % 3;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
                        opcao_menu = (opcao_menu + 1) % 3;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER)
                        confirmou = true;
                }
            }

            if (ev.type == ALLEGRO_EVENT_MOUSE_AXES && !ajuda_aberta)
            {
                for (int k = 0; k < 3; k++)
                {
                    if (clicou(ev.mouse.x, ev.mouse.y, 540, 330 + k * 80, 740, 390 + k * 80))
                        opcao_menu = k;
                }
            }

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1)
            {
                if (ajuda_aberta)
                {
                    ajuda_aberta = false;
                }
                else
                {
                    for (int k = 0; k < 3; k++)
                    {
                        if (clicou(ev.mouse.x, ev.mouse.y, 540, 330 + k * 80, 740, 390 + k * 80))
                        {
                            opcao_menu = k;
                            confirmou = true;
                        }
                    }
                }
            }

            if (confirmou)
            {
                if (opcao_menu == 0)
                    no_menu = false;
                else if (opcao_menu == 1)
                    ajuda_aberta = true;
                else
                    rodando = false;
            }

            if (ev.type == ALLEGRO_EVENT_TIMER)
            {
                contador_menu++;
                if (contador_menu >= 672)
                    contador_menu = 0;
            }

            if (ev.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(evento))
            {
                const char* rotulos_menu[3] = { "JOGAR", "COMO JOGAR", "SAIR" };
                int linha = DIR_BAIXO;
                int coluna = (contador_menu / 6) % 7;
                float altura = insp_h[linha] * 220 / 218.0;

                al_clear_to_color(al_map_rgb(25, 25, 40));

                for (int j = 0; j < 22; j++)
                {
                    al_draw_scaled_bitmap(tile_parede, 0, 0, al_get_bitmap_width(tile_parede), al_get_bitmap_height(tile_parede), j * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE, 0);
                    al_draw_scaled_bitmap(tile_parede, 0, 0, al_get_bitmap_width(tile_parede), al_get_bitmap_height(tile_parede), j * TILE_SIZE, 720 - TILE_SIZE, TILE_SIZE, TILE_SIZE, 0);
                }

                desenhar_texto_grande(fonte, al_map_rgb(0, 0, 0), 646, 116, 10, "THE BETAS");
                desenhar_texto_grande(fonte, al_map_rgb(255, 220, 60), 640, 110, 10, "THE BETAS");
                desenhar_texto_grande(fonte, al_map_rgb(200, 200, 200), 640, 220, 2, "Resolva as equacoes e escape do labirinto!");

                al_draw_scaled_bitmap(personagem_sprites, ((contador_menu / 8) % 4) * 56, DIR_BAIXO * 100, 56, 100, 150, 300, 123, 220, 0);
                al_draw_scaled_bitmap(inspetor_sprites, insp_x[coluna] - 75, insp_y[linha], 150, insp_h[linha], 1010, 300 + 220 - altura, 148, altura, 0);

                for (int k = 0; k < 3; k++)
                    desenhar_botao_menu(fonte, 540, 330 + k * 80, 740, 390 + k * 80, rotulos_menu[k], opcao_menu == k);

                al_draw_text(fonte, al_map_rgb(200, 200, 200), 640, 600, ALLEGRO_ALIGN_CENTER, "[SETAS] Escolher   [ENTER] Confirmar");

                if (ajuda_aberta)
                {
                    al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(0, 0, 0, 180));
                    al_draw_filled_rectangle(240, 150, 1040, 570, al_map_rgb(35, 35, 45));
                    al_draw_rectangle(240, 150, 1040, 570, al_map_rgb(220, 220, 220), 2);
                    al_draw_text(fonte, al_map_rgb(255, 220, 60), 640, 175, ALLEGRO_ALIGN_CENTER, "COMO JOGAR");
                    al_draw_line(280, 200, 1000, 200, al_map_rgb(220, 220, 220), 2);
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), 280, 235, ALLEGRO_ALIGN_LEFT, "[SETAS] Andar pelo labirinto");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), 280, 275, ALLEGRO_ALIGN_LEFT, "[E] Perto do professor: conversar e aprender equacoes");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), 280, 315, ALLEGRO_ALIGN_LEFT, "[E] Perto de uma porta: resolver a equacao para abri-la");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), 280, 355, ALLEGRO_ALIGN_LEFT, "[N] Abrir o caderno de anotacoes");
                    al_draw_text(fonte, al_map_rgb(255, 120, 120), 280, 405, ALLEGRO_ALIGN_LEFT, "Fuja dos inspetores! Se eles te pegarem, voce volta ao inicio.");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), 280, 445, ALLEGRO_ALIGN_LEFT, "Voce tem 5 minutos: se o tempo acabar, as portas fecham e a fase recomeca.");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), 280, 470, ALLEGRO_ALIGN_LEFT, "Pontos = portas abertas + bonus pelo tempo que sobrar.");
                    al_draw_text(fonte, al_map_rgb(200, 200, 200), 640, 535, ALLEGRO_ALIGN_CENTER, "Clique ou aperte [ENTER] para voltar");
                }

                al_flip_display();
            }

            continue;
        }

        if (fim_de_jogo)
        {
            bool confirmou = false;

            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                rodando = false;

            if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                if (ev.keyboard.keycode == ALLEGRO_KEY_UP)
                    opcao_fim = (opcao_fim + 1) % 2;
                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
                    opcao_fim = (opcao_fim + 1) % 2;
                if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER)
                    confirmou = true;
            }

            if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                for (int k = 0; k < 2; k++)
                {
                    if (clicou(ev.mouse.x, ev.mouse.y, 540, 330 + k * 80, 740, 390 + k * 80))
                        opcao_fim = k;
                }
            }

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1)
            {
                for (int k = 0; k < 2; k++)
                {
                    if (clicou(ev.mouse.x, ev.mouse.y, 540, 330 + k * 80, 740, 390 + k * 80))
                    {
                        opcao_fim = k;
                        confirmou = true;
                    }
                }
            }

            if (confirmou)
            {
                if (opcao_fim == 0)
                {
                    carregar_mapa(0);
                    pontos_acumulados = 0;
                    aviso_mapa = 0;

                    personagem_x = 80;
                    personagem_y = 60;
                    direcao = DIR_BAIXO;
                    andando = false;
                    anim_andar = 0;
                    conversando = false;
                    perguntando = false;
                    caderno_aberto = false;
                    pagina_fala = 0;
                    pagina_caderno = 0;
                    falas_lidas = 0;
                    resposta[0] = '\0';
                    tamanho_resposta = 0;
                    mensagem[0] = '\0';
                    opcao_fim = 0;
                    aviso_porta = 0;
                    aviso_tempo = 0;
                    pontos = 0;
                    tempo_jogo = 0;
                    portas_abertas = 0;
                    bonus_tempo = 0;
                    fim_de_jogo = false;
                }
                else
                {
                    rodando = false;
                }
            }

            if (ev.type == ALLEGRO_EVENT_TIMER)
            {
                contador_fim++;
                if (contador_fim >= 672)
                    contador_fim = 0;
            }

            if (ev.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(evento))
            {
                const char* rotulos_fim[2] = { "JOGAR DE NOVO", "SAIR" };

                al_clear_to_color(al_map_rgb(25, 25, 40));

                for (int j = 0; j < 22; j++)
                {
                    al_draw_scaled_bitmap(tile_parede, 0, 0, al_get_bitmap_width(tile_parede), al_get_bitmap_height(tile_parede), j * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE, 0);
                    al_draw_scaled_bitmap(tile_parede, 0, 0, al_get_bitmap_width(tile_parede), al_get_bitmap_height(tile_parede), j * TILE_SIZE, 720 - TILE_SIZE, TILE_SIZE, TILE_SIZE, 0);
                }

                desenhar_texto_grande(fonte, al_map_rgb(0, 0, 0), 646, 116, 8, "VOCE ESCAPOU!");
                desenhar_texto_grande(fonte, al_map_rgb(120, 255, 120), 640, 110, 8, "VOCE ESCAPOU!");
                desenhar_texto_grande(fonte, al_map_rgb(200, 200, 200), 640, 220, 2, "Voce resolveu as equacoes e saiu do labirinto!");

                al_draw_scaled_bitmap(personagem_sprites, ((contador_fim / 8) % 4) * 56, DIR_DIREITA * 100, 56, 100, 150, 300, 123, 220, 0);
                al_draw_scaled_bitmap(personagem_sprites, ((contador_fim / 8) % 4) * 56, DIR_ESQUERDA * 100, 56, 100, 1010, 300, 123, 220, 0);

                char texto_portas[60];
                char texto_tempo[60];
                char texto_fim[60];
                int segundos_fim = TEMPO_LIMITE - tempo_jogo / 60;

                sprintf_s(texto_portas, sizeof(texto_portas), "Portas abertas: %d/%d  (+%d)", portas_abertas, contar_portas_total(), portas_abertas * PONTOS_PORTA);
                sprintf_s(texto_tempo, sizeof(texto_tempo), "Tempo restante: %02d:%02d  (+%d)", segundos_fim / 60, segundos_fim % 60, bonus_tempo);
                sprintf_s(texto_fim, sizeof(texto_fim), "Pontuacao total: %d", pontos);

                al_draw_text(fonte, al_map_rgb(200, 200, 200), 640, 248, ALLEGRO_ALIGN_CENTER, texto_portas);
                al_draw_text(fonte, al_map_rgb(200, 200, 200), 640, 264, ALLEGRO_ALIGN_CENTER, texto_tempo);
                al_draw_text(fonte, al_map_rgb(255, 255, 255), 640, 284, ALLEGRO_ALIGN_CENTER, texto_fim);

                for (int k = 0; k < 2; k++)
                    desenhar_botao_menu(fonte, 540, 330 + k * 80, 740, 390 + k * 80, rotulos_fim[k], opcao_fim == k);

                al_draw_text(fonte, al_map_rgb(200, 200, 200), 640, 600, ALLEGRO_ALIGN_CENTER, "[SETAS] Escolher   [ENTER] Confirmar");

                al_flip_display();
            }

            continue;
        }

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
            if (ev.keyboard.keycode == ALLEGRO_KEY_F12)
            {
                carregar_mapa((mapa_atual + 1) % QTD_MAPAS);
                personagem_x = 80;
                personagem_y = 60;
                direcao = DIR_BAIXO;
                andando = false;
                anim_andar = 0;
                tempo_jogo = 0;
                conversando = false;
                perguntando = false;
                caderno_aberto = false;
                pagina_fala = 0;
                pagina_caderno = 0;
                falas_lidas = 0;
                resposta[0] = '\0';
                tamanho_resposta = 0;
                mensagem[0] = '\0';
                aviso_porta = 0;
                aviso_tempo = 0;
                aviso_mapa = 0;
            }

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
                    if (atoi(resposta) == perguntas[mapa_atual][pergunta_atual].resposta)
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

            if (andando)
                anim_andar++;
            else
                anim_andar = 0;

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

                    if (fechar_porta_aleatoria())
                        aviso_porta = 180;

                    conversando = false;
                    perguntando = false;
                    caderno_aberto = false;
                    resposta[0] = '\0';
                    tamanho_resposta = 0;
                    mensagem[0] = '\0';
                }
            }

            if (!conversando && !caderno_aberto)
                tempo_jogo++;

            if (tempo_jogo >= TEMPO_LIMITE * 60)
            {
                reiniciar_mapa();
                reiniciar_inspetores();

                personagem_x = 80;
                personagem_y = 60;
                direcao = DIR_BAIXO;
                andando = false;
                anim_andar = 0;
                conversando = false;
                perguntando = false;
                caderno_aberto = false;
                resposta[0] = '\0';
                tamanho_resposta = 0;
                mensagem[0] = '\0';
                tempo_jogo = 0;
                aviso_porta = 0;
                aviso_tempo = 240;
            }

            int coluna_atual = (int)(personagem_x + CELL_W / 2) / TILE_SIZE;
            int linha_atual = (int)(personagem_y + CELL_H / 2) / TILE_SIZE;

            if (linha_atual == saida_linha && coluna_atual == saida_coluna)
            {
                int segundos = tempo_jogo / 60;

                portas_abertas = contar_portas_abertas();
                bonus_tempo = (TEMPO_LIMITE - segundos) * PONTOS_SEGUNDO;
                if (bonus_tempo < 0)
                    bonus_tempo = 0;

                int pontos_mapa = portas_abertas * PONTOS_PORTA + bonus_tempo;

                if (mapa_atual < QTD_MAPAS - 1)
                {
                    pontos_acumulados += pontos_mapa;
                    carregar_mapa(mapa_atual + 1);

                    personagem_x = 80;
                    personagem_y = 60;
                    direcao = DIR_BAIXO;
                    andando = false;
                    anim_andar = 0;
                    conversando = false;
                    perguntando = false;
                    caderno_aberto = false;
                    resposta[0] = '\0';
                    tamanho_resposta = 0;
                    mensagem[0] = '\0';
                    tempo_jogo = 0;
                    aviso_porta = 0;
                    aviso_tempo = 0;
                    aviso_mapa = 240;
                    pagina_fala = 0;
                    pagina_caderno = 0;
                    falas_lidas = 0;
                }
                else
                {
                    pontos = pontos_acumulados + pontos_mapa;
                    fim_de_jogo = true;
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

            if (aviso_porta > 0)
                aviso_porta--;
            if (aviso_tempo > 0)
                aviso_tempo--;
            if (aviso_mapa > 0)
                aviso_mapa--;

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

            const Tema* tema_atual = &tema_pedra;
            if (mapas[mapa_atual].tema == TEMA_BIBLIOTECA)
                tema_atual = &tema_biblioteca;
            else if (mapas[mapa_atual].tema == TEMA_LABORATORIO)
                tema_atual = &tema_laboratorio;

            desenhar_mapa(camera_x, camera_y, tema_atual);

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

            int quadro = 1;
            float larg = CELL_H * 56.0 / 100.0;
            float off_x = (CELL_W - larg) / 2;

            if (andando)
                quadro = (anim_andar / 8) % 4;

            al_draw_scaled_bitmap(personagem_sprites, quadro * 56, direcao * 100, 56, 100, personagem_x - camera_x + off_x, personagem_y - camera_y, larg, CELL_H, 0);

            al_draw_text(fonte, al_map_rgb(255, 255, 255), 1270, 10, ALLEGRO_ALIGN_RIGHT, "[N] Caderno");

            char texto_hud[80];
            int segundos_hud = TEMPO_LIMITE - tempo_jogo / 60;
            ALLEGRO_COLOR cor_hud = al_map_rgb(255, 255, 255);

            if (segundos_hud <= 30)
                cor_hud = al_map_rgb(255, 80, 80);

            sprintf_s(texto_hud, sizeof(texto_hud), "Mapa %d/%d   Portas: %d/%d   Tempo: %02d:%02d", mapa_atual + 1, QTD_MAPAS, contar_portas_abertas(), contar_portas_total(), segundos_hud / 60, segundos_hud % 60);
            al_draw_text(fonte, cor_hud, 10, 10, ALLEGRO_ALIGN_LEFT, texto_hud);

            if (aviso_porta > 0)
                al_draw_text(fonte, al_map_rgb(255, 120, 120), 640, 20, ALLEGRO_ALIGN_CENTER, "O inspetor te pegou e fechou uma porta do labirinto!");

            if (aviso_tempo > 0)
                al_draw_text(fonte, al_map_rgb(255, 160, 60), 640, 45, ALLEGRO_ALIGN_CENTER, "O TEMPO ACABOU! As portas foram fechadas e a fase recomecou.");

            if (aviso_mapa > 0)
            {
                const char* nome_mapa = (mapa_atual == 1) ? "MAPA 2 - BIBLIOTECA" : "MAPA 3 - LABORATORIO";
                al_draw_text(fonte, al_map_rgb(120, 220, 255), 640, 70, ALLEGRO_ALIGN_CENTER, nome_mapa);
            }

            if (conversando && !perguntando)
            {
                char numero_pagina[20];

                al_draw_filled_rectangle(140, 570, 1140, 700, al_map_rgb(35, 35, 45));
                al_draw_rectangle(140, 570, 1140, 700, al_map_rgb(220, 220, 220), 2);
                al_draw_text(fonte, al_map_rgb(255, 255, 0), 160, 585, ALLEGRO_ALIGN_LEFT, "PROFESSOR");
                al_draw_multiline_text(fonte, al_map_rgb(255, 255, 255), 160, 610, 960, 14, ALLEGRO_ALIGN_LEFT, falas[mapa_atual][pagina_fala]);

                sprintf_s(numero_pagina, sizeof(numero_pagina), "%d/%d", pagina_fala + 1, QTD_FALAS);

                al_draw_text(fonte, al_map_rgb(200, 200, 200), 160, 680, ALLEGRO_ALIGN_LEFT, numero_pagina);
                al_draw_text(fonte, al_map_rgb(200, 200, 200), 1120, 680, ALLEGRO_ALIGN_RIGHT, "[E] Continuar   [ESC] Fechar");
            }

            if (perguntando)
            {
                al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(0, 0, 0, 180));
                al_draw_filled_rectangle(300, 150, 980, 570, al_map_rgb(35, 35, 45));
                al_draw_rectangle(300, 150, 980, 570, al_map_rgb(220, 220, 220), 2);

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 640, 170, ALLEGRO_ALIGN_CENTER, "PORTA BLOQUEADA - RESOLVA A EQUACAO");
                al_draw_text(fonte, al_map_rgb(255, 255, 0), 640, 205, ALLEGRO_ALIGN_CENTER, perguntas[mapa_atual][pergunta_atual].enunciado);

                const char* subtitulo = "Qual o resultado?";
                if (mapa_atual == 0)
                    subtitulo = "Qual o valor de x?";

                al_draw_text(fonte, al_map_rgb(200, 200, 200), 640, 225, ALLEGRO_ALIGN_CENTER, subtitulo);

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
                    al_draw_multiline_text(fonte, al_map_rgb(40, 30, 20), 280, 200, 720, 16, ALLEGRO_ALIGN_LEFT, falas[mapa_atual][pagina_caderno]);
                }

                al_draw_text(fonte, al_map_rgb(110, 70, 30), 640, 590, ALLEGRO_ALIGN_CENTER, "[<] [>] Trocar anotacao   [N] ou [ESC] Fechar");
            }

            al_flip_display();
        }
    }

    if (tem_professor)
        al_destroy_bitmap(prof_sprites);

    if (tile_porta_saida != NULL)
        al_destroy_bitmap(tile_porta_saida);

    al_destroy_bitmap(lab_chao2);
    al_destroy_bitmap(lab_chao);
    al_destroy_bitmap(lab_porta_saida);
    al_destroy_bitmap(lab_porta);
    al_destroy_bitmap(lab_bancada);
    al_destroy_bitmap(lab_parede);
    al_destroy_bitmap(esc_quadro);
    al_destroy_bitmap(esc_estante);
    al_destroy_bitmap(esc_chao);
    al_destroy_bitmap(esc_porta);
    al_destroy_bitmap(esc_parede);
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