#ifndef BARCOOPONENTE_H
#define BARCOOPONENTE_H

#include "barcosolar.h"

class Pista;

class BarcoOponente : public BarcoSolar
{
public:
    BarcoOponente(float x, float y, QString nome);

    // Implementação Polimórfica: A CPU ignora o input e usa a Pista para decidir
    void controlar(InputManager* input, const Pista* pista) override;

    // Lógica interna da IA
    void tomarDecisao(const Pista* pista);
};

#endif // BARCOOPONENTE_H
