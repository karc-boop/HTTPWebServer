#ifdef USE_RINGBUFFER
#else
#pragma once
#include "Copyable.h"

#include <algorithm>
#include <vector>
#include <string>


namespace kback {
    class Buffer : public kback::Copyable {
    public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    // Constructor intializing buffer and indices
    Buffer()
        : buffer_(kCheapPrepend + kInitialSize),
          readerIndex_(kCheapPrepend),
          writerIndex_(kCheapPrepend) {
        assert(readableBytes() == 0);
        assert(writableBytes() == kInitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    // swaps function for swapping buffer and indices with another buffer
    void swap(Buffer &rhs) {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerIndex_, rhs.writerIndex_);
    }

    // getter method of readbableBytes
    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }

    // getter method of writableBytes
    size_t writableBytes() const {
        return buffer_.size() - writerIndex_;
    }

    // getter method of prependableBytes
    size_t prependableBytes() const {
        return readerIndex_;
    }   

    // getter method of a pointer to the first readable byte
    const char *peek() const {
        return begin() + readerIndex_;
    }

    // function to find the first occurence of CRLF in the buffer
    const char *findCRLF() const {
        const char *crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    // retrives data from the buffer
    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if (len < readableBytes()) {
            readerIndex_ += len;
        } else {
            retrieveAll();
        }
    }

    // function to retrieve all data from the buffer
    void retrieveAll() {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    // function to retrieve data until a certain point
    void retriveUnitl(const char *end) {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    // function to retrieve all data as a string
    std::string retrieveAllAsString() {
        std::string str(peek(), readableBytes());
        retrieveAll();
        return retrieveAsString(readableBytes());
    }

    // function to append data to the buffer
    void append(const std::string &str) {
        append(str.data(), str.length());
    }

    // purpose of this function:
    // 1. ensure that there is enough space in the buffer to write data
    // 2. copy data to the buffer
    // 3. update the writerIndex
    void append(const char *data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    void append(const void *data, size_t len) {
        append(static_cast<const char *>(data), len);
    }

    // when the rest space is not enough, make space for cache
    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }

        assert(writableBytes() >= len);
    }

    char *beginWrite() {
        return begin() + writerIndex_;
    }

    const char *beginWrite() const {
        return begin() + writerIndex_;
    }

    void hasWritten(size_t len) {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    void prepend(const void *data, size_t len) {
        assert(len <= prependableBytes());
        readerIndex_ -= len;
        const char *d = static_cast<const char *>(data);
        std::copy(d, d + len, begin() + readerIndex_);
    }

    void shrink (size_t reserve) {
        Buffer other;
        other.ensureWritableBytes(readableBytes() + reserve);
        other.append(peek(), readableBytes());
        swap(other);
    }

    ssize_t readFd(int fd, int *savedErrno);

    ssize_t readFdET(int fd, int *savedErrno);

    ssize_t writeFd(int fd, int *savedErrno);

    ssize_t writeFdET(int fd, int *savedErrno);

    private:
        char *begin() {
            return &*buffer_.begin();
        }

        const char *begin() const {
            return &*buffer_.begin();
        }

        void makeSpace(size_t len) {
            if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
                buffer_.resize(writerIndex_ + len);
            } else {
                assert(kCheapPrepend < readerIndex_);
                size_t readable = readableBytes();
                std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
                readerIndex_ = kCheapPrepend;
                writerIndex_ = readerIndex_ + readable;
                assert(readable == readableBytes());
            }
        }

        private:
            std::vector<char> buffer_;
            size_t readerIndex_;
            size_t writerIndex_;

            static const char kCRLF[];
            
    };

}

#endif
