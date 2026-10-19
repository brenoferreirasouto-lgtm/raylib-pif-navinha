// Desafio 6 - Navinha que atira
// PIF - CESAR School - 2026.2
// Equipe: Breno Souto, Bruno Rodrigues, Joao Fernando e Vinicius Beleza
//
// Uma nave (triangulo) anda na horizontal, embaixo da tela. Cada toque em Espaco
// dispara UM projetil, que sobe. Alvos atravessam a tela no alto; acertou, ganha ponto.
//
// Criterios do enunciado:
//   - projeteis num vetor fixo (tiros[MAX_TIROS]), reaproveitando os inativos
//   - um tiro por toque: IsKeyPressed
//   - projetil que sai da tela vira inativo
//   - um laco atualiza todos os projeteis com a mesma funcao (map): atualizaTiro
// Extra do enunciado: segurando Espaco, no maximo 1 tiro a cada 0,3 s (temporizador).
// Ideias da equipe: partida de 60 s, dificuldade crescente, alvos com valores
// diferentes, recorde e precisao dos tiros.

#include "raylib.h"
#include <math.h>

#define LARGURA 800
#define ALTURA  450

#define MAX_TIROS           20
#define MAX_ALVOS           5
#define MAX_ESTRELAS        60

#define VEL_NAVE            350.0f      // pixels por segundo
#define LARGURA_NAVE        40.0f
#define ALTURA_NAVE         30.0f

#define VEL_TIRO            520.0f
#define LARGURA_TIRO        4.0f
#define ALTURA_TIRO         12.0f

#define TEMPO_PARTIDA       60.0f       // segundos
#define RECARGA_TIRO        0.3f        // extra: intervalo do tiro segurando Espaco
#define TIRO_AUTOMATICO     true        // false desliga o extra (fica so 1 tiro por toque)

#define ALTURA_HUD          40          // faixa do topo com o placar
#define Y_MIN_ALVO          (ALTURA_HUD + 30)
#define Y_MAX_ALVO          210         // alvos ficam "no alto" da tela

#define PONTOS_POR_NIVEL    10          // a cada 10 pontos os alvos aceleram
#define AUMENTO_POR_NIVEL   0.15f       // +15% de velocidade por nivel

typedef enum { MENU, JOGANDO, FIM } Estado;

typedef struct {
    Vector2 pos;                        // centro da base do triangulo
    float velocidade;
} Nave;

typedef struct {
    Vector2 pos;                        // canto superior esquerdo do projetil
    float vel;                          // pixels por segundo, para cima
    bool ativo;                         // false = livre para ser reaproveitado
} Projetil;

typedef struct {
    Vector2 pos;
    float raio;
    float vel;                          // horizontal: positivo vai para a direita
    int valor;                          // pontos que o alvo vale
} Alvo;

typedef struct {
    Estado estado;
    Nave nave;
    Projetil tiros[MAX_TIROS];
    Alvo alvos[MAX_ALVOS];
    int pontos;
    int recorde;                        // nao e zerado no reinicio: vale enquanto o jogo estiver aberto
    int tirosDisparados;
    int acertos;
    float tempo;                        // tempo restante da partida
    float recarga;                      // extra: tempo ate poder atirar de novo segurando Espaco
} Jogo;

// =====================================================================
// ATUALIZAR: funcoes que mudam o estado (nao desenham nada)
// =====================================================================

// ---------- nave ----------
void atualizaNave(Nave *n, float dt) {
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  n->pos.x -= n->velocidade * dt;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) n->pos.x += n->velocidade * dt;

    if (n->pos.x < LARGURA_NAVE / 2)           n->pos.x = LARGURA_NAVE / 2;
    if (n->pos.x > LARGURA - LARGURA_NAVE / 2) n->pos.x = LARGURA - LARGURA_NAVE / 2;
}

// ---------- tiros ----------
// procura o primeiro tiro inativo e o coloca na ponta da nave;
// devolve false se os MAX_TIROS ja estao no ar
bool disparaTiro(Projetil tiros[], int n, Nave nave) {
    for (int i = 0; i < n; i++) {
        if (!tiros[i].ativo) {
            tiros[i].pos = (Vector2){ nave.pos.x - LARGURA_TIRO / 2, nave.pos.y - ALTURA_NAVE - ALTURA_TIRO };
            tiros[i].vel = VEL_TIRO;
            tiros[i].ativo = true;
            return true;
        }
    }
    return false;
}

// funcao aplicada a cada projetil do vetor (map)
void atualizaTiro(Projetil *t, float dt) {
    if (!t->ativo) return;
    t->pos.y -= t->vel * dt;
    if (t->pos.y + ALTURA_TIRO < 0) t->ativo = false;   // saiu pelo topo: vira inativo
}

Rectangle retanguloDoTiro(Projetil t) {
    return (Rectangle){ t.pos.x, t.pos.y, LARGURA_TIRO, ALTURA_TIRO };
}

