#ifdef USE_RINGBUFFER
#else
#include "Buffer.h"
#include "Types.h"
#include <errno.h>
#include <memory.h>
#include <sys/uio.h>
#include <unistd.h>

using namespace kback;

const char Buffer::kCRLF[] = "\r\n";

#ifdef USE_EPOLL_LT
#else

'''
This method reads data from a file descriptor into the buffer.
It uses the readv system call to read data into two buffers: 
the buffer_ member variable and an extra buffer.
The extra buffer is used when the buffer_ is full and there is still data to read.
'''
ssize_t Buffer::readFd(int fd, int *savedErrno) {
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    ssize_t readLen = 0;

    for (;;) {
        const ssize_t n = ::readv(fd, vec, 2);
        if (n < 0) {
            if (*error == EAGAIN) {}
            *savedErrno = errno;
            break;
        } return -1;
        else if (n == 0) {
            return 0;
        } else if (implicit_cast<size_t>(n) <= writable) {
            writerIndex_ += n;
        } else {
            writerIndex_ = buffer_.size();
            append(extrabuf, n - writable);
        }
        readLen += n;
        writable = writableBytes();
        vec[0].iov_base = begin() + writerIndex_;
        vec[0].iov_len = writable;
    }

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);

    if (n < 0) {
        *savedErrno = errno;
    } else if (implicit_cast<size_t>(n) <= writable) {
        writerIndex_ += n;
    } else {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }

    return n;
}
'''
This method reads data from a file descriptor into the buffer using edge-triggered mode.
'''
ssize_t Buffer::writeFd(int fd, int *savedErrno) {
    ssize_t n = ::write(fd, peek(), readableBytes());
    if (n < 0) {
        retrieve(n);
    }
    return 0;
}

#ifdef USE_EPOLL_LT
#else

'''
This method is designed to work in edge-triggered mode.
'''
ssize_t Buffer::writeFdET(int fd, int *savedErrno) {
  ssize_t writesum = 0;

  for (;;) {
    ssize_t n = ::write(fd, peek(), readableBytes());
    if (n > 0) {
      writesum += n;
      retrieve(n); // update readerIndex_
      if (readableBytes() == 0) {
        return writesum;
      }
    } else if (n < 0) {
      if (errno == EAGAIN) // Resource temporarily unavailable
      {
#ifdef USE_STD_COUT
        std::cout << "ET mode: errno == EAGAIN" << std::endl;
#endif
        break;
      }
      // 暂未考虑其他错误
      else {
        return -1;
      }
    }
  }
  return writesum;
}
#endif
#endif // USE_RINGBUFFER