// Desafio 5 - Coletor de moedas (partindo do molde.c)
// PIF - CESAR School - 2026.2
// Equipe: Breno Souto, Bruno Rodrigues, Joao Fernando e Vinicius Beleza
//
// 10 moedas espalhadas e um jogador que anda nas 4 direcoes. Encostou numa moeda,
// ela some e vale 1 ponto. Pegou todas: vitoria, com o tempo total gasto.
// Extra: um inimigo persegue o jogador; se encostar, derrota.

#include "raylib.h"
#include <math.h>

#define LARGURA 800
#define ALTURA  450
#define MAX_ITENS 10                                 // moedas

#define VEL_JOGADOR     250.0f                       // pixels por segundo
#define VEL_INIMIGO     110.0f                       // mais lento que o jogador, para ser justo
#define RAIO_MOEDA      10.0f
#define RAIO_INIMIGO    16.0f
#define ALTURA_HUD      40                           // faixa do topo com o texto

typedef enum { MENU, JOGANDO, FIM } Estado;          // telas do jogo

typedef struct {
    Rectangle rec;
    float velocidade;                                // pixels por segundo
} Jogador;

typedef struct {
    Vector2 pos;
    float raio;
    bool ativo;                                      // false = some do jogo
} Item;

typedef struct {
    Vector2 pos;
    float raio;
    float velocidade;
} Inimigo;

typedef struct {
    Estado estado;
    Jogador jogador;
    Item itens[MAX_ITENS];
    Inimigo inimigo;
    int pontos;
    float tempoGasto;                                // segundos desde o inicio da partida
    bool venceu;                                     // resultado mostrado na tela FIM
} Jogo;

// ---------- logica (atualizar): muda o estado, nao desenha ----------
void reiniciaJogo(Jogo *j) {
    j->jogador = (Jogador){ { LARGURA / 2.0f - 15, ALTURA - 60, 30, 30 }, VEL_JOGADOR };
    j->inimigo = (Inimigo){ { 40, ALTURA_HUD + 30.0f }, RAIO_INIMIGO, VEL_INIMIGO };
    for (int i = 0; i < MAX_ITENS; i++)
        j->itens[i] = (Item){ { (float)GetRandomValue(30, LARGURA - 30),
                                (float)GetRandomValue(ALTURA_HUD + 30, ALTURA - 100) },
                              RAIO_MOEDA, true };
    j->pontos = 0;
    j->tempoGasto = 0;
    j->venceu = false;
}

void atualizaJogador(Jogador *j, float dt) {
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  j->rec.x -= j->velocidade * dt;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) j->rec.x += j->velocidade * dt;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    j->rec.y -= j->velocidade * dt;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  j->rec.y += j->velocidade * dt;

    if (j->rec.x < 0) j->rec.x = 0;
    if (j->rec.y < ALTURA_HUD) j->rec.y = ALTURA_HUD;
    if (j->rec.x > LARGURA - j->rec.width)  j->rec.x = LARGURA - j->rec.width;
    if (j->rec.y > ALTURA - j->rec.height)  j->rec.y = ALTURA - j->rec.height;
}

// encostou numa moeda ativa: ela some e vale 1 ponto
void coletaMoedas(Item itens[], int n, Rectangle jogador, int *pontos) {
    for (int i = 0; i < n; i++) {
        if (itens[i].ativo && CheckCollisionCircleRec(itens[i].pos, itens[i].raio, jogador)) {
            itens[i].ativo = false;
            (*pontos)++;
        }
    }
}

// extra: a cada quadro o inimigo anda um pouco em direcao ao centro do jogador
void atualizaInimigo(Inimigo *in, Rectangle alvo, float dt) {
    float dx = (alvo.x + alvo.width / 2) - in->pos.x;
    float dy = (alvo.y + alvo.height / 2) - in->pos.y;
    float distancia = sqrtf(dx * dx + dy * dy);
    if (distancia > 1) {
        in->pos.x += dx / distancia * in->velocidade * dt;
        in->pos.y += dy / distancia * in->velocidade * dt;
    }
}

