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
    void setAppState(QString string);

private:
    Ui::MainWindow *ui;
    void keyReleaseEvent ( QKeyEvent * event );

signals:
    void setState(QString state);

};

#endif // MAINWINDOW_H