void trataDisparo(Jogo *j, float dt) {
    j->recarga -= dt;

    bool atirou = false;
    if (IsKeyPressed(KEY_SPACE)) {                                  // 1 tiro por toque
        atirou = disparaTiro(j->tiros, MAX_TIROS, j->nave);
        j->recarga = RECARGA_TIRO;
    } else if (TIRO_AUTOMATICO && IsKeyDown(KEY_SPACE) && j->recarga <= 0) {
        // extra: a tecla continua segurada, mas so atira quando o temporizador zera
        atirou = disparaTiro(j->tiros, MAX_TIROS, j->nave);
        j->recarga = RECARGA_TIRO;
    }
    if (atirou) j->tirosDisparados++;
}

// ---------- alvos ----------
float multiplicadorDificuldade(int pontos) {
    int nivel = pontos / PONTOS_POR_NIVEL;
    return 1.0f + AUMENTO_POR_NIVEL * nivel;
}

// sorteia tipo, altura e lado de entrada; o alvo comeca fora da tela
void reposicionaAlvo(Alvo *a) {
    bool pequeno = (GetRandomValue(1, 4) == 1);                     // 1 em cada 4 e pequeno
    if (pequeno) {
        a->raio = 12;
        a->valor = 3;
        a->vel = (float)GetRandomValue(220, 300);                   // pequeno e rapido: vale 3
    } else {
        a->raio = 22;
        a->valor = 1;
        a->vel = (float)GetRandomValue(110, 180);                   // grande e lento: vale 1
    }

    bool vemDaEsquerda = (GetRandomValue(0, 1) == 0);
    a->pos.y = (float)GetRandomValue(Y_MIN_ALVO, Y_MAX_ALVO);
    if (vemDaEsquerda) {
        a->pos.x = -a->raio;
    } else {
        a->pos.x = LARGURA + a->raio;
        a->vel = -a->vel;
    }
}

void atualizaAlvo(Alvo *a, float dt, float multiplicador) {
    a->pos.x += a->vel * multiplicador * dt;
    bool saiuPelaDireita  = (a->vel > 0 && a->pos.x - a->raio > LARGURA);
    bool saiuPelaEsquerda = (a->vel < 0 && a->pos.x + a->raio < 0);
    if (saiuPelaDireita || saiuPelaEsquerda) reposicionaAlvo(a);
}

// cada tiro ativo que encosta num alvo some, soma os pontos e o alvo reaparece
void verificaAcertos(Jogo *j) {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!j->tiros[i].ativo) continue;
        for (int k = 0; k < MAX_ALVOS; k++) {
            Alvo *a = &j->alvos[k];
            if (CheckCollisionCircleRec(a->pos, a->raio, retanguloDoTiro(j->tiros[i]))) {
                j->tiros[i].ativo = false;
                j->pontos += a->valor;
                j->acertos++;
                reposicionaAlvo(a);
                break;                                              // este tiro ja acabou
            }
        }
    }
}

// ---------- folds: agregam um vetor num unico numero ----------
int contaTirosAtivos(const Projetil tiros[], int n) {
    int total = 0;
    for (int i = 0; i < n; i++)
        if (tiros[i].ativo) total++;
    return total;
}

int precisao(int acertos, int disparados) {                         // em %
    if (disparados == 0) return 0;
    return acertos * 100 / disparados;
}

// ---------- jogo ----------
void reiniciaJogo(Jogo *j) {
    j->nave = (Nave){ { LARGURA / 2.0f, ALTURA - 15.0f }, VEL_NAVE };
    for (int i = 0; i < MAX_TIROS; i++) j->tiros[i].ativo = false;
    for (int k = 0; k < MAX_ALVOS; k++) {
        reposicionaAlvo(&j->alvos[k]);
        // espalha os alvos pela largura para a partida nao comecar com a tela vazia
        j->alvos[k].pos.x = (float)GetRandomValue(60, LARGURA - 60);
    }
    j->pontos = 0;
    j->tirosDisparados = 0;
    j->acertos = 0;
    j->tempo = TEMPO_PARTIDA;
    j->recarga = 0;
}

void atualizaJogo(Jogo *j, float dt) {
    switch (j->estado) {
        case MENU:
            if (IsKeyPressed(KEY_ENTER)) { reiniciaJogo(j); j->estado = JOGANDO; }
            break;

        case JOGANDO: {
            atualizaNave(&j->nave, dt);
            trataDisparo(j, dt);

            for (int i = 0; i < MAX_TIROS; i++)                     // map: mesma funcao em todos
                atualizaTiro(&j->tiros[i], dt);

            float mult = multiplicadorDificuldade(j->pontos);
            for (int k = 0; k < MAX_ALVOS; k++)
                atualizaAlvo(&j->alvos[k], dt, mult);

            verificaAcertos(j);

            j->tempo -= dt;
            if (j->tempo <= 0) {
                j->tempo = 0;
                if (j->pontos > j->recorde) j->recorde = j->pontos;
                j->estado = FIM;
            }
        } break;

        case FIM:
            if (IsKeyPressed(KEY_R)) { reiniciaJogo(j); j->estado = JOGANDO; }
            if (IsKeyPressed(KEY_M)) j->estado = MENU;
            break;
    }
}

