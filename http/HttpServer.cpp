#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "Icons.h"
#include "Callbacks.h"

#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

extern char favicon[555];

using namespace back;

// default callback function for handling HTTP requests
void defaultHttpCallBack(const HttpRequest &req, HttpResponse *resp) {

    // first path is the root path
    if (req.path() == "/") {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/html");
        resp->addHeader("Server", "back");
        string now = Timestamp::now().toFormattedString();
        resp->setBody("<html><head><title>This is title</title></head>"
                      "<body><h1>Hello</h1>Now is " + now +
                      "</body></html>");
    
    // second path is the favicon.ico
    } else if (req.path() == "/favicon.ico") {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("image/png");
        resp->setBody(string(favicon, sizeof favicon));
    
    // third path is the hello path
    } else if (req.path() == "/hello") {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/plain");
        resp->addHeader("Server", "back");
        resp->setBody("hello, world!\n");

    // fourth path is the file path
    } else if (req.path() == "/file") {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/html");
        resp->addHeader("Server", "back");
    
    // fifth path is the good path
    } else if (req.path() == "/good") {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/plain");
        resp->addHeader("Server", "back");
        string AAA(950, '!');
        AAA += "\n";
        resp->setBody(AAA);
    
    // sixth path is the bad undefined path
    } else {
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }
}