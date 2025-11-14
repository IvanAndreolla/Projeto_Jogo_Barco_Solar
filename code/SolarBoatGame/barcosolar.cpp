#include "barcosolar.h"
#include <cmath>
#include <algorithm>

const float FORCA_MOTOR = 450.0f;
const float PI = 3.14159265f;
const float MAPA_LARGURA = 5000.0f;
const float MAPA_ALTURA = 5000.0f;

BarcoSolar::BarcoSolar(float x, float y) {
    posicao = Ponto2D(x, y);
    velocidade = Ponto2D(0, 0);
    aceleracao = Ponto2D(0, 0);
    massa = 4.0f;
    angulo = 90.0f;
    velocidadeAngular = 0.0f;
    aceleracaoAngular = 0.0f;

    nivelBateria = 1000.0f;
    capacidadeBateria = 100000.0f;
    taxaConsumo = 0.5f;
    taxaRecarga = 0.1f;

    voltaAtual = 0;          // Começa na volta 0 (aquecimento/largada)
    proximoCheckpointId = 0; // Alvo inicial é a linha de largada
}

void BarcoSolar::aplicarForca(Ponto2D forca) {
    Ponto2D f = forca / massa;
    aceleracao.add(f);
}

void BarcoSolar::aplicarHidrodinamica() {
    if (velocidade.mag() < 0.1f) return;

    float rad = angulo * (PI / 180.0f);
    Ponto2D heading(std::cos(rad), std::sin(rad));
    heading.normalize();

    float velFrontalMag = (velocidade.x * heading.x) + (velocidade.y * heading.y);
    Ponto2D velFrontal = heading * velFrontalMag;
    Ponto2D velLateral = velocidade - velFrontal;

    aplicarForca(velFrontal * -0.01f);
    aplicarForca(velLateral * -3.0f);
}

void BarcoSolar::verificarLimitesMapa() {
    float amortecimento = -0.3f;
    if (posicao.x < 0) { posicao.x = 0; velocidade.x *= amortecimento; }
    if (posicao.x > MAPA_LARGURA) { posicao.x = MAPA_LARGURA; velocidade.x *= amortecimento; }
    if (posicao.y < 0) { posicao.y = 0; velocidade.y *= amortecimento; }
    if (posicao.y > MAPA_ALTURA) { posicao.y = MAPA_ALTURA; velocidade.y *= amortecimento; }
}

void BarcoSolar::chocar() {
    velocidade.mult(-0.5f);
    posicao.add(velocidade * 0.2f);
}

void BarcoSolar::atualizar(float deltaTime) {
    aplicarHidrodinamica();

    if (velocidadeAngular != 0) {
        float arrastoRotacao = velocidadeAngular * -3.0f;
        aceleracaoAngular += arrastoRotacao;
    }

    velocidadeAngular += aceleracaoAngular * deltaTime;
    angulo += velocidadeAngular * deltaTime;
    aceleracaoAngular = 0.0f;

    velocidade.add(aceleracao * deltaTime);
    velocidade.limit(300.0f);
    posicao.add(velocidade * deltaTime);

    verificarLimitesMapa();
    aceleracao.mult(0);
    carregarBateria();
}

void BarcoSolar::acelerar() {
    if (nivelBateria > taxaConsumo) {
        float rad = angulo * (PI / 180.0f);
        Ponto2D forcaMotor(std::cos(rad), std::sin(rad));
        forcaMotor.mult(FORCA_MOTOR);
        aplicarForca(forcaMotor);
        nivelBateria -= taxaConsumo;
    }
}

void BarcoSolar::virar(float val) {
    float speed = velocidade.mag();
    float eficienciaLeme = std::clamp(speed / 50.0f, 0.2f, 1.0f);
    float torque = val * 40.0f * eficienciaLeme;
    aceleracaoAngular += torque;
}

void BarcoSolar::carregarBateria() {
    if (nivelBateria < capacidadeBateria) nivelBateria += taxaRecarga;
}

void BarcoSolar::buscarAlvo(Ponto2D alvo) {
    Ponto2D desejado = alvo - posicao;
    float anguloDesejadoRad = std::atan2(desejado.y, desejado.x);
    float anguloDesejadoGraus = anguloDesejadoRad * (180.0f / PI);

    float erro = anguloDesejadoGraus - angulo;
    while (erro > 180) erro -= 360;
    while (erro < -180) erro += 360;

    if (erro > 5.0f) virar(3.0f);
    else if (erro < -5.0f) virar(-3.0f);

    if (std::abs(erro) < 60.0f || velocidade.mag() < 20.0f) {
        acelerar();
    }
}

void BarcoSolar::incrementarCheckpoint(int totalCheckpoints) {
    // Se o checkpoint é o 0
    // Significa que cruzamos a linha de chegada/largada.
    if (proximoCheckpointId == 0) {
        voltaAtual++;
    }

    // Avança para o próximo alvo
    proximoCheckpointId++;

    // Se passou do último, volta a buscar o 0
    if (proximoCheckpointId >= totalCheckpoints) {
        proximoCheckpointId = 0;
    }
}

Ponto2D BarcoSolar::getPosicao() const { return posicao; }
float BarcoSolar::getAngulo() const { return angulo; }
float BarcoSolar::getBateria() const { return nivelBateria; }
float BarcoSolar::getVelocidadeAtual() const { return velocidade.mag(); }
int BarcoSolar::getProximoCheckpointId() const { return proximoCheckpointId; }
int BarcoSolar::getVoltaAtual() const { return voltaAtual; }