// =====================================================================
// DESENHAR: funcoes que so leem o estado
// =====================================================================

void textoCentralizado(const char *texto, int y, int tamanho, Color cor) {
    DrawText(texto, LARGURA / 2 - MeasureText(texto, tamanho) / 2, y, tamanho, cor);
}

void desenhaEstrelas(const Vector2 estrelas[], int n) {
    for (int i = 0; i < n; i++) DrawPixelV(estrelas[i], RAYWHITE);
}

void desenhaNave(Nave n) {
    // vertices em sentido anti-horario: ponta, base esquerda, base direita
    DrawTriangle((Vector2){ n.pos.x, n.pos.y - ALTURA_NAVE },
                 (Vector2){ n.pos.x - LARGURA_NAVE / 2, n.pos.y },
                 (Vector2){ n.pos.x + LARGURA_NAVE / 2, n.pos.y }, SKYBLUE);
}

void desenhaTiros(const Projetil tiros[], int n) {
    for (int i = 0; i < n; i++)
        if (tiros[i].ativo) DrawRectangleRec(retanguloDoTiro(tiros[i]), YELLOW);
}

void desenhaAlvos(const Alvo alvos[], int n) {
    for (int k = 0; k < n; k++) {
        Color cor = (alvos[k].valor > 1) ? ORANGE : RED;
        DrawCircleV(alvos[k].pos, alvos[k].raio, cor);
        DrawCircleV(alvos[k].pos, alvos[k].raio * 0.45f, RAYWHITE);
    }
}

void desenhaHUD(const Jogo *j) {
    DrawRectangle(0, 0, LARGURA, ALTURA_HUD, (Color){ 20, 20, 45, 255 });
    DrawText(TextFormat("Pontos: %d", j->pontos), 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("Tempo: %d", (int)ceilf(j->tempo)), 170, 10, 20, RAYWHITE);
    DrawText(TextFormat("Nivel: %d", j->pontos / PONTOS_POR_NIVEL + 1), 310, 10, 20, RAYWHITE);
    DrawText(TextFormat("Tiros no ar: %d/%d", contaTirosAtivos(j->tiros, MAX_TIROS), MAX_TIROS), 440, 10, 20, RAYWHITE);
    DrawText(TextFormat("Recorde: %d", j->recorde), 660, 10, 20, GOLD);
}

void desenhaJogo(const Jogo *j, const Vector2 estrelas[], int nEstrelas) {
    desenhaEstrelas(estrelas, nEstrelas);                           // fundo primeiro

    if (j->estado == MENU) {
        textoCentralizado("NAVINHA QUE ATIRA", 110, 44, SKYBLUE);
        textoCentralizado("A/D ou setas: mover  |  Espaco: atirar", 190, 20, RAYWHITE);
        textoCentralizado("Alvo vermelho: 1 ponto  |  Alvo laranja: 3 pontos", 220, 20, RAYWHITE);
        textoCentralizado(TextFormat("Recorde: %d", j->recorde), 265, 20, GOLD);
        textoCentralizado("ENTER para comecar", 310, 24, LIGHTGRAY);
        textoCentralizado("Equipe: Breno Souto, Bruno Rodrigues, Joao Fernando e Vinicius Beleza", 420, 16, GRAY);
    } else if (j->estado == JOGANDO) {
        desenhaAlvos(j->alvos, MAX_ALVOS);
        desenhaTiros(j->tiros, MAX_TIROS);
        desenhaNave(j->nave);
        desenhaHUD(j);                                              // interface por ultimo
    } else {
        textoCentralizado("FIM!", 100, 50, SKYBLUE);
        textoCentralizado(TextFormat("Pontos: %d", j->pontos), 175, 30, RAYWHITE);
        textoCentralizado(TextFormat("Recorde: %d", j->recorde), 215, 24, GOLD);
        textoCentralizado(TextFormat("Precisao: %d%% (%d acertos em %d tiros)",
                          precisao(j->acertos, j->tirosDisparados), j->acertos, j->tirosDisparados),
                          255, 20, LIGHTGRAY);
        textoCentralizado("R: jogar de novo  |  M: menu", 320, 20, GRAY);
    }
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Desafio 6 - Navinha que atira");
    SetTargetFPS(60);

    Vector2 estrelas[MAX_ESTRELAS];                                 // cenario fixo, sorteado uma vez
    for (int i = 0; i < MAX_ESTRELAS; i++)
        estrelas[i] = (Vector2){ (float)GetRandomValue(0, LARGURA), (float)GetRandomValue(ALTURA_HUD, ALTURA) };

    Jogo jogo = { 0 };
    jogo.estado = MENU;
    reiniciaJogo(&jogo);

    while (!WindowShouldClose()) {
        // 1. ENTRADA + 2. ATUALIZAR
        atualizaJogo(&jogo, GetFrameTime());

        // 3. DESENHAR
        BeginDrawing();
            ClearBackground((Color){ 10, 10, 30, 255 });
            desenhaJogo(&jogo, estrelas, MAX_ESTRELAS);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
