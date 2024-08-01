#pragma once

#include "HttpRequest.h"
#include "Buffer.h"
#include "Timestamp.h"
#include "Copyable.h"

// Namespace for back-end components
namespace kback {
    class Buffer;

    // The class is for handling HTTP content parsing
    class HttpContext : public Copyable {
    public:
        // Enum for parsing state
        enum HttpRequestParseState {
            expectRequestLine,  // Expecting the request line (method, path, version)
            expectHeaders,      // Expecting the headers
            expectBody,         // Expecting the body
            gotAll,             // Parsing complete
        };

        // Constructor initializes the state to expectRequestLine
        HttpContext() : state_(expectRequestLine) {}

        // Parses the HTTP request from the buffer
        bool parseRequest(Buffer *buf, Timestamp receiveTime);

        // Checks if the parsing state is complete
        bool gotAll() const { return state_ == gotAll; }

        // Resets the context to initial state
        void reset() {
            state_ = expectRequestLine;
            HttpRequest dummy;
            request_.swap(dummy);
        }

        // Returns a const reference to the parsed request
        const HttpRequest &request() const { return request_; }

        // Returns a reference to the parsed request
        HttpRequest &request() { return request_; }

    private:
        // Processes the request line (method, path, version)
        bool processRequestLine(const char *begin, const char *end);

        HttpRequestParseState state_;  // Current state of the parser
        HttpRequest request_;          // The HTTP request being parsed

        static const char CRLF[];      // CRLF sequence ("\r\n")
    };

}
