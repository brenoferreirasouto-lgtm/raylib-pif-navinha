# Desafio 6: Navinha que atira

**Programação Imperativa e Funcional (PIF)** · CESAR School · 2026.2 · Prof. Roni Maciel

**Integrantes**
- Breno Souto
- Bruno Rodrigues
- João Fernando
- Vinicius Beleza

**Arquivo do jogo:** `desafio6.c` · **Compilar:** `make desafio6`

---

## 1. O jogo em uma frase

Uma nave em forma de triângulo anda na horizontal, embaixo da tela. Cada toque em **Espaço** dispara **um** projétil para cima. Alvos atravessam a tela no alto; acertou, ganha ponto. A partida tem tempo limitado e termina numa tela de fim com pontuação e recorde.

## 2. O que o professor vai cobrar

Critérios obrigatórios do enunciado:

- [ ] Projéteis num vetor fixo (`Projetil tiros[20]`), reaproveitando os inativos
- [ ] Um tiro por toque: `IsKeyPressed`, nunca `IsKeyDown`
- [ ] Projétil que sai da tela vira inativo
- [ ] Um laço atualiza todos os projéteis com a mesma função (*map*)

Avaliação geral: cada critério funcionando, o jogo compila **sem erros** com `make desafio6`, e o código é legível (nomes claros, `#define` para constantes, atualizar separado de desenhar).

Além disso, o plano inclui o **extra** do enunciado (tiro contínuo a cada 0,3 s) e **ideias próprias** da equipe (seção 5).

## 3. Como o código vai ser organizado

Partimos do `molde.c`, que já traz as telas `MENU`, `JOGANDO` e `FIM`, `struct`s e a separação entre atualizar e desenhar.

**Constantes (`#define`)**: `LARGURA`, `ALTURA`, `MAX_TIROS` (20), `MAX_ALVOS`, `VEL_NAVE`, `VEL_TIRO`, `TEMPO_PARTIDA`, `RECARGA_TIRO` (0,3 s).

**Structs**

| Struct | Campos | Para quê |
| --- | --- | --- |
| `Nave` | `Vector2 pos` (centro da base), `float velocidade` | O jogador (largura e altura são `#define`) |
| `Projetil` | `Vector2 pos`, `float vel`, `bool ativo` | Cada tiro do vetor `tiros[MAX_TIROS]` |
| `Alvo` | `Vector2 pos`, `float raio`, `float vel`, `int valor` | Cada alvo do vetor `alvos[MAX_ALVOS]` (sempre em jogo: ao sair ou ser atingido, reaparece) |
| `Jogo` | estado, nave, vetores, pontos, recorde, tempo, recarga | Junta todo o estado para passar às funções |

**Funções**

| Tipo | Função | O que faz |
| --- | --- | --- |
| Atualizar | `atualizaNave(Nave *n, float dt)` | Move com A/D ou setas e prende na tela |
| Atualizar | `disparaTiro(Projetil tiros[], int n, Nave nave)` | Ativa o **primeiro tiro inativo** na ponta da nave; devolve `false` se os 20 estão no ar |
| Atualizar | `trataDisparo(Jogo *j, float dt)` | Toque com `IsKeyPressed` e o extra do tiro segurado com temporizador |
| Atualizar (*map*) | `atualizaTiro(Projetil *t, float dt)` | Sobe o tiro; se saiu da tela, `ativo = false` |
| Atualizar | `atualizaAlvo(Alvo *a, float dt, float multiplicador)` | Move o alvo (mais rápido a cada nível); se saiu da tela, reaparece |
| Atualizar | `reposicionaAlvo(Alvo *a)` | Sorteia tipo, altura e lado de entrada do alvo |
| Atualizar | `verificaAcertos(...)` | Colisão tiro × alvo, soma pontos, reposiciona o alvo |
| Atualizar | `reiniciaJogo(...)` | Zera pontos, tempo, tiros e alvos |
| *Fold* | `contaTirosAtivos(const Projetil tiros[], int n)` | Quantos tiros estão no ar |
| Cálculo | `precisao(int acertos, int disparados)` | Porcentagem de acerto mostrada no fim |
| Desenhar | `desenhaNave`, `desenhaTiros`, `desenhaAlvos`, `desenhaHUD` | Só leem o estado e desenham |

## 4. Passo a passo de produção

Cada passo termina com o jogo **compilando e rodando**. Só avançamos quando o teste do passo funcionar.

### Passo 0: Preparar o arquivo
1. Na pasta `desafio` do repositório, copiar o molde: `cp molde.c desafio6.c`.
2. Rodar `make desafio6` e abrir o jogo, para confirmar que o molde funciona antes de mexer.
3. Trocar o título da janela e o texto do menu para "NAVINHA QUE ATIRA".

**Teste:** abre o menu, ENTER vai para o jogo, a janela tem o título novo.

### Passo 1: Constantes e structs
1. Criar os `#define` da seção 3.
2. Trocar o `Jogador` do molde pela struct `Nave` e criar as structs `Projetil`, `Alvo` e `Jogo`.
3. Declarar os vetores `Projetil tiros[MAX_TIROS]` e `Alvo alvos[MAX_ALVOS]` e iniciar todos os tiros com `ativo = false`.

**Teste:** compila sem avisos (`-Wall`).

