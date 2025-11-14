#include "inputmanager.h"

InputManager::InputManager() {}

void InputManager::processarPressaoTecla(QKeyEvent *event) {
    teclasPressionadas.insert(event->key());
}

void InputManager::processarSolturaTecla(QKeyEvent *event) {
    teclasPressionadas.remove(event->key());
}

bool InputManager::isTeclaPressionada(int tecla) const {
    return teclasPressionadas.contains(tecla);
}
