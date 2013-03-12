#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturethread.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Configura l'ambiente grafico
    ui->setupUi(this);

    // Inizializza e avvia il thread per il riconoscimento dell'immagine
    CaptureThread *captureThread = new CaptureThread();
    captureThread->start();
    // Configura l'evento per il disegno dell'UI
    connect(captureThread,SIGNAL(newFrame(QImage)),this,SLOT(draw(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * draw
 * Descrizione: Evento di disegno dell'immagine della webcam sulla label
 * Parametri:
 * - img: immagine da visualizzare
 * */
void MainWindow::draw(QImage img){
    // Scala l'immagine nella dimensione voluta
    img = img.scaled(ui->WidgetImageLabel->width(), ui->WidgetImageLabel->height());
    // Associa l'immagine alla label
    ui->WidgetImageLabel->setPixmap(QPixmap::fromImage(img));
}
