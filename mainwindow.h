#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QBuffer>

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
    void on_audioInput_stateChanged(QAudio::State state);
    void on_audioInput_notify();
    void on_udpSocket_readyRead();
    void on_DisconnectButton_clicked();

private:
    Ui::MainWindow *ui;
    QAudioInput *m_audioInput;
    QUdpSocket *m_udpSocket;
    QIODevice *m_audioInputDevice;
    QBuffer *m_audioBuffer;
};

#endif // MAINWINDOW_H
