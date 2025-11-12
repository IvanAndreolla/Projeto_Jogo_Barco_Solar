#include "barcosolar.h"
#include <cmath>

// Constantes de Física
const float FORCA_MOTOR = 15.0f;  // Aumentei um pouco para ficar mais responsivo
const float COEF_ARRASTO = 0.05f; // Resistência da água
const float PI = 3.14159265f;

BarcoSolar::BarcoSolar(float x, float y) {
    posicao = Ponto2D(x, y);
    velocidade = Ponto2D(0, 0);
    aceleracao = Ponto2D(0, 0);

    massa = 1.0f;
    angulo = 0.0f; // 0 graus = Direita/Leste

    nivelBateria = 100.0f;
    capacidadeBateria = 100.0f;
    taxaConsumo = 0.2f;
    taxaRecarga = 0.05f;

    voltaAtual = 0;
    proximoCheckpointId = 0;
}

void BarcoSolar::aplicarForca(Ponto2D forca) {
    // F = M * A -> A = F / M
    Ponto2D f = forca / massa;
    aceleracao.add(f);
}

void BarcoSolar::aplicarArrasto() {
    // Força de arrasto oposta à velocidade
    float speed = velocidade.mag();
    if (speed > 0) {
        Ponto2D forcaArrasto = velocidade;
        forcaArrasto.normalize();
        forcaArrasto.mult(-1);
        forcaArrasto.mult(COEF_ARRASTO * speed * speed);
        aplicarForca(forcaArrasto);
    }
}

void BarcoSolar::atualizar(float deltaTime) {
    aplicarArrasto();

    // Integração de Euler
    velocidade.add(aceleracao);

    // Limite físico de velocidade
    velocidade.limit(15.0f);

    // Atualiza posição considerando o tempo (S = So + V*t)
    Ponto2D deslocamento = velocidade * deltaTime;
    posicao.add(deslocamento);

    // Limpa aceleração
    aceleracao.mult(0);

    carregarBateria();
}

void BarcoSolar::acelerar() {
    if (nivelBateria > taxaConsumo) {
        // Converte ângulo para vetor de força
        float rad = angulo * (PI / 180.0f);
        Ponto2D forcaMotor(std::cos(rad), std::sin(rad));

        forcaMotor.mult(FORCA_MOTOR);
        aplicarForca(forcaMotor);

        nivelBateria -= taxaConsumo;
    }
}

void BarcoSolar::virar(float val) {
    // Barco só vira se tiver movimento mínimo
    if (velocidade.mag() > 0.1f) {
        angulo += val;
    }
}

void BarcoSolar::carregarBateria() {
    if (nivelBateria < capacidadeBateria) {
        nivelBateria += taxaRecarga;
    }
}

// Getters
Ponto2D BarcoSolar::getPosicao() const { return posicao; }
float BarcoSolar::getAngulo() const { return angulo; }
float BarcoSolar::getBateria() const { return nivelBateria; }
