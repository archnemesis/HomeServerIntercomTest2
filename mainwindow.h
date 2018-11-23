#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioOutput>
#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QSslSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QBuffer>
#include <QMutex>
#include <QByteArray>

#include "messageparser.h"

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
    void on_talkButton_clicked();
    void on_audioOutput_stateChanged(QAudio::State state);
    void on_audioOutput_notify();
    void on_audioInput_stateChanged(QAudio::State state);
    void on_audioInput_notify();
    void on_udpSocket_readyRead();
    void on_DisconnectButton_clicked();
    void on_controlSocket_readyRead();
    void on_controlSocket_connected();
    void on_controlSocket_disconnected();
    void on_controlSocket_encrypted();
    void on_controlSocket_sslErrors(const QList<QSslError>&errors);
    void on_endButton_clicked();
    void on_messageParser_messageReceived(message_t *message);

    void on_certBrowseButton_clicked();

protected:
    void sendMessage(message_t *message);
    void logEventMessage(const QString &message);
    void openIntercomChannel(const QString &address, const int port);

private:
    Ui::MainWindow *ui;
    QAudioInput *m_audioInput = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QSslSocket *m_controlSocket = nullptr;
    QUdpSocket *m_udpSocket = nullptr;
    QIODevice *m_audioInputDevice = nullptr;
    QIODevice *m_audioOutputDevice = nullptr;
    QMutex m_bufferMutex;
    QByteArray m_buffer;
    MessageParser *m_parser = nullptr;
};

#endif // MAINWINDOW_H
