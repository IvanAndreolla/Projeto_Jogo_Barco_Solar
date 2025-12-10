#include "barcooponente.h"
#include "pista.h"
#include "inputmanager.h"

BarcoOponente::BarcoOponente(float x, float y, QString nome)
    : BarcoSolar(x, y, nome)
{
    massa = 450.0f; // Altera o peso mudando a dificuldade do jogo
}

void BarcoOponente::controlar(InputManager *input, const Pista *pista) {
    Q_UNUSED(input);
    tomarDecisao(pista);
}

void BarcoOponente::tomarDecisao(const Pista* pista) {
    int idAlvo = this->getProximoCheckpointId();
    const auto& checkpoints = pista->getCheckpoints();

    if (idAlvo >= 0 && idAlvo < checkpoints.size()) {
        Checkpoint* cp = checkpoints[idAlvo];
        Ponto2D alvo = cp->getCentro();
        this->buscarAlvo(alvo);
    }
}
