#include "messageparser.h"
#include <string.h>
#include <stdint.h>

MessageParser::MessageParser(QObject *parent) : QObject(parent)
{
    this->in_header = 0;
    this->in_body = 0;
    this->got_header = 0;
    this->byte_count = 0;
    this->bytes_expected = 0;

    this->buffer = (message_t *)malloc(sizeof(message_t));
    memset((void *)this->buffer, 0, sizeof(message_t));
    this->error_count = 0;
}

MessageParser::~MessageParser()
{
    free(this->buffer);
}

int MessageParser::processBytes(QByteArray &data)
{
    return processBytes((unsigned char *)data.data(), data.size());
}

int MessageParser::processBytes(unsigned char *data, size_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++) {
      if (this->in_body == 1) {
        *((unsigned char *) this->buffer + (this->byte_count++)) = data[i];

        //
        // once we know the size of the message, we can
        // adjust what we are expecting (save traffic)
        //
        if (this->got_header == 0 && this->byte_count == MESSAGE_HEADER_SIZE) {
          if (this->buffer->size > MESSAGE_MAX_TOTAL_SIZE) {
            this->in_header = 0;
            this->in_body = 0;
            this->got_header = 0;
            this->byte_count = 0;
            this->bytes_expected = 0;
            this->buffer->id = 0;
            this->buffer->size = 0;
            this->error_count++;

            continue;
          }

          this->bytes_expected = this->buffer->size;
          this->got_header = 1;
        }

        if (this->byte_count == this->bytes_expected) {
          message_t *msg = (message_t *) malloc(
              this->buffer->size);
          memcpy((void *) msg, (void *) this->buffer, this->buffer->size);

          emit messageReceived(msg);

          this->rx_count++;
          this->in_header = 0;
          this->got_header = 0;
          this->in_body = 0;
          this->byte_count = 0;
          this->bytes_expected = 0;
          this->buffer->id = 0;
          this->buffer->size = 0;
        }
      }
      else if (this->in_header == 0) {
        if (data[i] == 'A') {
          this->in_header = 1;
        }
      }
      else if (this->in_header == 1) {
        if (data[i] == 'E') {
          this->in_body = 1;
          this->in_header = 0;
          this->byte_count = 0;
          this->bytes_expected = MESSAGE_MAX_TOTAL_SIZE;
        }
        else {
          this->in_header = 0;
          this->got_header = 0;
          this->in_body = 0;
          this->byte_count = 0;
          this->bytes_expected = 0;
          this->buffer->id = 0;
          this->buffer->size = 0;
          this->error_count++;
        }
      }
    }

    return 0;
}
