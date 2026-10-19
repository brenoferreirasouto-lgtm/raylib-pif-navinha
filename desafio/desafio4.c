// Desafio 4 - Clique no alvo
// PIF - CESAR School - 2026.2
// Equipe: Breno Souto, Bruno Rodrigues, Joao Fernando e Vinicius Beleza
//
// Um alvo aparece numa posicao aleatoria. Clicou nele: +1 ponto, ele muda de
// lugar e encolhe um pouco. A partida dura 30 segundos; R reinicia.

#include "raylib.h"
#include <math.h>

#define LARGURA 800
#define ALTURA  450

#define TEMPO_PARTIDA   30.0f       // segundos
#define RAIO_INICIAL    40.0f
#define RAIO_MINIMO     10.0f
#define FATOR_ENCOLHE   0.9f        // a cada acerto o raio fica 10% menor
#define ALTURA_HUD      40          // faixa do topo reservada para o texto

typedef enum { JOGANDO, FIM } Estado;

typedef struct {
    Vector2 centro;
    float raio;
} Alvo;

typedef struct {
    Estado estado;
    Alvo alvo;
    int pontos;
    int erros;                      // cliques fora do alvo
    float tempo;                    // tempo restante, em segundos
} Jogo;

// ---------- atualizar: muda o estado, nao desenha ----------
void sorteiaPosicao(Alvo *a) {
    // o alvo inteiro fica dentro da tela e abaixo do placar
    a->centro.x = (float)GetRandomValue((int)a->raio, LARGURA - (int)a->raio);
    a->centro.y = (float)GetRandomValue(ALTURA_HUD + (int)a->raio, ALTURA - (int)a->raio);
}

void reiniciaJogo(Jogo *j) {
    j->estado = JOGANDO;
    j->pontos = 0;
    j->erros = 0;
    j->tempo = TEMPO_PARTIDA;
    j->alvo.raio = RAIO_INICIAL;
    sorteiaPosicao(&j->alvo);
}

void trataClique(Jogo *j, Vector2 mouse) {
    if (CheckCollisionPointCircle(mouse, j->alvo.centro, j->alvo.raio)) {
        j->pontos++;
        j->alvo.raio *= FATOR_ENCOLHE;
        if (j->alvo.raio < RAIO_MINIMO) j->alvo.raio = RAIO_MINIMO;
        sorteiaPosicao(&j->alvo);
    } else {
        j->erros++;                                     // clique fora nao pontua
    }
}

void atualizaJogo(Jogo *j, float dt) {
    switch (j->estado) {
        case JOGANDO:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) trataClique(j, GetMousePosition());
            j->tempo -= dt;                             // cronometro regressivo
            if (j->tempo <= 0) {
                j->tempo = 0;
                j->estado = FIM;
            }
            break;
        case FIM:
            if (IsKeyPressed(KEY_R)) reiniciaJogo(j);
            break;
    }
}

// ---------- desenhar: so le o estado ----------
void desenhaAlvo(Alvo a) {
    DrawCircleV(a.centro, a.raio, RED);
    DrawCircleV(a.centro, a.raio * 0.66f, RAYWHITE);
    DrawCircleV(a.centro, a.raio * 0.33f, RED);
}

void desenhaJogo(const Jogo *j) {
    if (j->estado == JOGANDO) {
        desenhaAlvo(j->alvo);
        DrawText(TextFormat("Tempo: %d s", (int)ceilf(j->tempo)), 10, 10, 24, DARKGRAY);
        DrawText(TextFormat("Pontos: %d", j->pontos), 640, 10, 24, DARKGRAY);
    } else {
        const char *fim = TextFormat("Fim! Pontos: %d", j->pontos);
        DrawText(fim, LARGURA / 2 - MeasureText(fim, 40) / 2, 170, 40, MAROON);
        const char *erros = TextFormat("Cliques fora do alvo: %d", j->erros);
        DrawText(erros, LARGURA / 2 - MeasureText(erros, 20) / 2, 230, 20, DARKGRAY);
        DrawText("R para jogar de novo", LARGURA / 2 - MeasureText("R para jogar de novo", 20) / 2, 270, 20, GRAY);
    }
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Desafio 4 - Clique no alvo");
    SetTargetFPS(60);

    Jogo jogo;
    reiniciaJogo(&jogo);

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
