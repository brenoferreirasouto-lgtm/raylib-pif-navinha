// Desafio 1 - Minha cena
// PIF - CESAR School - 2026.2
// Equipe: Breno Souto, Bruno Rodrigues, Joao Fernando e Vinicius Beleza
//
// Paisagem parada: ceu, chao, casa com telhado, sol, arvore e cerca.
// Extra: o sol "pulsa", com o raio variando com GetTime().

#include "raylib.h"
#include <math.h>

#define LARGURA 800
#define ALTURA  450

#define ALTURA_CHAO     120                 // altura da faixa de grama
#define Y_CHAO          (ALTURA - ALTURA_CHAO)

#define RAIO_SOL_BASE   45.0f
#define PULSO_SOL       6.0f                // quanto o raio cresce/encolhe
#define VELOC_PULSO     2.5f                // velocidade do pulso

// cores criadas pela equipe em RGBA
#define COR_CEU       (Color){ 135, 206, 235, 255 }
#define COR_GRAMA     (Color){  76, 153,  60, 255 }
#define COR_PAREDE    (Color){ 230, 200, 150, 255 }
#define COR_TELHADO   (Color){ 170,  60,  45, 255 }
#define COR_PORTA     (Color){ 110,  70,  40, 255 }
#define COR_VIDRO     (Color){ 190, 230, 255, 255 }
#define COR_TRONCO    (Color){ 120,  80,  45, 255 }
#define COR_FOLHAS    (Color){  40, 120,  50, 255 }
#define COR_CERCA     (Color){ 245, 245, 235, 255 }

// ---------- atualizar: so calcula valores ----------
float raioDoSol(double tempo) {
    return RAIO_SOL_BASE + PULSO_SOL * sinf((float)tempo * VELOC_PULSO);
}

// ---------- desenhar: so desenha ----------
void desenhaCeuEChao(void) {
    DrawRectangle(0, 0, LARGURA, Y_CHAO, COR_CEU);                       // retangulo 1
    DrawRectangle(0, Y_CHAO, LARGURA, ALTURA_CHAO, COR_GRAMA);           // retangulo 2
    DrawLine(0, Y_CHAO, LARGURA, Y_CHAO, DARKGREEN);                     // linha do horizonte
}

void desenhaSol(float raio) {
    DrawCircle(680, 80, raio, GOLD);                                     // circulo 1
}

void desenhaCasa(void) {
    int x = 220, largura = 200, altura = 140;
    int y = Y_CHAO - altura;

    DrawRectangle(x, y, largura, altura, COR_PAREDE);                    // parede
    // telhado: vertices em sentido anti-horario (ponta, base esquerda, base direita)
    DrawTriangle((Vector2){ x + largura / 2.0f, y - 80 },
                 (Vector2){ x - 20.0f, y },
                 (Vector2){ x + largura + 20.0f, y }, COR_TELHADO);      // triangulo
    DrawRectangle(x + 80, Y_CHAO - 70, 40, 70, COR_PORTA);               // porta
    DrawRectangle(x + 20, y + 30, 40, 35, COR_VIDRO);                    // janela esquerda
    DrawRectangle(x + 140, y + 30, 40, 35, COR_VIDRO);                   // janela direita
}

void desenhaArvore(void) {
    DrawRectangle(560, Y_CHAO - 90, 24, 90, COR_TRONCO);                 // tronco
    DrawCircle(572, Y_CHAO - 110, 45, COR_FOLHAS);                       // copa
    DrawCircle(545, Y_CHAO - 85, 30, COR_FOLHAS);
    DrawCircle(600, Y_CHAO - 85, 30, COR_FOLHAS);
}

void desenhaCerca(void) {
    for (int x = 20; x < 200; x += 25)
        DrawLine(x, Y_CHAO - 40, x, Y_CHAO, COR_CERCA);                  // estacas
    DrawLine(20, Y_CHAO - 30, 195, Y_CHAO - 30, COR_CERCA);              // travessas
    DrawLine(20, Y_CHAO - 12, 195, Y_CHAO - 12, COR_CERCA);
}

void desenhaNomes(void) {
    DrawText("Breno Souto, Bruno Rodrigues,", 10, ALTURA - 50, 20, RAYWHITE);
    DrawText("Joao Fernando e Vinicius Beleza", 10, ALTURA - 26, 20, RAYWHITE);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Desafio 1 - Minha cena");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // 1. ENTRADA + 2. ATUALIZAR
        float raioSol = raioDoSol(GetTime());

        // 3. DESENHAR: fundo primeiro, objetos no meio, nomes por ultimo (por cima de tudo)
        BeginDrawing();
            ClearBackground(COR_CEU);
            desenhaCeuEChao();
            desenhaSol(raioSol);
            desenhaCasa();
            desenhaArvore();
            desenhaCerca();
            desenhaNomes();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
