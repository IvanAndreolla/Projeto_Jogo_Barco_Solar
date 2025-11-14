#include "barcooponente.h"
#include "pista.h"

BarcoOponente::BarcoOponente(float x, float y) : BarcoSolar(x, y) {
    // Ajuste na performance para diferenciar do jogador
    massa = 4.2f;
}

void BarcoOponente::tomarDecisao(const Pista* pista) {
    int idAlvo = this->getProximoCheckpointId();
    const auto& checkpoints = pista->getCheckpoints();

    // Se o ID for válido, busca o centro do checkpoint
    if (idAlvo >= 0 && idAlvo < checkpoints.size()) {
        Checkpoint* cp = checkpoints[idAlvo];
        Ponto2D alvo = cp->getCentro();
        this->buscarAlvo(alvo);
    }
}
