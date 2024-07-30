#pragma once



// The class is for handeling http content parsing

namespace kback{
    class Buffer;
    
    class HttpContext : public copyable {
    public:
        // enum for parsing state
        enum HttpRequestParseState {
            expectRequestLine,
            expectHeaders,
            expectBody,
            gotAll,
        };
        HttpContext() : state_(expectRequestLine) {}
        
        bool parseRequest(Buffer *buf, TimeStamp receiveTime);

        // reset the state
        bool gotAll() const { return state_ == gotAll; }

        void reset() {
            state_ = expectRequestLine;
            HttpRequest dummy;
            request_.swap(dummy);
        }

        const HttpRequest &request() const { return request_; }

        HttpRequest &request() { return request_; }

    private:
        bool processRequestLine(const char *begin, const char *end);

        HttpRequestParseState state_;
        HttpRequest request_;

        static const char CRLF[];
    };

}