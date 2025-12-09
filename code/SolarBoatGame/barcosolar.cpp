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
    classificacaoFinal = 0; // 0 significa que ainda não terminou
}

void BarcoSolar::limparDebug() {
    debugForcaMotor = Ponto2D(0,0);
    debugForcaArrasto = Ponto2D(0,0);
    debugAnguloLeme = 0.0f;
}

void BarcoSolar::setIntensidadeSolar(float valor) {
    if (valor < 0.0f) valor = 0.0f;
    if (valor > 1.0f) valor = 1.0f;
    intensidadeSolar = valor;
}

void BarcoSolar::setControle(bool habilitado) {
    controleHabilitado = habilitado;
}

void BarcoSolar::aplicarForca(Ponto2D forca) {
    Ponto2D a = forca / massa;
    aceleracao.add(a);
}

void BarcoSolar::aplicarHidrodinamica() {
    debugForcaArrasto = Ponto2D(0,0);
    float speed = velocidade.mag();
    if (speed < 0.01f) return;

    float rad = angulo * (PI / 180.0f);
    Ponto2D heading(std::cos(rad), std::sin(rad));
    heading.normalize();

    float velFrontalMag = (velocidade.x * heading.x) + (velocidade.y * heading.y);
    Ponto2D velFrontal = heading * velFrontalMag;
    Ponto2D velLateral = velocidade - velFrontal;

    Ponto2D arrastoFrente = velFrontal;
    arrastoFrente.normalize();
    arrastoFrente.mult(-1);
    arrastoFrente.mult(COEF_ARRASTO_CASCO * speed * speed);

    aplicarForca(arrastoFrente);
    debugForcaArrasto.add(arrastoFrente);

    float coefQuilha = COEF_ARRASTO_CASCO * 200.0f;
    Ponto2D vecArrastoLado = velLateral * -coefQuilha * speed;

    aplicarForca(vecArrastoLado);
    debugForcaArrasto.add(vecArrastoLado);
}

void BarcoSolar::verificarLimitesMapa() {
    float amortecimento = -0.3f;
    if (posicao.x < 0) { posicao.x = 0; velocidade.x *= amortecimento; }
    if (posicao.x > MAPA_LARGURA) { posicao.x = MAPA_LARGURA; velocidade.x *= amortecimento; }
    if (posicao.y < 0) { posicao.y = 0; velocidade.y *= amortecimento; }
    if (posicao.y > MAPA_ALTURA) { posicao.y = MAPA_ALTURA; velocidade.y *= amortecimento; }
}

void BarcoSolar::chocar(Ponto2D posObstaculo) {
    velocidade.mult(-0.2f);
    Ponto2D direcao = posicao - posObstaculo;
    direcao.normalize();
    posicao.add(direcao * 20.0f);
}

void BarcoSolar::colidirComBarco(BarcoSolar* outroBarco) {
    Ponto2D direcaoFuga = posicao - outroBarco->getPosicao();
    direcaoFuga.normalize();
    velocidade.add(direcaoFuga * 50.0f);
    posicao.add(direcaoFuga * 5.0f);
    velocidade.mult(0.9f);
}

void BarcoSolar::atualizar(float deltaTime) {
    aplicarHidrodinamica();

    if (terminado) {
        velocidade.mult(0.98f);
    }

    if (velocidadeAngular != 0) {
        aceleracaoAngular += -8.0f * velocidadeAngular;
    }

    velocidadeAngular += aceleracaoAngular * deltaTime;
    angulo += velocidadeAngular * deltaTime;
    aceleracaoAngular = 0.0f;

    debugForcaResultante = aceleracao * massa;

    Ponto2D mudancaVelocidade = aceleracao * deltaTime;
    velocidade.add(mudancaVelocidade);

    Ponto2D deslocamento = velocidade * deltaTime;
    posicao.add(deslocamento);

    verificarLimitesMapa();
    aceleracao.mult(0);
    carregarBateria();
}

void BarcoSolar::acelerar() {
    if (controleHabilitado && nivelBateria > taxaConsumo) {
        float rad = angulo * (PI / 180.0f);
        Ponto2D forcaMotor(std::cos(rad), std::sin(rad));
        float forcaPixels = FORCA_MOTOR * (ESCALA / 10.0f);
        forcaMotor.mult(forcaPixels);
        aplicarForca(forcaMotor);
        debugForcaMotor = forcaMotor;
        nivelBateria -= taxaConsumo;
    }
}

void BarcoSolar::virar(float val) {
    if (controleHabilitado) {
        float speed = velocidade.mag();
        float eficienciaLeme = std::clamp(speed / 150.0f, 0.1f, 1.0f);
        float torque = val * 250.0f * eficienciaLeme;
        aceleracaoAngular += torque;
        debugAnguloLeme = val;
    }
}

void BarcoSolar::carregarBateria() {
    if (nivelBateria < capacidadeBateria) {
        nivelBateria += taxaRecarga * intensidadeSolar;
    }
}

void BarcoSolar::buscarAlvo(Ponto2D alvo) {
    if (!controleHabilitado) return;

    Ponto2D desejado = alvo - posicao;
    float anguloDesejadoRad = std::atan2(desejado.y, desejado.x);
    float anguloDesejadoGraus = anguloDesejadoRad * (180.0f / PI);

    float erro = anguloDesejadoGraus - angulo;
    while (erro > 180) erro -= 360;
    while (erro < -180) erro += 360;

    float sensibilidade = 0.15f;
    float comandoLeme = erro * sensibilidade;
    if (comandoLeme > 3.0f) comandoLeme = 3.0f;
    if (comandoLeme < -3.0f) comandoLeme = -3.0f;

    virar(comandoLeme);

    if (std::abs(erro) < 110.0f || velocidade.mag() < 50.0f) {
        acelerar();
    }
}

void BarcoSolar::incrementarCheckpoint(int totalCheckpoints) {
    if (proximoCheckpointId == 0) voltaAtual++;
    proximoCheckpointId++;
    if (proximoCheckpointId >= totalCheckpoints) proximoCheckpointId = 0;
}

void BarcoSolar::setTerminado(bool t) {
    terminado = t;
    if(t) controleHabilitado = false;
}

bool BarcoSolar::isTerminado() const { return terminado; }

void BarcoSolar::setClassificacaoFinal(int posicao) { classificacaoFinal = posicao; }
int BarcoSolar::getClassificacaoFinal() const { return classificacaoFinal; }

QString BarcoSolar::getNome() const { return nome; }

int BarcoSolar::getScoreCorrida() const {
    if (terminado) return 1000000 + (1000 - 0);
    return (voltaAtual * 1000) + proximoCheckpointId;
}

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
