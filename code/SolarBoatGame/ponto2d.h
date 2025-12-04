#ifndef PONTO2D_H
#define PONTO2D_H

#include <cmath>

class Ponto2D
{
public:
    // Coordenadas cartesianas
    float x;
    float y;

    // Construtores padrão e parametrizado
    Ponto2D();
    Ponto2D(float x_, float y_);

    // Métodos modificadores (alteram o objeto atual)
    void add(Ponto2D v);      // Soma vetorial acumulativa
    void mult(float n);       // Multiplicação por escalar
    float mag() const;        // Calcula magnitude (comprimento) do vetor
    void normalize();         // Converte em vetor unitário (magnitude 1)
    void limit(float max);    // Restringe a magnitude ao valor máximo
    float dist(Ponto2D v) const; // Calcula distância euclidiana para outro ponto

    // Sobrecarga de operadores aritméticos (retornam nova instância)
    Ponto2D operator+(const Ponto2D& v) const;
    Ponto2D operator-(const Ponto2D& v) const;
    Ponto2D operator*(float n) const;
    Ponto2D operator/(float n) const;
};

#endif // PONTO2D_H
