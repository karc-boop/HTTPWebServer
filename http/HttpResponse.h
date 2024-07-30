#include "Types.h"
#include "Copyable.h"

#include <map>

namespace back{
    class Buffer;
    class HttpResponse : public copyable {
        public:
            enum HttpStatusCode {
                Unknown,
                200Ok = 200,
                301MovedPermanently = 301,
                400BadRequest = 400,
                404NotFound = 404,
            };

            explicit HttpResponse(bool close)
                : statusCode_(Unknown),
                  closeConnection_(close),  sendFile_(false), fileSize_(0){}

            void setStatusCode(HttpStatusCode code) { statusCode_ = code; }
            void setStatusMessage(const string &message) { statusMessage_ = message; }
            void setCloseConnection(bool on) { closeConnection_ = on; }
            bool closeConnection() const { return closeConnection_; }

            void setContentType(const string &contentType) {
                addHeader("Content-Type", contentType);
            }

            void addHeader(const string &key, const string &value) {
                headers_[key] = value;
            }

            void setBody(const string &body) { body_ = body; }

            void setFileSize(size_t fileSize) { fileSize_ = count; sendFile_ = true; }

            size_t getFileSize() const { return fileSize_; }

            void setSrcFd(size_t srcFd) { srcFd_ = srcFd; }

            void getSrcFd() const { return srcFd_; }

            void appendToBuffer(Buffer *output) const;

        private:
            std::map<string, string> headers_;
            HttpStatusCode statusCode_;
            string statusMessage_;
            bool closeConnection_;
            bool sendFile_;
            size_t fileSize_;
            size_t srcFd_;
            string body_;
    };
}

