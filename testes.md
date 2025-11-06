# Testes

>[!NOTE]
> Relatar o processo de testes do sistema, especificando quais testes foram realizados e os resultados obtidos.

Foram planejados dois tipos principais de testes para garantir a qualidade e o funcionamento correto do jogo:

### Testes de Unidade

Testes focados em verificar o funcionamento de classes e métodos de forma isolada.
* **Classe BarcoSolar**:
    * **Teste de Aceleração**: Verificar se o método `acelerar()` aumenta a velocidade e diminui a bateria na proporção correta.
    * **Teste de Resistência**: Verificar se a `velocidade` diminui passivamente com o atrito da água (implementado no método `atualizar()`).
    * **Teste de Carregamento**: Verificar se o método `gerarEnergia()` aumenta o nível da bateria corretamente.
    * **Teste de Limites**: Verificar se a bateria não ultrapassa 100% e não fica abaixo de 0%.
    * **Teste de Lógica de Corrida**: Verificar se os atributos `voltaAtual` e `proximoCheckpointId` são atualizados corretamente.
* **Classe Pista**:
    * **Teste de Carregamento**: Verificar se a lista de `Checkpoints` é carregada corretamente na `Pista`.

### Testes de Jogo (Playtesting)

Testes realizados por um jogador para avaliar a experiência de jogo, a jogabilidade e a dificuldade.
* **Teste de Jogabilidade**: Avaliar se os controles do barco (isométricos) são responsivos e intuitivos.
* **Teste de Balanceamento**: Verificar se o consumo e a geração de energia estão bem calibrados. A corrida é desafiadora, mas não impossível?
* **Teste de Colisão**: Navegar pela pista e verificar se a detecção de colisão com os limites da pista está funcionando.
* **Teste de Checkpoint (Integração)**: Verificar se ao cruzar fisicamente um checkpoint no jogo, a lógica de corrida (voltas, próximo checkpoint) é acionada corretamente.
* **Teste de Condição de Vitória**: Completar uma corrida para garantir que a tela de vitória é acionada corretamente.

<div align="center">

[Retroceder](implementacao.md) | [Início](README.md)

</div>