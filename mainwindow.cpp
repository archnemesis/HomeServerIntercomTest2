#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QFileDialog>
#include <QStandardPaths>
#include <QListWidgetItem>
#include "command.h"

const char CACERT[] = "-----BEGIN CERTIFICATE-----\r\n"
    "MIICQDCCAcWgAwIBAgIJAJvz0HNKEO7bMAoGCCqGSM49BAMCMFwxCzAJBgNVBAYT\r\n"
    "AlVTMQ8wDQYDVQQIDAZPcmVnb24xEjAQBgNVBAcMCUhpbGxzYm9ybzEWMBQGA1UE\r\n"
    "CgwNUm9iaW4gR2luZ3JhczEQMA4GA1UECwwHQ2VudHJhbDAeFw0xODAzMTcyMjUy\r\n"
    "MTVaFw0xOTAzMTcyMjUyMTVaMFwxCzAJBgNVBAYTAlVTMQ8wDQYDVQQIDAZPcmVn\r\n"
    "b24xEjAQBgNVBAcMCUhpbGxzYm9ybzEWMBQGA1UECgwNUm9iaW4gR2luZ3JhczEQ\r\n"
    "MA4GA1UECwwHQ2VudHJhbDB2MBAGByqGSM49AgEGBSuBBAAiA2IABCf3KeOEkT1i\r\n"
    "yKPzrDAckZUrQc409KYkE5IgetNz061DZQU75Gn6pldGnud0S1iim4HBf2IhQzbi\r\n"
    "badzZJEVcIDoolOPo5uKYHzLGhlW2JP5BGNI3uBT0X8VpCF2+YhLpKNTMFEwHQYD\r\n"
    "VR0OBBYEFOON2/n1+a89uuN5q0M7/oe1TWn1MB8GA1UdIwQYMBaAFOON2/n1+a89\r\n"
    "uuN5q0M7/oe1TWn1MA8GA1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwIDaQAwZgIx\r\n"
    "AIw2jqNqaJ9In98LO6ez7AvCFadk0ly3+s7XLP8soimig8TIZxn4NIhYB9RsHCex\r\n"
    "nwIxAJGoej1KdWPjJZFrkaNFClQqznImSNe3QGkKinUAhcI4TlAiUz2xmpDkf75m\r\n"
    "un0opg==\r\n"
    "-----END CERTIFICATE-----\r\n";

const char HSCERT[] = "-----BEGIN CERTIFICATE-----\r\n"
    "MIICATCCAYYCAQEwCgYIKoZIzj0EAwIwXDELMAkGA1UEBhMCVVMxDzANBgNVBAgM\r\n"
    "Bk9yZWdvbjESMBAGA1UEBwwJSGlsbHNib3JvMRYwFAYDVQQKDA1Sb2JpbiBHaW5n\r\n"
    "cmFzMRAwDgYDVQQLDAdDZW50cmFsMB4XDTE4MDMxNzIzMDQyM1oXDTE5MDMxNzIz\r\n"
    "MDQyM1owfzELMAkGA1UEBhMCVVMxDzANBgNVBAgMBk9yZWdvbjESMBAGA1UEBwwJ\r\n"
    "SGlsbHNib3JvMRYwFAYDVQQKDA1Sb2JpbiBHaW5ncmFzMQ0wCwYDVQQLDARIb21l\r\n"
    "MSQwIgYDVQQDDBtob21lc2VydmVyLnJvYmluZ2luZ3Jhcy5jb20wdjAQBgcqhkjO\r\n"
    "PQIBBgUrgQQAIgNiAATowO7/AkiRdkvWS4fSaitLlRkf2O5YOEAWf43k5BUb7BwY\r\n"
    "/5p+V1x/0HQ7FsWSQ+3egg1d4suhChYlrdTTf8qO2p1pnDeuVl2TT/y5RmXdhhay\r\n"
    "N3SF344dYhxXi37QmkcwCgYIKoZIzj0EAwIDaQAwZgIxAP2by+lqapub6JusFO0Q\r\n"
    "M2xreE44cSQRo0bvkAUDqxrNmUQupaqrJNiZh3w722DeNwIxAKj0BAz4opPBJKGl\r\n"
    "cDkYQp4to6PzvtJBvPxGDa/9odtELMcJEc6wr12vfZ8uPi27tA==\r\n"
    "-----END CERTIFICATE-----\r\n";

