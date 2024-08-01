#include <map>
#include <string>
#include "Types.h"
#include "Copyable.h"

namespace back {
    class Buffer;

    class HttpResponse : public Copyable {
    public:
        enum HttpStatusCode {
            Unknown,
            k200Ok = 200,
            k301MovedPermanently = 301,
            k400BadRequest = 400,
            k404NotFound = 404,
        };

        explicit HttpResponse(bool close)
            : statusCode_(Unknown),
              closeConnection_(close),
              sendFile_(false),
              fileSize_(0),
              srcFd_(0) {}

        void setStatusCode(HttpStatusCode code) { statusCode_ = code; }
        void setStatusMessage(const std::string &message) { statusMessage_ = message; }
        void setCloseConnection(bool on) { closeConnection_ = on; }
        bool closeConnection() const { return closeConnection_; }

        void setContentType(const std::string &contentType) {
            addHeader("Content-Type", contentType);
        }

        void addHeader(const std::string &key, const std::string &value) {
            headers_[key] = value;
        }

        void setBody(const std::string &body) { body_ = body; }

        void setFileSize(size_t fileSize) { fileSize_ = fileSize; sendFile_ = true; }

        size_t getFileSize() const { return fileSize_; }

        void setSrcFd(size_t srcFd) { srcFd_ = srcFd; }

        size_t getSrcFd() const { return srcFd_; }

        void appendToBuffer(Buffer *output) const;

    private:
        std::map<std::string, std::string> headers_;
        HttpStatusCode statusCode_;
        std::string statusMessage_;
        bool closeConnection_;
        bool sendFile_;
        size_t fileSize_;
        size_t srcFd_;
        std::string body_;
    };
}
