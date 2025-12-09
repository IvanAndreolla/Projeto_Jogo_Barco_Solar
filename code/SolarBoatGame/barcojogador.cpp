#include "barcojogador.h"

BarcoJogador::BarcoJogador(float x, float y, QString nome)
    : BarcoSolar(x, y, nome)
{
    // Física padrão
}

void BarcoJogador::controlar(InputManager *input, const Pista *pista) {
    Q_UNUSED(pista);
    processarInput(input);
}

void BarcoJogador::processarInput(InputManager *input) {
    if (input->isTeclaPressionada(Qt::Key_W)) {
        this->acelerar();
    }
    if (input->isTeclaPressionada(Qt::Key_A)) {
        this->virar(-3.0f);
    }
    if (input->isTeclaPressionada(Qt::Key_D)) {
        this->virar(3.0f);
    }
}
