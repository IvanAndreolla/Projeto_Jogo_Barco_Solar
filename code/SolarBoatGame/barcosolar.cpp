#include "barcosolar.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

const float PI = 3.14159265f;
const float MAPA_LARGURA = 5000.0f;
const float MAPA_ALTURA = 5000.0f;

const float ESCALA = 30.0f;
const float MASSA_KG = 300.0f;
const float FORCA_MOTOR_NEWTONS = 5000.0f;
const float COEF_ARRASTO_CASCO = 0.005f;

BarcoSolar::BarcoSolar(float x, float y) {
    massa = MASSA_KG;
    capacidadeBateria = 1000.0f;
    taxaConsumo = 0.3f;
    taxaRecarga = 0.3f;
    raioColisao = 8.0f;
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
}

void BarcoSolar::setIntensidadeSolar(float valor) {
    if (valor < 0.0f) valor = 0.0f;
    if (valor > 1.0f) valor = 1.0f;
    intensidadeSolar = valor;
}

void BarcoSolar::aplicarForca(Ponto2D forca) {
    Ponto2D f = forca / massa;
    aceleracao.add(f);
}

void BarcoSolar::aplicarHidrodinamica() {
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

    float coefQuilha = COEF_ARRASTO_CASCO * 400.0f;
    Ponto2D vecArrastoLado = velLateral * -coefQuilha * speed;
    aplicarForca(vecArrastoLado);
}

void BarcoSolar::verificarLimitesMapa() {
    float amortecimento = -0.3f;
    if (posicao.x < 0) { posicao.x = 0; velocidade.x *= amortecimento; }
    if (posicao.x > MAPA_LARGURA) { posicao.x = MAPA_LARGURA; velocidade.x *= amortecimento; }
    if (posicao.y < 0) { posicao.y = 0; velocidade.y *= amortecimento; }
    if (posicao.y > MAPA_ALTURA) { posicao.y = MAPA_ALTURA; velocidade.y *= amortecimento; }
}

//Implementação do "Unstick"
void BarcoSolar::chocar(Ponto2D posObstaculo) {
    // Inverte a velocidade (Ricochete)
    velocidade.mult(-0.3f);

    // Empurrão de segurança para fora do objeto
    // Calcula o vetor que aponta DO obstáculo PARA o barco
    Ponto2D direcaoFuga = posicao - posObstaculo;
    direcaoFuga.normalize();

    // Teleporta o barco 10 pixels para longe
    posicao.add(direcaoFuga * 10.0f);
}

void BarcoSolar::atualizar(float deltaTime) {
    aplicarHidrodinamica();

    if (velocidadeAngular != 0) {
        float arrastoRotacao = -6.0f * velocidadeAngular;
        aceleracaoAngular += arrastoRotacao;
    }

    velocidadeAngular += aceleracaoAngular * deltaTime;
    angulo += velocidadeAngular * deltaTime;
    aceleracaoAngular = 0.0f;

    Ponto2D mudancaVelocidade = aceleracao * deltaTime;
    velocidade.add(mudancaVelocidade);

    Ponto2D deslocamento = velocidade * deltaTime;
    posicao.add(deslocamento);

    verificarLimitesMapa();
    aceleracao.mult(0);
    carregarBateria();
}

void BarcoSolar::acelerar() {
    if (nivelBateria > taxaConsumo) {
        float rad = angulo * (PI / 180.0f);
        Ponto2D forcaMotor(std::cos(rad), std::sin(rad));
        float forcaPixels = FORCA_MOTOR_NEWTONS * (ESCALA / 10.0f);
        forcaMotor.mult(forcaPixels);
        aplicarForca(forcaMotor);
        nivelBateria -= taxaConsumo;
    }
}

void BarcoSolar::virar(float val) {
    float speed = velocidade.mag();
    float eficienciaLeme = std::clamp(speed / 150.0f, 0.2f, 1.0f);
    float torque = val * 200.0f * eficienciaLeme;
    aceleracaoAngular += torque;
}

void BarcoSolar::carregarBateria() {
    if (nivelBateria < capacidadeBateria) {
        nivelBateria += taxaRecarga * intensidadeSolar;
    }
}

void BarcoSolar::buscarAlvo(Ponto2D alvo) {
    Ponto2D desejado = alvo - posicao;
    float anguloDesejadoRad = std::atan2(desejado.y, desejado.x);
    float anguloDesejadoGraus = anguloDesejadoRad * (180.0f / PI);

    float erro = anguloDesejadoGraus - angulo;
    while (erro > 180) erro -= 360;
    while (erro < -180) erro += 360;

    if (erro > 10.0f) virar(3.0f);
    else if (erro < -10.0f) virar(-3.0f);

    if (std::abs(erro) < 90.0f || velocidade.mag() < 50.0f) {
        acelerar();
    }
}

void BarcoSolar::incrementarCheckpoint(int totalCheckpoints) {
    if (proximoCheckpointId == 0) voltaAtual++;
    proximoCheckpointId++;
    if (proximoCheckpointId >= totalCheckpoints) proximoCheckpointId = 0;
}

Ponto2D BarcoSolar::getPosicao() const { return posicao; }
float BarcoSolar::getAngulo() const { return angulo; }
float BarcoSolar::getBateria() const { return nivelBateria; }
float BarcoSolar::getVelocidadeAtual() const { return velocidade.mag(); }
int BarcoSolar::getProximoCheckpointId() const { return proximoCheckpointId; }
int BarcoSolar::getVoltaAtual() const { return voltaAtual; }
float BarcoSolar::getRaioColisao() const { return raioColisao; }
