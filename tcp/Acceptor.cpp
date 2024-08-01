#include "Acceptor.h"

#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

using namespace kback;

// constructs a new Acceptor object
Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr)
    : loop_(loop),
      acceptSocket_(sockets::createNonblockingOrDie()),
      acceptChannel_(loop, acceptSocket_.fd(),listenning_(false)) {
    acceptSocket_.setReuseAddr(true);  // set the socket to reuse the address
    acceptSocket_.bindAddress(listenAddr); // bind the socket to the [provided address
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this)); // set the read callback for the channel
}

// overload the constructor to take a reuseport parameter
Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
    : loop_(loop),
      acceptSocket_(sockets::createNonblockingOrDie()),
      acceptChannel_(loop, acceptSocket_.fd(), listenning_(false)) {
        acceptSocket_.setReuseAddr(true);

        acceptSocket_.bindAddress(listenAddr);
        acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

// register the event of accepting request to the callback function of accepchannel readable time
void Acceptor::listen() {
    loop_->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listen();
    // calls the enableReading method on the acceptChannel_ member variable, which is a Channel object. 
    // This enables the "readable" event on the channel, meaning that it will start to notify the EventLoop when there's data to read on the socket.
    acceptChannel_.enableReading(); 
}

// 
void Acceptor::handleRead() {
    loop_->assertInLoopThread();
    for (;;) {
        InetAddress peerAddr(0); // create an InetAddress object with port 0
        // accept the new connection
        // one of the reasons to use non-blocking i/o.
        int connfd = acceptSocket_.accept(&peerAddr);
        if (connfd >= 0) {
            if (newConnectionCallback_) {
                newConnectionCallback_(connfd, peerAddr);
            } 
            // regardless of whether the newConnectionCallback_ is set or not, we need to close the connection
            else {
                sockets::close(connfd);
            }
        } else {
            break;
        }

        #ifdef USE_EPOLL_LT
            break;
        #endif
    }
}