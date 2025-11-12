#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene> // <--- O Palco
#include <QGraphicsView>  // <--- A Câmera
#include <QGraphicsRectItem> // <--- Para um teste visual temporário
#include "barcosolar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void atualizarJogo();

private:
    Ui::MainWindow *ui;

    // Elementos Gráficos
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsRectItem *barcoVisual; // Um quadrado simples para representar o barco por enquanto

    // Lógica
    QTimer *timer;
    BarcoSolar *barco;
    int frameCount;
};

#endif // MAINWINDOW_H