int contaAtivos(const Item itens[], int n) {         // "fold": agrega a lista num numero
    int total = 0;
    for (int i = 0; i < n; i++)
        if (itens[i].ativo) total++;
    return total;
}

void atualizaJogo(Jogo *j, float dt) {
    switch (j->estado) {
        case MENU:
            if (IsKeyPressed(KEY_ENTER)) { reiniciaJogo(j); j->estado = JOGANDO; }
            break;
        case JOGANDO:
            j->tempoGasto += dt;
            atualizaJogador(&j->jogador, dt);
            coletaMoedas(j->itens, MAX_ITENS, j->jogador.rec, &j->pontos);
            atualizaInimigo(&j->inimigo, j->jogador.rec, dt);

            if (contaAtivos(j->itens, MAX_ITENS) == 0) {                         // pegou todas
                j->venceu = true;
                j->estado = FIM;
            } else if (CheckCollisionCircleRec(j->inimigo.pos, j->inimigo.raio, j->jogador.rec)) {
                j->venceu = false;                                               // pego pelo inimigo
                j->estado = FIM;
            }
            break;
        case FIM:
            if (IsKeyPressed(KEY_R)) j->estado = MENU;
            break;
    }
}

// ---------- desenho: so le o estado ----------
void desenhaJogador(Jogador j) { DrawRectangleRec(j.rec, BLUE); }

void desenhaItens(const Item itens[], int n) {
    for (int i = 0; i < n; i++)
        if (itens[i].ativo) DrawCircleV(itens[i].pos, itens[i].raio, GOLD);
}

void desenhaInimigo(Inimigo in) { DrawCircleV(in.pos, in.raio, MAROON); }

void textoCentralizado(const char *texto, int y, int tamanho, Color cor) {
    DrawText(texto, LARGURA / 2 - MeasureText(texto, tamanho) / 2, y, tamanho, cor);
}

void desenhaJogo(const Jogo *j) {
    if (j->estado == MENU) {
        textoCentralizado("COLETOR DE MOEDAS", 140, 40, DARKBLUE);
        textoCentralizado("Pegue as 10 moedas e fuja do inimigo vermelho", 210, 20, DARKGRAY);
        textoCentralizado("WASD ou setas: mover", 245, 20, GRAY);
        textoCentralizado("ENTER para comecar", 290, 20, GRAY);
    } else if (j->estado == JOGANDO) {
        desenhaItens(j->itens, MAX_ITENS);
        desenhaJogador(j->jogador);
        desenhaInimigo(j->inimigo);
        DrawText(TextFormat("Pontos: %d", j->pontos), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Restam: %d", contaAtivos(j->itens, MAX_ITENS)), 160, 10, 20, DARKGRAY);
        DrawText(TextFormat("Tempo: %.1f s", j->tempoGasto), 640, 10, 20, DARKGRAY);
    } else {
        if (j->venceu) {
            textoCentralizado("VITORIA!", 150, 50, DARKGREEN);
            textoCentralizado(TextFormat("Tempo total: %.1f s", j->tempoGasto), 220, 24, DARKGRAY);
        } else {
            textoCentralizado("O INIMIGO TE PEGOU!", 150, 40, MAROON);
            textoCentralizado(TextFormat("Moedas: %d de %d em %.1f s", j->pontos, MAX_ITENS, j->tempoGasto), 220, 24, DARKGRAY);
        }
        textoCentralizado("R para voltar ao menu", 280, 20, GRAY);
    }
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Desafio 5 - Coletor de moedas");
    SetTargetFPS(60);

    Jogo jogo;
    reiniciaJogo(&jogo);
    jogo.estado = MENU;

    while (!WindowShouldClose()) {
        // 1. ENTRADA + 2. ATUALIZAR
        atualizaJogo(&jogo, GetFrameTime());

        // 3. DESENHAR
        BeginDrawing();
            ClearBackground(RAYWHITE);
            desenhaJogo(&jogo);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
