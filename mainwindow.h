#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <jsoncpp/json/json.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void draw(QImage img);
    void setValues (Json::Value values);
    void setUserImage (QImage img);

private:
    Ui::MainWindow *ui;
    void keyReleaseEvent ( QKeyEvent * event );

};

#endif // MAINWINDOW_H
