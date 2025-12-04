#include "ponto2d.h"

// Construtores: Padrão (Origem) e Parametrizado
Ponto2D::Ponto2D() : x(0), y(0) {}
Ponto2D::Ponto2D(float x_, float y_) : x(x_), y(y_) {}

// Modifica o vetor atual somando componentes
void Ponto2D::add(Ponto2D v) { x += v.x; y += v.y; }

// Modifica o vetor atual multiplicando por escalar
void Ponto2D::mult(float n) { x *= n; y *= n; }

// Retorna a magnitude (norma euclidiana) do vetor
float Ponto2D::mag() const { return std::sqrt(x*x + y*y); }

// Transforma em vetor unitário (magnitude = 1) mantendo a direção
void Ponto2D::normalize() {
    float m = mag();
    if (m != 0) mult(1 / m);
}

// Restringe a magnitude máxima do vetor
void Ponto2D::limit(float max) {
    if (mag() > max) {
        normalize();
        mult(max);
    }
}

// Calcula distância euclidiana entre dois pontos
float Ponto2D::dist(Ponto2D v) const {
    float dx = x - v.x;
    float dy = y - v.y;
    return std::sqrt(dx*dx + dy*dy);
}

// Sobrecargas de operadores aritméticos (retornam nova instância)
Ponto2D Ponto2D::operator+(const Ponto2D& v) const { return Ponto2D(x + v.x, y + v.y); }
Ponto2D Ponto2D::operator-(const Ponto2D& v) const { return Ponto2D(x - v.x, y - v.y); }
Ponto2D Ponto2D::operator*(float n) const { return Ponto2D(x * n, y * n); }
Ponto2D Ponto2D::operator/(float n) const { return Ponto2D(x / n, y / n); }
