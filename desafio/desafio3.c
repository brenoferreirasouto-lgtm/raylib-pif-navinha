// Desafio 3 - Personagem livre
// PIF - CESAR School - 2026.2
// Equipe: Breno Souto, Bruno Rodrigues, Joao Fernando e Vinicius Beleza
//
// Um quadrado anda com WASD a 250 px/s; segurando Shift, corre a 500 px/s.
// Ele nunca sai da tela.
// Dica resolvida: a direcao e normalizada, entao na diagonal ele anda na mesma
// velocidade que em linha reta (sem o bonus de ~1,41x).

#include "raylib.h"
#include <math.h>

#define LARGURA 800
#define ALTURA  450

#define TAMANHO         40.0f
#define VEL_ANDAR       250.0f      // pixels por segundo
#define VEL_CORRER      500.0f

typedef struct {
    Rectangle rec;
    float velocidadeAtual;          // 0 parado, 250 andando, 500 correndo
} Personagem;

// ---------- atualizar: muda o estado, nao desenha ----------
Vector2 direcaoDoTeclado(void) {
    Vector2 dir = { 0, 0 };
    if (IsKeyDown(KEY_D)) dir.x += 1;
    if (IsKeyDown(KEY_A)) dir.x -= 1;
    if (IsKeyDown(KEY_S)) dir.y += 1;               // y cresce para baixo
    if (IsKeyDown(KEY_W)) dir.y -= 1;

    float tamanho = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (tamanho > 0) {                              // normaliza: diagonal com tamanho 1
        dir.x /= tamanho;
        dir.y /= tamanho;
    }
    return dir;
}

void limitaNaTela(Rectangle *r) {
    if (r->x < 0)                   r->x = 0;
    if (r->y < 0)                   r->y = 0;
    if (r->x > LARGURA - r->width)  r->x = LARGURA - r->width;
    if (r->y > ALTURA - r->height)  r->y = ALTURA - r->height;
}

void atualizaPersonagem(Personagem *p, float dt) {
    bool correndo = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    float velocidade = correndo ? VEL_CORRER : VEL_ANDAR;
    Vector2 dir = direcaoDoTeclado();

    p->rec.x += dir.x * velocidade * dt;            // todo movimento multiplicado por dt
    p->rec.y += dir.y * velocidade * dt;
    limitaNaTela(&p->rec);

    bool movendo = (dir.x != 0 || dir.y != 0);
    p->velocidadeAtual = movendo ? velocidade : 0;
}

// ---------- desenhar: so le o estado ----------
void desenhaPersonagem(Personagem p) {
    Color cor = (p.velocidadeAtual == VEL_CORRER) ? ORANGE : BLUE;
    DrawRectangleRec(p.rec, cor);
}

void desenhaInterface(Personagem p) {
    const char *modo = (p.velocidadeAtual == 0) ? "parado"
                     : (p.velocidadeAtual == VEL_CORRER) ? "correndo" : "andando";
    DrawText(TextFormat("Velocidade: %.0f px/s (%s)", p.velocidadeAtual, modo), 10, 10, 20, DARKGRAY);
    DrawText("WASD: mover | Shift: correr", 10, ALTURA - 28, 18, GRAY);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Desafio 3 - Personagem livre");
    SetTargetFPS(60);

    Personagem jogador = { { LARGURA / 2.0f - TAMANHO / 2, ALTURA / 2.0f - TAMANHO / 2, TAMANHO, TAMANHO }, 0 };

    while (!WindowShouldClose()) {
        // 1. ENTRADA + 2. ATUALIZAR
        float dt = GetFrameTime();
        atualizaPersonagem(&jogador, dt);

        // 3. DESENHAR
        BeginDrawing();
            ClearBackground(RAYWHITE);
            desenhaPersonagem(jogador);
            desenhaInterface(jogador);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
