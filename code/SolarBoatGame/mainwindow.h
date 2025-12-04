#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include "gamemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Captura entrada de teclado para controle do barco
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // Ajusta o tamanho da view e elementos de UI ao redimensionar a janela
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;

    // Motor de renderização gráfica (cena e viewport)
    QGraphicsScene *scene;
    QGraphicsView *view;

    // Container para sobreposição de UI (HUD) sobre o jogo
    QWidget *uiLayer;

    // Painel de Heads-Up Display
    QFrame *painelHUD;

    // Indicadores de status do jogo
    QProgressBar *barraBateria;
    QLabel *labelVelocidade;
    QLabel *labelVoltas;
    QLabel *labelPosicao;

    // Botões de seleção de nível
    QPushButton *btnPista1;
    QPushButton *btnPista2;
    QPushButton *btnPista3;

    // Botão de reinício rápido
    QPushButton *btnReiniciar;

    // Botões de controle de fluxo (Pause/Menu)
    QPushButton *btnRetomar;
    QPushButton *btnMenuPrincipal;

    QLabel *labelTitulo;

    // Controlador principal da lógica do jogo
    GameManager *gameManager;
};

#endif // MAINWINDOW_H
