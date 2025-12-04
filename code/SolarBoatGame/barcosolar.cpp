#include "barcosolar.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

// Constantes físicas e dimensões do mundo
const float PI = 3.14159265f;
const float MAPA_LARGURA = 5000.0f;
const float MAPA_ALTURA = 5000.0f;

// --- FÍSICA ---
const float ESCALA = 30.0f;
const float MASSA = 500.0f;
const float FORCA_MOTOR = 5000.0f;
const float COEF_ARRASTO_CASCO = 0.025f;

BarcoSolar::BarcoSolar(float x, float y, QString nome) {
    // Inicializa propriedades físicas e estado inicial
    this->nome = nome;
    massa = MASSA;
    capacidadeBateria = 1000.0f;
    taxaConsumo = 0.2f;
    taxaRecarga = 0.1f;
    raioColisao = 8.0f;
    limparDebug();
    resetar(x, y);
}

void BarcoSolar::resetar(float x, float y) {
    // Restaura variáveis cinemáticas e de jogo para o padrão
    posicao = Ponto2D(x, y);
    velocidade = Ponto2D(0, 0);
    aceleracao = Ponto2D(0, 0);
    angulo = 0.0f;
    velocidadeAngular = 0.0f;
    aceleracaoAngular = 0.0f;
    nivelBateria = 1000.0f;
    voltaAtual = 0;
    proximoCheckpointId = 0;
    intensidadeSolar = 1.0f;
    terminado = false;
    controleHabilitado = true;
}

void BarcoSolar::limparDebug() {
    // Zera vetores de visualização no início do frame
    debugForcaMotor = Ponto2D(0,0);
    debugForcaArrasto = Ponto2D(0,0);
    debugAnguloLeme = 0.0f;
}

void BarcoSolar::setIntensidadeSolar(float valor) {
    // Aplica clamp entre 0.0 e 1.0 para eficiência do painel
    if (valor < 0.0f) valor = 0.0f;
    if (valor > 1.0f) valor = 1.0f;
    intensidadeSolar = valor;
}

void BarcoSolar::setControle(bool habilitado) {
    controleHabilitado = habilitado;
}

void BarcoSolar::aplicarForca(Ponto2D forca) {
    // Segunda lei de Newton (F = m*a)
    Ponto2D a = forca / massa;
    aceleracao.add(a);
}

void BarcoSolar::aplicarHidrodinamica() {
    // Reinicia vetor de arrasto para cálculo deste frame
    debugForcaArrasto = Ponto2D(0,0);

    float speed = velocidade.mag();
    if (speed < 0.01f) return;

    // Calcula vetor de direção (heading) baseado no ângulo
    float rad = angulo * (PI / 180.0f);
    Ponto2D heading(std::cos(rad), std::sin(rad));
    heading.normalize();

    // Decompõe a velocidade em componentes frontal e lateral
    float velFrontalMag = (velocidade.x * heading.x) + (velocidade.y * heading.y);
    Ponto2D velFrontal = heading * velFrontalMag;
    Ponto2D velLateral = velocidade - velFrontal;

    // Aplica arrasto quadrático frontal (resistência da água)
    Ponto2D arrastoFrente = velFrontal;
    arrastoFrente.normalize();
    arrastoFrente.mult(-1);
    arrastoFrente.mult(COEF_ARRASTO_CASCO * speed * speed);

    aplicarForca(arrastoFrente);
    debugForcaArrasto.add(arrastoFrente);

    // Aplica resistência lateral alta para simular a quilha (evita drift excessivo)
    float coefQuilha = COEF_ARRASTO_CASCO * 200.0f;
    Ponto2D vecArrastoLado = velLateral * -coefQuilha * speed;

    aplicarForca(vecArrastoLado);
    debugForcaArrasto.add(vecArrastoLado);
}

void BarcoSolar::verificarLimitesMapa() {
    // Colisão simples com as bordas do mundo (inverte velocidade com amortecimento)
    float amortecimento = -0.3f;
    if (posicao.x < 0) { posicao.x = 0; velocidade.x *= amortecimento; }
    if (posicao.x > MAPA_LARGURA) { posicao.x = MAPA_LARGURA; velocidade.x *= amortecimento; }
    if (posicao.y < 0) { posicao.y = 0; velocidade.y *= amortecimento; }
    if (posicao.y > MAPA_ALTURA) { posicao.y = MAPA_ALTURA; velocidade.y *= amortecimento; }
}

void BarcoSolar::chocar(Ponto2D posObstaculo) {
    // Resposta de colisão estática: inverte velocidade e empurra para fora
    velocidade.mult(-0.2f);
    Ponto2D direcao = posicao - posObstaculo;
    direcao.normalize();
    posicao.add(direcao * 20.0f);
}

void BarcoSolar::colidirComBarco(BarcoSolar* outroBarco) {
    // Resposta de colisão dinâmica: afasta os barcos e transfere momento
    Ponto2D direcaoFuga = posicao - outroBarco->getPosicao();
    direcaoFuga.normalize();
    velocidade.add(direcaoFuga * 50.0f);
    posicao.add(direcaoFuga * 5.0f);
    velocidade.mult(0.9f);
}

