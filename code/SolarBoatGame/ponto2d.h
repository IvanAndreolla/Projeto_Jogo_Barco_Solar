#ifndef PONTO2D_H
#define PONTO2D_H

#include <cmath>

class Ponto2D
{
public:
    float x;
    float y;

    // Construtores
    Ponto2D();
    Ponto2D(float x_, float y_);

    // Operações Vetoriais (Física)
    void add(Ponto2D v);          // Soma vetor
    void mult(float n);           // Escala vetor
    float mag() const;            // Magnitude (tamanho)
    void normalize();             // Normaliza (tamanho = 1)
    void limit(float max);        // Limita magnitude máxima

    // Sobrecarga de Operadores
    Ponto2D operator+(const Ponto2D& v) const;
    Ponto2D operator-(const Ponto2D& v) const;
    Ponto2D operator*(float n) const;
    Ponto2D operator/(float n) const;
};

#endif // PONTO2D_H
