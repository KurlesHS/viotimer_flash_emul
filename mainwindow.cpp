#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mSerialPort(nullptr)
{
    ui->setupUi(this);
    for (const QSerialPortInfo &spi :QSerialPortInfo::availablePorts()) {
        ui->comboBox->addItem(spi.portName());
    }
    updateButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::logMessage(const QString &message)
{
    ui->textEdit->append(tr("%0: %1").arg(QTime::currentTime().toString(), message));
}

void MainWindow::onReadyRead()
{
    qDebug() << Q_FUNC_INFO << mSerialPort->bytesAvailable();
    if (mSerialPort && mSerialPort->bytesAvailable() == 34) {
        mSerialPort->readAll();
        switch (mCurrentState) {
        case FirstPacket:
            mSerialPort->write("GOOD");
            mCurrentState = MiddlePacket;
            break;
        case MiddlePacket:
            mSerialPort->write("GOOD");
            break;
        default:
            break;
        }


    }
}

void MainWindow::updateButtons()
{
    if (mSerialPort) {
        ui->pushButtonOpenClose->setText(tr("Закрыть порт"));
    } else {
        ui->pushButtonOpenClose->setText(tr("Открыть порт"));
    }
}



void MainWindow::open()
{
    if (mSerialPort) {
        return;
    }
    logMessage(tr("Попытка открыть последовательный порт %0").arg(ui->comboBox->currentText()));
    mSerialPort = new QSerialPort(ui->comboBox->currentText());
    if (mSerialPort->open(QIODevice::ReadWrite)) {
        mSerialPort->setBaudRate(QSerialPort::Baud38400);
        mSerialPort->setDataBits(QSerialPort::Data8);
        mSerialPort->setFlowControl(QSerialPort::NoFlowControl);
        mSerialPort->setParity(QSerialPort::NoParity);
        mSerialPort->setStopBits(QSerialPort::OneStop);
        mCurrentState = FirstPacket;
        connect(mSerialPort, &QSerialPort::readyRead,
                this, &MainWindow::onReadyRead);
        logMessage(tr("Последовательный порт %0 открыт").arg(ui->comboBox->currentText()));
    } else {
        delete mSerialPort;
        mSerialPort = nullptr;
        logMessage(tr("Не получилось открыть последовательный порт %0").arg(ui->comboBox->currentText()));

    }
}

void MainWindow::close()
{
    if (mSerialPort) {
        mSerialPort->close();
        mSerialPort->deleteLater();
        mSerialPort = 0;
        logMessage(tr("Закрываем соединение с последовательным портом"));
    }
}

void MainWindow::on_pushButtonOpenClose_clicked()
{
    if (!mSerialPort) {
        open();
    } else {
        close();
    }
    updateButtons();
}