#define MIN(a,b) (((a)<(b))?(a):(b))
#define FRAME_SIZE 320

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->DisconnectButton->setEnabled(false);
    m_parser = new MessageParser(this);
    connect(m_parser, &MessageParser::messageReceived, this, &MainWindow::on_messageParser_messageReceived);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    QString host = ui->hostAddress->text();
    int port = ui->hostPort->text().toInt();

    logEventMessage(QString("Loading certificate..."));
    QByteArray cacertdata = QByteArray::fromRawData((const char *)CACERT, sizeof(CACERT));
    QList<QSslCertificate> cacerts = QSslCertificate::fromPath(ui->cacertPath->text());
    //QList<QSslCertificate> cacerts = QSslCertificate::fromData(cacertdata);

    logEventMessage(QString("Connecting to %1:%2...").arg(host).arg(port));

    m_controlSocket = new QSslSocket(this);
    m_controlSocket->addCaCertificates(cacerts);
    m_controlSocket->setProtocol(QSsl::TlsV1_2);
    connect(m_controlSocket, &QSslSocket::readyRead, this, &MainWindow::on_controlSocket_readyRead);
    connect(m_controlSocket, &QSslSocket::connected, this, &MainWindow::on_controlSocket_connected);
    connect(m_controlSocket, &QSslSocket::disconnected, this, &MainWindow::on_controlSocket_disconnected);
    connect(m_controlSocket, &QSslSocket::encrypted, this, &MainWindow::on_controlSocket_encrypted);
    connect(m_controlSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
        this, &MainWindow::on_controlSocket_sslErrors);
    m_controlSocket->connectToHostEncrypted(host, port, QIODevice::ReadWrite);

}

void MainWindow::on_talkButton_clicked()
{
    QList<QListWidgetItem *> items = ui->contactList->selectedItems();
    for (QListWidgetItem *item : items) {
        QByteArray hwid = item->data(Qt::UserRole).toByteArray();
        logEventMessage(QString("Opening channel to %1").arg(QString(hwid.toHex())));
        intercom_channel_request_message_t request;
        memcpy((void *)&request.hwid_callee, (void *)hwid.data(), 6);
        message_t msg;
        intercom_channel_request_encode(&msg, &request);
        sendMessage(&msg);
        break;
    }
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
    }

    qDebug() << "+";

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
    while (m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram dg = m_udpSocket->receiveDatagram(FRAME_SIZE);
        m_bufferMutex.lock();
        m_buffer.append(dg.data());
        ui->networkBufferProgress->setValue(m_buffer.size());
        qDebug("Size: %d", m_buffer.size());
        m_bufferMutex.unlock();
    }

    if (m_buffer.count() > (FRAME_SIZE * 10) && m_audioOutput->state() == QAudio::IdleState) {
        qDebug() << ".";
        m_audioOutput->resume();
        on_audioOutput_notify();
    }
}

void MainWindow::on_DisconnectButton_clicked()
{
    logEventMessage("Disconnecting from server...");
    m_controlSocket->disconnectFromHost();
}

void MainWindow::on_controlSocket_readyRead()
{
    QByteArray data = m_controlSocket->readAll();
    m_parser->processBytes(data);
}

void MainWindow::on_controlSocket_connected()
{
    qDebug() << "Control socket connected";
    logEventMessage("Connected to HomeServer");
    ui->DisconnectButton->setEnabled(true);
    ui->connectButton->setEnabled(false);
}

void MainWindow::on_controlSocket_disconnected()
{
    qDebug() << "Control socket disconnected";
    logEventMessage("Disconnected from HomeServer");
    m_controlSocket->deleteLater();
    m_controlSocket = nullptr;
    ui->DisconnectButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
}

void MainWindow::on_controlSocket_encrypted()
{
    qDebug() << "Control socket encrypted";
    logEventMessage("Communications channel secured");

    command_message_t cmd;
    cmd.command_id = RequestConfigurationCommand;
    message_t *msg = (message_t *)command_encode_alloc((command_message_t *)&cmd);
    sendMessage(msg);
    free(msg);
}

void MainWindow::on_controlSocket_sslErrors(const QList<QSslError> &errors)
{
    for (QSslError err : errors) {
        qCritical() << err.errorString();
    }
}

