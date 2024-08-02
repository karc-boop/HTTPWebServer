#pragma once

#include "NonCopyable.h"
#include "KChannel.h"

#include <functional>
#include "Socket.h"

namespace kback {
    class EventLoop;
    class InetAddress

    class Acceptor : public NonCopyable {
    
    public:
        // defines a callback function that is called when a new connection is established
        typedef std::function<void(int sockfd, const InetAddress &)> NewConnectionCallback;

        Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
        Acceptor(EventLoop *loop, const InetAddress &listenAddr);

        // setter for a new connection callback
        void setNewConnectionCallback(const NewConnectionCallback &cb) {
            newConnectionCallback_ = cb;
        }

        bool listenning() const { return listenning_; }

        // starts listening for new connections
        void listen();
    private:
        void handleRead();

        EventLoop *loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        bool listenning_;
    };
}