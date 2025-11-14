# 🎮 Projeto de Jogo em C

Bem-vindo ao repositório do nosso projeto desenvolvido em **C**, como parte da disciplina **Programação Imperativa e Funcional - 2025.2** da **CESAR School**.  
Este projeto tem como objetivo aplicar os conceitos fundamentais da linguagem C, explorando estruturas de controle, funções, modularização e interação via terminal.

---

## 🧠 Sobre o Projeto

Este jogo foi desenvolvido com base em conceitos de programação imperativa e funcional, utilizando a biblioteca CLI-lib para interface com o terminal. Nosso objetivo foi criar um jogo leve, acessível e engajador, que integrasse jogabilidade retrô com o conteúdo de lógica proposicional, garantindo clareza e aplicabilidade prática.

---

## 🧱 O Jogo: Pong Lógico

# 🎯 Descrição Geral
O Pong Lógico é uma versão educativa do clássico Pong, combinando ação simples com raciocínio lógico. O jogador controla uma raquete e deve rebater a bola, mas ela só será rebatida se a proposição lógica exibida no topo da tela for verdadeira, com base nos valores atuais de P, Q e R. A jogabilidade une reflexo e lógica, tornando o aprendizado de proposições algo natural, visual e divertido.

---

# ⚙️ Mecânica do Jogo

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
- Fórmula lógica atual (ϕ)

- Valores de P, Q e R

- Indicação visual:

✔️ VERDADEIRO (verde)

❌ FALSO (vermelho)

- Pontuação
 
- Bola e raquete, renderizadas no terminal via CLI-lib

## 🏆 Sistema de Pontuação e Ranking

O jogo possui um sistema simples e competitivo de pontuação:

   ```bash
  ✔️ Quando ganha ponto:
A cada rebatida correta, ou seja, quando:

A bola encosta na raquete e

A proposição lógica atual (ϕ) está VERDADEIRA
→ O jogador ganha +1 ponto automaticamente.

   ```

   ```bash
   ❌ Quando perde tudo:
Se a proposição estiver FALSA no momento da colisão,

A bola atravessa a raquete

O jogador perde seu combo/pontuação atual (pontuação volta para zero)
   ```

   ```bash
   🏅 Ranking das 5 maiores pontuações
Sempre que o jogador perde a pontuação, o valor final é registrado no ranking.

O ranking guarda as 5 maiores pontuações já atingidas.

O ranking é mostrado em uma tela própria, permitindo que o jogador tente bater seus próprios recordes.
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

## 🧰 Como Executar o Projeto

1. **Clone este repositório:**

   ```bash
   git clone https://github.com/devpedrois/PROJETO-PIF.git

   ```

2. **Baixe a biblioteca CLI-lib:**
   ```bash
   git clone https://github.com/tgfb/cli-lib.git
   ```