### Passo 2: A nave
1. Desenhar a nave como triângulo com `DrawTriangle`, na parte de baixo da tela. A ordem dos vértices é ponta, base esquerda, base direita, em sentido **anti-horário**; senão o triângulo some.
2. `atualizaNave`: mover na horizontal com **A/D** ou **setas**, sempre multiplicando por `dt`.
3. Prender a nave entre `0` e `LARGURA - largura`.

**Teste:** a nave anda e não sai da tela pelos lados.

### Passo 3: Disparo (um tiro por toque)
1. Com `IsKeyPressed(KEY_SPACE)`, chamar `disparaTiro`.
2. `disparaTiro` percorre o vetor, acha o **primeiro tiro inativo**, coloca-o na ponta da nave e faz `ativo = true`. Se os 20 estiverem ativos, não dispara.
3. Desenhar só os tiros ativos (retângulo fino ou círculo pequeno).

**Teste:** cada toque no espaço sai **exatamente um** tiro; segurar o espaço não dispara mais.

### Passo 4: Movimento dos tiros (*map*)
1. Escrever `atualizaTiro(Projetil *t, float dt)`: se o tiro está ativo, sobe `VEL_TIRO * dt`; se `pos.y < 0`, vira inativo.
2. No `JOGANDO`, **um único laço** chama `atualizaTiro(&tiros[i], dt)` para todos os tiros. Essa é a operação *map*: a mesma função aplicada a cada elemento do vetor.
3. Mostrar na tela "Tiros no ar: N" com o *fold* `contaTirosAtivos`.

**Teste:** os tiros sobem, somem no topo, e o contador volta a 0. Disparar sem parar nunca trava, porque os tiros inativos são reaproveitados.

### Passo 5: Alvos
1. Iniciar os alvos no alto da tela, com `y` e velocidade sorteados por `GetRandomValue`.
2. `atualizaAlvo`: move na horizontal; ao sair por um lado, volta pelo outro numa altura sorteada.
3. Desenhar com `DrawCircleV`.

**Teste:** os alvos atravessam a tela continuamente, sem nenhum tiro ainda.

### Passo 6: Acertos e pontuação
1. `verificaAcertos`: para cada tiro ativo e cada alvo, testar a colisão com `CheckCollisionCircleRec` (alvo = círculo, tiro = retângulo).
2. Acertou: o tiro fica inativo, `pontos += alvo.valor` e o alvo reaparece num lugar sorteado.
3. Mostrar o placar no topo, desenhado **por último**.

**Teste:** acertar um alvo soma ponto, o tiro some e o alvo reaparece. Um tiro não pode pontuar duas vezes.

### Passo 7: Telas, tempo e reinício
1. Cronômetro: `float tempo = TEMPO_PARTIDA` (por exemplo 60 s), subtraindo `dt` a cada quadro e mostrando na tela.
2. Tempo zerado → `estado = FIM`, com "Fim! Pontos: N".
3. Escrever `reiniciaJogo(...)` e chamar no início e ao apertar **R** na tela de fim.

**Teste:** a partida acaba sozinha, mostra os pontos, e **R** começa uma partida limpa sem fechar a janela.

### Passo 8: Extra do enunciado (tiro contínuo a cada 0,3 s)
1. Criar `float recarga = 0`, que diminui `dt` a cada quadro.
2. O **toque** continua com `IsKeyPressed` (critério obrigatório), e ele também faz `recarga = RECARGA_TIRO`.
3. Regra separada: se o espaço continua **segurado** e `recarga <= 0`, dispara de novo e faz `recarga = RECARGA_TIRO`.

**Teste:** segurando o espaço, sai no máximo 1 tiro a cada 0,3 s. Tocar rápido continua dando 1 tiro por toque.

### Passo 9: Ideias da equipe
Ver a seção 5. Uma ideia por vez, testando depois de cada uma.

### Passo 10: Revisão e entrega
1. Conferir os 4 critérios da seção 2, um por um, jogando.
2. Revisar a legibilidade: nomes claros, nenhum número "mágico" fora dos `#define`, nenhuma função de desenho mudando o estado.
3. `make desafio6` sem erros e sem avisos.
4. Commit e push do `desafio6.c` no repositório, antes do prazo do assignment.

## 5. Ideias a mais da equipe

Opcionais, feitas depois que os obrigatórios estiverem prontos:

| Ideia | Como fazer | Conceito praticado |
| --- | --- | --- |
| **Dificuldade crescente** | A cada 10 pontos, os alvos ficam 15% mais rápidos | Estado do jogo |
| **Alvos com valores diferentes** | Alvo pequeno e rápido vale 3 pontos; grande e lento vale 1 (campo `valor` da struct) | Vetor de `struct` |
| **Recorde** | Guardar o maior placar enquanto o jogo está aberto e mostrar no menu e no fim | Variável fora do `reiniciaJogo` |
| **Precisão** | No fim, mostrar acertos ÷ tiros disparados, em % | *Fold* / contadores |

## 6. Controles do jogo

| Tecla | Ação |
| --- | --- |
| **A / D** ou **← / →** | Mover a nave |
| **Espaço** | Atirar (segurando: 1 tiro a cada 0,3 s) |
| **ENTER** | Começar (menu) |
| **R** | Jogar de novo (tela de fim) |
| **M** | Voltar ao menu (tela de fim) |
| **ESC** | Fechar o jogo |
