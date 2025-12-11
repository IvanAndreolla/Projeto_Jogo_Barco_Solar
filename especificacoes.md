# Especificações das Pistas

O jogo apresenta três circuitos distintos, projetados para explorar a mecânica de movimentação isométrica e o sistema de inércia do barco solar.

## Pista 1: Enseada do Sol (Iniciante)

* **Conceito:** Uma pista de formato **Oval** clássico, larga e sem obstáculos internos.
* **Jogabilidade:** O foco é a adaptação aos controles. Como as curvas são abertas e constantes, o jogador pode manter a velocidade máxima na maior parte do tempo, aprendendo a lidar com a inércia do barco e o delay do leme sem punições severas.
* **Visual:** Água aberta com ilhas apenas nas bordas distantes para delimitar o cenário.

## Pista 2: Circuito da Ponte (Intermediário)

* **Conceito:** Um traçado em formato de **"8"** (Oito), criando um ponto de cruzamento central.
* **Destaque Visual:** A pista conta com uma renderização em alta escala da **Ponte Hercílio Luz**. Embora seja um elemento visual (sem colisão física), ela é renderizada acima dos barcos, criando a imersão de navegar por baixo do cartão postal de Florianópolis.
* **Jogabilidade:** O formato em 8 exige que o jogador alterne entre curvas para a esquerda e para a direita, testando a capacidade de resposta do leme. O cruzamento central adiciona um risco estratégico de colisão com oponentes retardatários ou líderes.

## Pista 3: Slalom Sinuoso (Avançado)

* **Conceito:** Uma pista técnica gerada matematicamente (curvas senoidais), apelidada de "Ameba". O traçado é estreito e altamente sinuoso.
* **Jogabilidade:** Exige controle preciso da potência do motor. O jogador não pode manter aceleração máxima constante, precisando desacelerar para fazer as curvas fechadas.
* **Sistema de Guias:** Devido à complexidade das curvas, esta pista utiliza um sistema híbrido de visualização:
    * **Boias:** Delimitam os portões principais.
    * **Marcadores de Guia:** Pequenos pontos amarelos na água indicam o traçado ideal nas seções onde não há boias, ajudando o jogador a antecipar a próxima curva "invisível".
* **Visual:** Ilhas estratégicas posicionadas nas "barrigas" das curvas (internas e externas) para forçar o traçado geográfico.

## Mecânicas Ambientais Implementadas

* **Clima Dinâmico (Nuvens):** O jogo possui um sistema de nuvens procedurais que se movem sobre a pista. Quando uma nuvem passa sobre o barco do jogador (sombra), a **geração de energia solar cai drasticamente**, afetando a recarga da bateria e exigindo estratégia.
* **Física Realista:** Os barcos possuem inércia de motor (não param instantaneamente) e atraso no leme (o ângulo de virada não é imediato), simulando o comportamento real de uma embarcação na água.

<div align="center">

[Retornar à análise](analise.md)

</div>