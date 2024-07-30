 #include "HttpResponse.h"
#include <stdio.h>

#ifdef USE_RINGBUFFER
#include "RingBuffer.h"
#else
#include "Buffer.h"
#endif

using namespace back;

/**
 * This function appends the HTTP response to the output buffer.
 * 
 * @param output Pointer to the output buffer.
 * 
 * The function first formats the HTTP status line (version, status code, status message) and appends it to the buffer.
 * Then it checks if the connection should be closed after the response is sent.
 * If so, it appends the "Connection: close" header.
 * If not, it appends the "Content-Length" header with the size of the body, and the "Connection: Keep-Alive" header.
 * 
 * Then it appends all the headers in the headers_ map.
 * 
 * After all headers have been appended, it appends an empty line to indicate the end of the headers.
 * 
 * Finally, if the response includes a file, it appends the file to the buffer.
 * If not, it appends the body of the response.
 */
void HttpResponse::appendToBuffer(Buffer *output) const {
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode_);
    output->append(buf);
    output->append(statusMessage_);
    output->append("\r\n");

    if (closeConnection_) {
        output->append("Connection: close\r\n");
    } else {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
        output->append(buf);
        output->append("Connection: Keep-Alive\r\n");
    }

    for (const auto &header : headers_) {
        output->append(header.first);
        output->append(": ");
        output->append(header.second);
        output->append("\r\n");
    }

    output->append("\r\n");
    if (sendFile_) {
        output->appendFile(srcFd_, fileSize_);
    } else {
        output->append(body_);
    }
}