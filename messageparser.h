#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <QObject>
#include "message.h"

class MessageParser : public QObject
{
    Q_OBJECT
public:
    explicit MessageParser(QObject *parent = nullptr);
    ~MessageParser();
    int processBytes(QByteArray &data);
    int processBytes(unsigned char *data, size_t len);

protected:
    bool in_header;
    bool in_body;
    bool got_header;
    int byte_count;
    int bytes_expected;
    int error_count;
    int rx_count;
    message_t *buffer;

signals:
    void messageReceived(message_t *message);
};

#endif // MESSAGEPARSER_H
