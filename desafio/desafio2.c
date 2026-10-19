// Desafio 2 - Pincel do mouse
// PIF - CESAR School - 2026.2
// Equipe: Breno Souto, Bruno Rodrigues, Joao Fernando e Vinicius Beleza
//
// Um circulo segue o cursor. Clique esquerdo sorteia uma cor nova (uma vez por clique)
// e a roda do mouse aumenta ou diminui o raio, sempre entre 5 e 100.
// Extra: programa de pintura. Segurando o botao direito, cada posicao vira uma
// "pincelada" guardada num vetor de ate 500; C limpa a tela.

#include "raylib.h"

#define LARGURA 800
#define ALTURA  450

#define RAIO_MIN        5.0f
#define RAIO_MAX        100.0f
#define RAIO_INICIAL    30.0f
#define PASSO_RODA      5.0f            // quanto cada "clique" da roda muda o raio
#define MAX_PINCELADAS  500

typedef struct {
    Vector2 pos;
    float raio;
    Color cor;
} Pincel;

typedef struct {
    Pincel marcas[MAX_PINCELADAS];
    int quantidade;                     // quantas posicoes do vetor estao em uso
} Pintura;

// ---------- atualizar: muda o estado, nao desenha ----------
Color corAleatoria(void) {
    return (Color){ (unsigned char)GetRandomValue(0, 255),
                    (unsigned char)GetRandomValue(0, 255),
                    (unsigned char)GetRandomValue(0, 255), 255 };
}

float limitaRaio(float raio) {
    if (raio < RAIO_MIN) return RAIO_MIN;
    if (raio > RAIO_MAX) return RAIO_MAX;
    return raio;
}

void atualizaPincel(Pincel *p) {
    p->pos = GetMousePosition();                                     // segue o mouse em todo quadro
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) p->cor = corAleatoria();   // 1 troca por clique
    p->raio = limitaRaio(p->raio + GetMouseWheelMove() * PASSO_RODA);
}

void guardaPincelada(Pintura *pintura, Pincel p) {
    if (pintura->quantidade >= MAX_PINCELADAS) return;               // vetor cheio
    if (pintura->quantidade > 0) {                                    // mouse parado: nao repete
        Pincel ultima = pintura->marcas[pintura->quantidade - 1];
        if (ultima.pos.x == p.pos.x && ultima.pos.y == p.pos.y && ultima.raio == p.raio) return;
    }
    pintura->marcas[pintura->quantidade] = p;
    pintura->quantidade++;
}

// ---------- desenhar: so le o estado ----------
void desenhaPintura(const Pintura *pintura) {
    for (int i = 0; i < pintura->quantidade; i++)
        DrawCircleV(pintura->marcas[i].pos, pintura->marcas[i].raio, pintura->marcas[i].cor);
}

void desenhaInterface(Pincel p, int pinceladas) {
    DrawRectangle(0, 0, LARGURA, 34, Fade(RAYWHITE, 0.85f));
    DrawText(TextFormat("Raio: %.0f", p.raio), 10, 8, 20, DARKGRAY);
    DrawText(TextFormat("Pinceladas: %d/%d", pinceladas, MAX_PINCELADAS), 140, 8, 20, DARKGRAY);
    DrawText("Esq: cor | Roda: raio | Dir: pintar | C: limpar", 380, 10, 16, GRAY);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Desafio 2 - Pincel do mouse");
    SetTargetFPS(60);

    Pincel pincel = { { LARGURA / 2.0f, ALTURA / 2.0f }, RAIO_INICIAL, MAROON };
    static Pintura pintura = { .quantidade = 0 };                    // static: vetor grande fora da pilha

    while (!WindowShouldClose()) {
        // 1. ENTRADA + 2. ATUALIZAR
        atualizaPincel(&pincel);
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) guardaPincelada(&pintura, pincel);
        if (IsKeyPressed(KEY_C)) pintura.quantidade = 0;

        // 3. DESENHAR
        BeginDrawing();
            ClearBackground(RAYWHITE);
            desenhaPintura(&pintura);
            DrawCircleV(pincel.pos, pincel.raio, pincel.cor);         // o pincel fica por cima da pintura
            DrawCircleLines((int)pincel.pos.x, (int)pincel.pos.y, pincel.raio, DARKGRAY);
            desenhaInterface(pincel, pintura.quantidade);              // interface por ultimo
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
