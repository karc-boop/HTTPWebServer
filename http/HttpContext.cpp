#idef USER_RING_BUFFER
#include "HTTP/tcp/RingBuffer.h"
#else
#include "HTTP/tcp/Buffer.h"
#endif

#include "HttpContext.h"
#include <algorithm> // standared library of built-in functions including searching, sorting, and transforming elements

using namespace kback;

const char HttpContext::CRLF[] = "\r\n"; // CRLF is a sequence of two characters: carriage return and line feed

bool HttpContext::processRequestLine(const char *begin, const char *end) {
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' '); // find the first space
    HttpRequestParseState state = expectRequestLine;
    if (space != end && request_.setMethod(start, space)) {
        start = space + 1;
        space = std::find(start, end, ' '); // find the second space
        if (space != end) {
            const char *question = std::find(start, space, '?'); // find the question mark
            if (question != space) {
                request_.setPath(start, question);
                request_.setQuery(question, space);
            } else {
                request_.setPath(start, space);
            }
            start = space + 1;
            succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1."); // check if the version is HTTP/1.x
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
        state = expectHeaders;
    }
    return succeed;
}

#ifdef USE_RINGBUFFER
bool HttpContext::parseRequest(Buffer *buf, TimeStamp receiveTime) {
    string str_buf = buf->retrieveAllAsString();
    bool ok = true;
    bool hasMore = true;

    // parse request line by three parts: method, path, version
    while (hasMore) {
        int start = 0

        // if the state is ExpectRequestLine, parse the request line
        if (state_ == ExpectRequestLine) {
            const char *crlf = std :: search(str_buf.data() + start, str_buf.data() + str_buf.size(), CRLF, CRLF + 2);

            // if the request line is complete, process it
            if (crlf) {
                ok = processRequestLine(str_buf.data() + start, crlf);

                // if the request line is valid, set the receive time and move to the next state
                if (ok) {
                    request_.setReceiveTime(receiveTime);
                    
                    start = crlf - str_buf.data() + 2;
                    state_ = ExpectHeaders;
                } else {
                    hasMore = false;
                }
            } else {
                hasMore = false;
            }
        // if the state is expectheaders, parse the header
        } else if (state_ == ExpectHeaders) {
            const char *crlf = std :: search(str_buf.data() + start, str_buf.data() + str_buf.size(), CRLF, CRLF + 2);
            if (crlf) {
                const char *colon = std::find(str_buf.data() + start, crlf, ':');
                if (colon != crlf) {
                    request_.addHeader(str_buf.data() + start, colon, crlf);
                } else {
                    // empty line, end of header
                    state_ = GotAll;
                    hasMore = false;
                }
                start = crlf - str_buf.data() + 2;
            } else {
                hasMore = false;
            }
        } else if (state_ == ExpectBody) {
            // TODO: parse body
        }
    }
    return ok;
}

#else

bool HttpContext::parseRequest(Buffer *buf, TimeStamp receiveTime){
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
                    // empty line, end of header
                    state_ = gotAll;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            } else {
                hasMore = false;
            }
        } else if (state_ == expectBody) {
            // TODO: parse body
        }
    }
    return ok;
}
#endif