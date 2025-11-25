# 🎮 LogicPong

Bem-vindo ao repositório do LogicPong, jogo desenvolvido em C como parte da disciplina **Programação Imperativa e Funcional - 2025.2** da **CESAR School**.  
Este projeto tem como objetivo aplicar os conceitos fundamentais da linguagem C, explorando estruturas de controle, funções, modularização e interação via terminal.

---

## 🧠 Sobre o Projeto

O LogicPong foi pensado para unir:

- Jogabilidade retrô (estilo Pong, no terminal);

- Lógica proposicional, com fórmulas envolvendo P, Q e R;

- Prática de programação em C, usando a biblioteca CLI-lib para desenhar no terminal.

O foco é ser um jogo leve, acessível e didático: enquanto você tenta rebater a bolinha, precisa raciocinar sobre o valor lógico das proposições.

---

## 🧱 O Jogo: Pong Lógico

# 🎯 Descrição Geral
O LogicPong é uma versão educativa do clássico Pong, combinando ação simples com raciocínio lógico. O jogador controla uma raquete e deve rebater a bola, mas ela só será rebatida se a proposição lógica exibida no topo da tela for verdadeira, com base nos valores atuais de P, Q e R. A jogabilidade une reflexo e lógica, tornando o aprendizado de proposições algo natural, visual e divertido.

---

# ⚙️ Regras / Mecânica

O jogo percorre um conjunto de 10 fórmulas lógicas envolvendo P, Q e R.

A ordem das fórmulas é embaralhada de tempos em tempos, para evitar que o jogador decore sempre a mesma sequência.

Cada vez que você rebate a bola com a fórmula VERDADEIRA, você ganha +1 ponto e a bola fica um pouco mais rápida.

Se você errar (fórmula falsa ou deixar a bola passar):
- Seu combo atual volta para zero.
- A bola é reposicionada no campo.
- Seu melhor combo daquela partida é salvo com o seu nome no ranking global (top 5), se estiver entre as maiores pontuações.

# 🎮 Objetivo
Manter a bola em jogo o maior tempo possível, garantindo que a fórmula lógica (ϕ) seja verdadeira sempre que a bola encostar na raquete e acumular pontos. Se a proposição for falsa, a bola atravessa a raquete e o jogador perde ponto ou vida.

# 🕹️ Como Jogar
   ```bash
   Mover a raquete:
↑ (cima) e ↓ (baixo)

Alterar valores das variáveis:

1 → alterna o valor de P (V e F)

2 → alterna o valor de Q (V e F)

3 → alterna o valor de R (V e F)

Interação da bola com a lógica:

Se ϕ é VERDADEIRA → a bola rebate normalmente.

Se ϕ é FALSA → a bola passa direto, penalizando o jogador e reiniciando seus pontos. Cada rebatida dá UM ponto.

   ```

# 🧮 Exemplo Rápido

   ```bash
   Expressão: P → Q
Valores atuais: P = V // Q = F
Resultado: FALSO → a bola vai atravessar!
Obs: Se o jogador mudar Q para 1, o resultado se torna VERDADEIRO, e a bola volta a rebater normalmente.

   ```

## 📊 Elementos da Tela (HUD)

- Nome do jogador
  
- Número e texto da fórmula atual (ex: Formula 3/10: ~(P V R) /\ Q)

- Valores de P, Q e R

- Indicação visual:

✔️ VERDADEIRO (verde)

❌ FALSO (vermelho)

- Score atual (combo da rodada)

- Melhor score dessa partida
 
- Bola e raquete, renderizadas no terminal via CLI-lib

## 🏆 Sistema de Pontuação e Ranking

O jogo possui um sistema simples e competitivo de pontuação:

   ```bash
  ✔️ Quando ganha ponto:
+1 ponto a cada rebatida com a expressão VERDADEIRA. Ou seja, quando:

A bola encosta na raquete e

A proposição lógica atual (ϕ) está VERDADEIRA
→ O jogador ganha +1 ponto automaticamente.

   ```

   ```bash
   ❌ Se a expressão for FALSA na hora da colisão (ou a bola passar direto):
O combo atual volta para 0.

A melhor pontuação dessa sessão é atualizada, se for maior.

O jogador perde seu combo/pontuação atual (pontuação volta para zero)
   ```

   ```bash
   🏅 Ranking das 5 maiores pontuações
Ao sair da partida com [Q], o melhor score do jogador é:
Comparado com o que já existe no ranking para aquele nome.
Atualizado se for maior.

Sempre que o jogador perde a pontuação, o valor final é registrado no ranking.

O ranking guarda as 5 maiores pontuações já atingidas.

No menu principal, existe a opção “Ver ranking”, o ranking é mostrado em uma tela própria, permitindo que o jogador tente bater seus próprios recordes.
   ```

Esse sistema cria um loop divertido: jogar, pontuar, errar, tentar bater o recorde anterior.

## 🧩 Tecnologias e Ferramentas

- **Linguagem:** C
- **Biblioteca CLI:** [CLI-lib](https://github.com/tgfb/cli-lib.git)
- **Editor sugerido:** VS Code
- **Sistema operacional:** compatível com Windows, Linux e macOS

---

## 👥 Equipe de Desenvolvimento

| Nome Completo   | Usuário no GitHub                                  |
| --------------- | -------------------------------------------------- |
| Pedro Iranildo  | [@Devpedrois](https://github.com/devpedrois)       |
| Gabriel Barbosa | [@GazeT3](https://github.com/GazeT3)               |
| Diogo Silas     | [@ildevdio](https://github.com/ildevdio)           |
| Rafael Aimbere  | [@codeblack2301](https://github.com/codeblack2301) |
| Gabriela Bayo   | [@gabibayo](https://github.com/gabibayo)           |
| João Vinicius   | [@sucogelad0](https://github.com/sucogelad0)       |

---

## 🏫 Informações Acadêmicas

- **Disciplina:** Programação Imperativa e Funcional - 2025.2
- **Instituição:** CESAR School

---

## 🧰 Como compilar e executar

1. **Clone este repositório:**

   ```bash
   git clone https://github.com/devpedrois/PROJETO-PIF.git

   ```

2. **Abra o terminal:**
 ---------------------------------
   Windows (PowerShell / CMD, com MinGW instalado)
   Certifique-se de que o gcc está instalado (via MinGW ou similar) e acessível no Prompt de Comando ou PowerShell.
   ```bash
   cd C:\Users\SeuUsuario\Desktop\PIF\projeto
   ```
   No diretório raiz do projeto, execute(para compilar):
   ```bash
   gcc src\main.c src\game.c src\logic.c src\ranking.c src\screen.c src\keyboard.c src\timer.c -Iinclude -Wall -Werror -g -o build\pong-logico.exe
   ```
   Para executar o jogo:
   ```bash
   .\build\pong-logico.exe
   ```


   ---------------------------------

   Linux / macOS (sem make):
   ```bash
   cd /caminho/para/o/projeto
   ```
   ```bash
    gcc src/*.c -Iinclude -Wall -Werror -g -o build/pong-logico
   ```
   ```bash
    ./build/pong-logico
   ```
  ---------------------------------

  Linux / macOS (com make):
   ```bash
  cd /caminho/para/o/projeto
   ```
   ```bash
     make
   ```
   ```bash
     make run
   ```
    
