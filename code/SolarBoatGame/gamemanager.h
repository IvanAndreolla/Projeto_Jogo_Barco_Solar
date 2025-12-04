#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QFrame>

#include "barcojogador.h"
#include "barcooponente.h"
#include "barcografico.h"
#include "inputmanager.h"
#include "pista.h"

class GameManager : public QObject
{
    Q_OBJECT
public:
    // Inicializa gerenciador, conecta UI e configura cena gráfica
    GameManager(QGraphicsScene *scene, QGraphicsView *view,
                QFrame *painel,
                QProgressBar *bar, QLabel *labelVel, QLabel *labelVoltas, QLabel *labelPos,
                QPushButton *btn1, QPushButton *btn2, QPushButton *btn3, QPushButton *btnReiniciar,
                QPushButton *btnRetomar, QPushButton *btnMenuPrincipal,
                QLabel *titulo,
                QObject *parent = nullptr);
    ~GameManager();

    // Delega eventos de teclado para o InputManager
    void processarEvento(QKeyEvent *event, bool pressionado);

public slots:
    // Loop principal: atualiza física, IA e renderização
    void gameLoop();

    // Seletores de nível
    void iniciarPista1();
    void iniciarPista2();
    void iniciarPista3();
    // Reinicia o nível atual
    void reiniciarFase();

    // Controle de fluxo (Pause/Resume/Menu)
    void pausarJogo();
    void retomarJogo();
    void voltarAoMenu();

private:
    // Máquina de estados do jogo
    enum EstadoJogo { MENU, JOGANDO, GAMEOVER, PAUSADO };
    EstadoJogo estadoAtual;
    Pista::Tipo pistaAtualSelecionada;

    bool modoDebug;

    // Configura nova partida e instância entidades
    void iniciarJogo(Pista::Tipo tipo);
    // Trata condição de vitória/derrota e para o loop
    void fimDeJogo(bool venceu);
    // Atualiza elementos do HUD (velocímetro, bateria, voltas)
    void atualizarInterface();

    // Sistema de ambiente (nuvens)
    void criarNuvens();
    void atualizarNuvens();
    // Ordena eixo Z dos objetos para oclusão correta (Z-sort)
    void atualizarProfundidadeVisual();
    // Recalcula posições dos competidores
    void atualizarRanking();

    // Referências para componentes gráficos e widgets
    QGraphicsScene *scene;
    QGraphicsView *view;
    QFrame *painelHUD;
    QProgressBar *barraBateria;
    QLabel *labelVelocidade;
    QLabel *labelVoltas;
    QLabel *labelPosicao;

    // Botões de controle
    QPushButton *btnPista1;
    QPushButton *btnPista2;
    QPushButton *btnPista3;
    QPushButton *btnReiniciar;
    QPushButton *btnRetomar;
    QPushButton *btnMenuPrincipal;
    QLabel *labelTitulo;

    QTimer *timer;

    // Entidades do Jogador (Lógica Física + Visual)
    BarcoJogador *jogadorLogico;
    BarcoGrafico *jogadorVisual;

    // Entidades dos Oponentes (IA Lógica + Visual)
    QList<BarcoOponente*> oponentesLogicos;
    QList<BarcoGrafico*> oponentesVisuais;

    // Listas para gerenciamento de classificação
    QList<BarcoSolar*> todosBarcos;
    QList<BarcoSolar*> barcosFinalizados;

    // Estrutura auxiliar para decoração
    struct Nuvem {
        QGraphicsPixmapItem* visual;
        float velocidade;
    };
    QList<Nuvem> listaNuvens;
    QPixmap texturaNuvem;

    InputManager *inputManager;
    Pista *pista; // Mapa físico de colisões

    // Cache de posições anteriores (física ou colisão)
    Ponto2D posicaoAnteriorJogador;
    QList<Ponto2D> posicoesAnterioresOponentes;
};

#endif // GAMEMANAGER_H
