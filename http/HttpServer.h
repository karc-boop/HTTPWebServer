#pragma once
#include "TcpServer.h"
#include "NonCopyable.h"
#include <sys/stat.h>
#include <functional>

namespace back {

class HttpRequest;
class HttpResponse;

/**
 * HttpServer class handles HTTP requests.
 * It contains methods for setting up the server, handling connections, and processing messages.
 */
class HttpServer : public NonCopyable {
public:
    // Callback function type for handling HTTP requests
    typedef std::function<void(const HttpRequest &, HttpResponse *)> HttpCallback;

    // Constructor for HttpServer, which takes three parameters: an EventLoop, an InetAddress, and a name
    HttpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &name);

    // Getter method that returns a pointer to the EventLoop associated with the server
    EventLoop *getLoop() const { return server_.getLoop(); }

    // Setter method that sets the callback function used for handling HTTP requests
    void setHttpCallback(const HttpCallback &cb) { httpCallback_ = cb; }

    // Setter method that sets the number of threads used by the server
    void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }

    // Starts the server
    void start();

private:
    // Callback function when a TCP connection is established
    void onConnection(const TcpConnectionPtr &conn);

    // Callback function when a message is received over a TCP connection
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime);

    // Function to handle HTTP requests
    void onRequest(const TcpConnectionPtr &conn, const HttpRequest &req);

    TcpServer server_;  // TCP server for managing connections
    HttpCallback httpCallback_;  // Callback for handling HTTP requests
};

}