void MainWindow::on_endButton_clicked()
{
    logEventMessage("Ending audio stream...");

    m_audioInput->stop();
    m_audioInput->deleteLater();
    m_audioOutput->stop();
    m_audioOutput->deleteLater();
    m_udpSocket->close();
    m_udpSocket->deleteLater();

    m_audioInput = nullptr;
    m_audioOutput = nullptr;
    m_udpSocket = nullptr;

    ui->audioBufferProgress->setValue(0);
    ui->hostAddress->setEnabled(true);
    ui->hostPort->setEnabled(true);
    ui->talkButton->setEnabled(true);
    ui->endButton->setEnabled(false);
}

void MainWindow::on_messageParser_messageReceived(message_t *message)
{
    logEventMessage(QString("Message received: ID=%1, size=%2").arg(message->id).arg(message->size));
    switch (message->id) {
    case MESSAGE_CONFIGURATION_PAYLOAD_ID: {
        logEventMessage("Processing configuration payload...");
        configuration_payload_message_t payload;
        configuration_payload_decode(message, &payload);
        ui->configDisplayName->setText(QString::fromLocal8Bit(payload.display_name));
        ui->configDescription->setText(QString::fromLocal8Bit(payload.description));
        break;
    }
    case MESSAGE_INTERCOM_DIRECTORY_LISTING_ID: {
        logEventMessage("Processing intercom directory listing");
        intercom_directory_listing_message_t listing;
        intercom_directory_listing_decode(message, &listing);
        ui->contactList->clear();
        for (int i = 0; i < listing.num_entries; i++) {
            QListWidgetItem *itm = new QListWidgetItem();

            QByteArray hwid(listing.entries[i].hwid, 6);
            itm->setText(QString::fromLocal8Bit(listing.entries[i].display_name));
            itm->setData(Qt::UserRole, hwid);
            ui->contactList->addItem(itm);
        }
        break;
    }
    }
    free(message);
}

void MainWindow::sendMessage(message_t *message)
{
    logEventMessage(QString("Sending message ID=%1, size=%2").arg(message->id).arg(message->size));
    size_t size = message->size + MESSAGE_HEADER_SIZE + 2;
    QStringList hwid_parts = ui->hwidEdit->text().split(":");
    bool bstatus = false;

    message->hwid[0] = hwid_parts.at(0).toUInt(&bstatus, 16);
    message->hwid[1] = hwid_parts.at(1).toUInt(&bstatus, 16);
    message->hwid[2] = hwid_parts.at(2).toUInt(&bstatus, 16);
    message->hwid[3] = hwid_parts.at(3).toUInt(&bstatus, 16);
    message->hwid[4] = hwid_parts.at(4).toUInt(&bstatus, 16);
    message->hwid[5] = hwid_parts.at(5).toUInt(&bstatus, 16);
    message->timestamp = 0;

    QByteArray bytes;
    bytes.append("AE");
    bytes.append((const char *)message, size);
    m_controlSocket->write(bytes);
}

void MainWindow::logEventMessage(const QString &message)
{
    ui->logText->appendPlainText(message);
    statusBar()->showMessage(message);
}

void MainWindow::openIntercomChannel(const QString &address, const int port)
{
    logEventMessage("Starting audio stream...");
    QHostAddress addr(address);

    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->bind(QHostAddress::AnyIPv4, port);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::on_udpSocket_readyRead);

    ui->hostAddress->setEnabled(false);
    ui->hostPort->setEnabled(false);
    ui->talkButton->setEnabled(false);
    ui->endButton->setEnabled(true);

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

    qDebug() << "Output:";
    qDebug() << "Buffer Size:" << m_audioOutput->bufferSize();
    qDebug() << "Notify Interval:" << m_audioOutput->notifyInterval();

    qDebug() << "Input:";
    qDebug() << "Buffer Size:" << m_audioInput->bufferSize();
    qDebug() << "Notify Interval:" << m_audioInput->notifyInterval();
    ui->audioBufferProgress->setMaximum(m_audioInput->bufferSize());
    ui->audioBufferProgress->setValue(0);

    logEventMessage("Starting audio input/output...");
    m_audioInputDevice = m_audioInput->start();
    m_audioOutputDevice = m_audioOutput->start();
}

void MainWindow::on_certBrowseButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Select CA Certificate"),
                QString(QStandardPaths::HomeLocation),
                tr("PEM Files (*.pem)"));
    ui->cacertPath->setText(filename);
}
