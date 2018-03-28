#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define FRAME_SIZE 320

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->DisconnectButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    QHostAddress addr(ui->hostAddress->text());
    int port = 2050;

    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->bind(port);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::on_udpSocket_readyRead);

    ui->hostAddress->setEnabled(false);
    ui->hostPort->setEnabled(false);
    ui->connectButton->setEnabled(false);
    ui->DisconnectButton->setEnabled(true);

    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default format unsupported!";
        return;
    }

    m_audioInput = new QAudioInput(format, this);
    m_audioInput->setNotifyInterval(10);
    m_audioInput->setBufferSize(FRAME_SIZE * 2);
    connect(m_audioInput, &QAudioInput::stateChanged, this, &MainWindow::on_audioInput_stateChanged);
    connect(m_audioInput, &QAudioInput::notify, this, &MainWindow::on_audioInput_notify);

    info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default format unsupported!";
        return;
    }

    m_audioOutput = new QAudioOutput(format, this);
    m_audioOutput->setNotifyInterval(10);
    m_audioOutput->setBufferSize(FRAME_SIZE * 2);
    connect(m_audioOutput, &QAudioOutput::stateChanged, this, &MainWindow::on_audioOutput_stateChanged);
    connect(m_audioOutput, &QAudioOutput::notify, this, &MainWindow::on_audioOutput_notify);
    ui->outputBufferProgress->setMaximum(m_audioOutput->bufferSize());
    ui->outputBufferProgress->setValue(0);
    ui->networkBufferProgress->setMaximum(FRAME_SIZE * 10);
    ui->networkBufferProgress->setValue(0);

    qDebug() << "Buffer Size:" << m_audioInput->bufferSize();
    qDebug() << "Notify Interval:" << m_audioInput->notifyInterval();
    ui->audioBufferProgress->setMaximum(m_audioInput->bufferSize());
    ui->audioBufferProgress->setValue(0);

    m_audioInputDevice = m_audioInput->start();
}

void MainWindow::on_audioOutput_stateChanged(QAudio::State state)
{
    switch (state) {
    case QAudio::ActiveState:
        qDebug("QAudio now in active state!");
        break;
    case QAudio::SuspendedState:
        qDebug("QAudio now suspended");
        break;
    case QAudio::StoppedState:
        qDebug("QAudio now stopped");
        break;
    case QAudio::IdleState:
        qDebug("QAudio now idle");
        break;
    default:
        qDebug("Other state...");
        break;
    }
}

void MainWindow::on_audioOutput_notify()
{
    m_bufferMutex.lock();

    int towrite = MIN(m_audioOutput->bytesFree(), m_buffer.size());
    if (towrite > 0) {
        m_audioOutputDevice->write(m_buffer.data(), towrite);
        m_buffer.remove(0, towrite);
        ui->outputBufferProgress->setValue(m_audioOutput->bufferSize() - m_audioOutput->bytesFree());
        ui->networkBufferProgress->setValue(m_buffer.size());
    }

    m_bufferMutex.unlock();
}

void MainWindow::on_audioInput_stateChanged(QAudio::State state)
{
    switch (state) {
    case QAudio::ActiveState:
        qDebug("QAudio now in active state!");
        break;
    case QAudio::SuspendedState:
        qDebug("QAudio now suspended");
        break;
    case QAudio::StoppedState:
        qDebug("QAudio now stopped");
        break;
    case QAudio::IdleState:
        qDebug("QAudio now idle");
        if (m_audioInput->error() != QAudio::NoError) {
            qDebug() << "QAudio Error " << m_audioInput->error();
        }
        else {
            m_audioInput->resume();
        }
        break;
    default:
        qDebug("Other state...");
        break;
    }
}

void MainWindow::on_audioInput_notify()
{
    QByteArray buf = m_audioInputDevice->read(FRAME_SIZE);
    m_udpSocket->writeDatagram(buf, QHostAddress("10.1.1.141"), 2050);

    int bytesReady = m_audioInput->bytesReady();
    if (bytesReady > ui->audioBufferProgress->maximum()) {
        ui->audioBufferProgress->setMaximum(bytesReady);
    }
    ui->audioBufferProgress->setValue(m_audioInput->bytesReady());
}

void MainWindow::on_udpSocket_readyRead()
{
    QNetworkDatagram dg = m_udpSocket->receiveDatagram(FRAME_SIZE);
    m_bufferMutex.lock();
    m_buffer.append(dg.data());
    m_bufferMutex.unlock();

    if (m_buffer.count() > (FRAME_SIZE * 10) && m_audioOutput->state() == QAudio::IdleState) {
        m_audioOutput->resume();
    }
}

void MainWindow::on_DisconnectButton_clicked()
{
    m_audioInput->stop();
    m_udpSocket->close();

    ui->audioBufferProgress->setValue(0);
    ui->hostAddress->setEnabled(true);
    ui->hostPort->setEnabled(true);
    ui->connectButton->setEnabled(true);
    ui->DisconnectButton->setEnabled(false);
}
