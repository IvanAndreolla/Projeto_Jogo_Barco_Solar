#include "ponto2d.h"

Ponto2D::Ponto2D() : x(0), y(0) {}
Ponto2D::Ponto2D(float x_, float y_) : x(x_), y(y_) {}

void Ponto2D::add(Ponto2D v) { x += v.x; y += v.y; }
void Ponto2D::mult(float n) { x *= n; y *= n; }

float Ponto2D::mag() const { return std::sqrt(x*x + y*y); }

void Ponto2D::normalize() {
    float m = mag();
    if (m != 0) mult(1 / m);
}

void Ponto2D::limit(float max) {
    if (mag() > max) {
        normalize();
        mult(max);
    }
}

float Ponto2D::dist(Ponto2D v) const {
    float dx = x - v.x;
    float dy = y - v.y;
    return std::sqrt(dx*dx + dy*dy);
}

Ponto2D Ponto2D::operator+(const Ponto2D& v) const { return Ponto2D(x + v.x, y + v.y); }
Ponto2D Ponto2D::operator-(const Ponto2D& v) const { return Ponto2D(x - v.x, y - v.y); }
Ponto2D Ponto2D::operator*(float n) const { return Ponto2D(x * n, y * n); }
Ponto2D Ponto2D::operator/(float n) const { return Ponto2D(x / n, y / n); }
