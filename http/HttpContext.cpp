#include "HttpContext.h"
#include <algorithm> // Standard library of built-in functions including searching, sorting, and transforming elements

using namespace kback;

const char HttpContext::CRLF[] = "\r\n"; // CRLF is a sequence of two characters: carriage return and line feed

// Function to process the request line (method, path, version)
bool HttpContext::processRequestLine(const char *begin, const char *end) {
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' '); // Find the first space

    // Check if the method is valid
    if (space != end && request_.setMethod(start, space)) {
        start = space + 1;
        space = std::find(start, end, ' '); // Find the second space
        if (space != end) {
            const char *question = std::find(start, space, '?'); // Find the question mark
            if (question != space) {
                request_.setPath(start, question);
                request_.setQuery(question, space);
            } else {
                request_.setPath(start, space);
            }
            start = space + 1;
            succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1."); // Check if the version is HTTP/1.x
            if (succeed) {
                if (*(end - 1) == '1') {
                    request_.setVersion(HttpRequest::HTTP11);
                } else if (*(end - 1) == '0') {
                    request_.setVersion(HttpRequest::HTTP10);
                } else {
                    succeed = false;
                }
            }
        }
        state_ = expectHeaders; // Update state to expect headers
    }
    return succeed;
}

#ifdef USE_RINGBUFFER
// Function to parse the HTTP request using a ring buffer
bool HttpContext::parseRequest(Buffer *buf, Timestamp receiveTime) {
    std::string str_buf = buf->retrieveAllAsString();
    bool ok = true;
    bool hasMore = true;

    // Parse request line by three parts: method, path, version
    while (hasMore) {
        int start = 0;

        // If the state is ExpectRequestLine, parse the request line
        if (state_ == expectRequestLine) {
            const char *crlf = std::search(str_buf.data() + start, str_buf.data() + str_buf.size(), CRLF, CRLF + 2);

            // If the request line is complete, process it
            if (crlf) {
                ok = processRequestLine(str_buf.data() + start, crlf);

                // If the request line is valid, set the receive time and move to the next state
                if (ok) {
                    request_.setReceiveTime(receiveTime);
                    start = crlf - str_buf.data() + 2;
                    state_ = expectHeaders;
                } else {
                    hasMore = false;
                }
            } else {
                hasMore = false;
            }
        // If the state is ExpectHeaders, parse the header
        } else if (state_ == expectHeaders) {
            const char *crlf = std::search(str_buf.data() + start, str_buf.data() + str_buf.size(), CRLF, CRLF + 2);
            if (crlf) {
                const char *colon = std::find(str_buf.data() + start, crlf, ':');
                if (colon != crlf) {
                    request_.addHeader(str_buf.data() + start, colon, crlf);
                } else {
                    // Empty line, end of header
                    state_ = gotAll;
                    hasMore = false;
                }
                start = crlf - str_buf.data() + 2;
            } else {
                hasMore = false;
            }
        } else if (state_ == expectBody) {
            // TODO: Parse body
        }
    }
    return ok;
}

#else
// Function to parse the HTTP request using a standard buffer
bool HttpContext::parseRequest(Buffer *buf, Timestamp receiveTime) {
    bool ok = true;
    bool hasMore = true;
    while (hasMore) {
        if (state_ == expectRequestLine) {
            const char *crlf = buf->findCRLF();
            if (crlf) {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok) {
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    state_ = expectHeaders;
                } else {
                    hasMore = false;
                }
            } else {
                hasMore = false;
            }
        } else if (state_ == expectHeaders) {
            const char *crlf = buf->findCRLF();
            if (crlf) {
                const char *colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf) {
                    request_.addHeader(buf->peek(), colon, crlf);
                } else {
                    // Empty line, end of header
                    state_ = gotAll;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            } else {
                hasMore = false;
            }
        } else if (state_ == expectBody) {
            // TODO: Parse body
        }
    }
    return ok;
}
#endif
