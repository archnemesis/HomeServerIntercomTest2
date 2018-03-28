#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioOutput>
#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QBuffer>
#include <QMutex>
#include <QByteArray>

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
    void on_connectButton_clicked();
    void on_audioOutput_stateChanged(QAudio::State state);
    void on_audioOutput_notify();
    void on_audioInput_stateChanged(QAudio::State state);
    void on_audioInput_notify();
    void on_udpSocket_readyRead();
    void on_DisconnectButton_clicked();

private:
    Ui::MainWindow *ui;
    QAudioInput *m_audioInput = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QUdpSocket *m_udpSocket = nullptr;
    QIODevice *m_audioInputDevice = nullptr;
    QIODevice *m_audioOutputDevice = nullptr;
    QMutex m_bufferMutex;
    QByteArray m_buffer;
};

#endif // MAINWINDOW_H
