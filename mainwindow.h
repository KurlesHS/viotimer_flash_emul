#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum State {
        FirstPacket,
        MiddlePacket
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void logMessage(const QString &message);
    void onReadyRead();
private:
    void updateButtons();
    void open();
    void close();

private slots:
    void on_pushButtonOpenClose_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *mSerialPort;
    State mCurrentState;
};

#endif // MAINWINDOW_H