void BarcoSolar::atualizar(float deltaTime) {
    // Calcula forças de resistência
    aplicarHidrodinamica();

    // Aplica freio automático se a corrida acabou
    if (terminado) {
        velocidade.mult(0.98f);
    }

    // Aplica amortecimento angular (resistência à rotação)
    if (velocidadeAngular != 0) {
        aceleracaoAngular += -8.0f * velocidadeAngular;
    }

    // Integração de Euler para rotação
    velocidadeAngular += aceleracaoAngular * deltaTime;
    angulo += velocidadeAngular * deltaTime;
    aceleracaoAngular = 0.0f;

    debugForcaResultante = aceleracao * massa;

    // Integração de Euler para translação (Posição e Velocidade)
    Ponto2D mudancaVelocidade = aceleracao * deltaTime;
    velocidade.add(mudancaVelocidade);

    Ponto2D deslocamento = velocidade * deltaTime;
    posicao.add(deslocamento);

    verificarLimitesMapa();

    // Reseta aceleração para o próximo frame e processa energia
    aceleracao.mult(0);
    carregarBateria();
}

void BarcoSolar::acelerar() {
    if (controleHabilitado && nivelBateria > taxaConsumo) {
        // Calcula vetor de força baseado na orientação atual
        float rad = angulo * (PI / 180.0f);
        Ponto2D forcaMotor(std::cos(rad), std::sin(rad));

        float forcaPixels = FORCA_MOTOR * (ESCALA / 10.0f);
        forcaMotor.mult(forcaPixels);

        aplicarForca(forcaMotor);
        debugForcaMotor = forcaMotor;

        // Consome energia
        nivelBateria -= taxaConsumo;
    }
}

void BarcoSolar::virar(float val) {
    if (controleHabilitado) {
        // Aplica torque. O leme é menos eficiente em baixas velocidades.
        float speed = velocidade.mag();
        float eficienciaLeme = std::clamp(speed / 150.0f, 0.1f, 1.0f);
        float torque = val * 250.0f * eficienciaLeme;
        aceleracaoAngular += torque;
        debugAnguloLeme = val;
    }
}

void BarcoSolar::carregarBateria() {
    // Regeneração passiva baseada na intensidade solar
    if (nivelBateria < capacidadeBateria) {
        nivelBateria += taxaRecarga * intensidadeSolar;
    }
}

void BarcoSolar::buscarAlvo(Ponto2D alvo) {
    if (!controleHabilitado) return;

    // Calcula ângulo necessário para olhar para o alvo
    Ponto2D desejado = alvo - posicao;
    float anguloDesejadoRad = std::atan2(desejado.y, desejado.x);
    float anguloDesejadoGraus = anguloDesejadoRad * (180.0f / PI);

    // Normaliza o erro angular entre -180 e 180
    float erro = anguloDesejadoGraus - angulo;
    while (erro > 180) erro -= 360;
    while (erro < -180) erro += 360;

    // Controle simples "Bang-Bang" para virar
    if (erro > 10.0f) virar(3.0f);
    else if (erro < -10.0f) virar(-3.0f);

    // Acelera se estiver alinhado ou muito lento
    if (std::abs(erro) < 90.0f || velocidade.mag() < 50.0f) {
        acelerar();
    }
}

void BarcoSolar::incrementarCheckpoint(int totalCheckpoints) {
    // Gerencia contagem de voltas e progresso de checkpoints
    if (proximoCheckpointId == 0) voltaAtual++;
    proximoCheckpointId++;
    if (proximoCheckpointId >= totalCheckpoints) proximoCheckpointId = 0;
}

void BarcoSolar::setTerminado(bool t) {
    terminado = t;
    if(t) controleHabilitado = false;
}
bool BarcoSolar::isTerminado() const { return terminado; }
QString BarcoSolar::getNome() const { return nome; }

int BarcoSolar::getScoreCorrida() const {
    // Pontuação: Alta prioridade para quem terminou, seguida por voltas e checkpoints
    if (terminado) return 1000000 + (1000 - 0);
    return (voltaAtual * 1000) + proximoCheckpointId;
}

// Getters padrão
Ponto2D BarcoSolar::getPosicao() const { return posicao; }
float BarcoSolar::getAngulo() const { return angulo; }
float BarcoSolar::getBateria() const { return nivelBateria; }
float BarcoSolar::getVelocidadeAtual() const { return velocidade.mag(); }
int BarcoSolar::getProximoCheckpointId() const { return proximoCheckpointId; }
int BarcoSolar::getVoltaAtual() const { return voltaAtual; }
float BarcoSolar::getRaioColisao() const { return raioColisao; }

Ponto2D BarcoSolar::getDebugForcaMotor() const { return debugForcaMotor; }
Ponto2D BarcoSolar::getDebugForcaArrasto() const { return debugForcaArrasto; }
Ponto2D BarcoSolar::getDebugForcaResultante() const { return debugForcaResultante; }
float BarcoSolar::getDebugAnguloLeme() const { return debugAnguloLeme; }
