#include "barcooponente.h"
#include "pista.h"

BarcoOponente::BarcoOponente(float x, float y) : BarcoSolar(x, y) {
    // Massa levemente diferente para ter uma "personalidade" distinta do jogador
    massa = 320.0f;
}

void BarcoOponente::tomarDecisao(const Pista* pista) {
    //  Descobre qual o próximo checkpoint
    int idAlvo = this->getProximoCheckpointId();
    const auto& checkpoints = pista->getCheckpoints();

    //  Se o alvo existir, define a rota
    if (idAlvo >= 0 && idAlvo < checkpoints.size()) {
        Checkpoint* cp = checkpoints[idAlvo];
        Ponto2D alvo = cp->getCentro();

        // IA Simples: Apenas vira e acelera em direção ao alvo
        // Ignora obstáculos
        this->buscarAlvo(alvo);
    }
}
