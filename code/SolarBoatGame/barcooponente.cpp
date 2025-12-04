#include "barcooponente.h"
#include "pista.h"

BarcoOponente::BarcoOponente(float x, float y, QString nome)
    : BarcoSolar(x, y, nome)
{
    massa = 520.0f; // Levemente mais pesado para diferenciar
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
