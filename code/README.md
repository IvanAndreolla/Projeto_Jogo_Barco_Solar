# Instruções para compilação e execução do sistema

>[!NOTE]
> Este documento descreve como compilar e executar o "Solar Boat The Game" a partir dos arquivos-fonte via terminal.

## Pré-requisitos

Antes de compilar, garanta que você tenha os seguintes softwares instalados:
* **Bibliotecas de Desenvolvimento do Qt 6:** (`qt6-base-dev` ou similar no Ubuntu).
* **Compilador C++:** `g++` ou `clang`.
* **Utilitário `qmake`:** (ex: `qmake6`).
* **Utilitário `make`**.

---

## Compilando e Executando via Terminal (Ubuntu/Linux)

1.  Abra um terminal e navegue até esta pasta:
    ```bash
    cd caminho/para/o/projeto/code
    ```

2.  Execute o `qmake` para gerar o `Makefile` a partir do arquivo de projeto:
    ```bash
    # Você pode precisar usar qmake6 dependendo da sua instalação
    qmake SolarBoatGame.pro
    ```

3.  Execute o `make` para compilar o projeto. Isso pode levar alguns momentos:
    ```bash
    make
    ```

4.  Após a compilação ser bem-sucedida, um arquivo executável será criado na pasta (ou em uma pasta `build`). Execute-o:
    ```bash
    ./SolarBoatGame
    ```