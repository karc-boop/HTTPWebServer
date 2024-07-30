#pragma once
#include "Timestamp.h"
#include "Copyable.h"
#include "Types.h"

#include <assert.h>
#include <stdio.h>
#include <map>

namespace kback {
    class Buffer;

    /**
     * HttpContext class represents an HTTP request.
     * It contains methods for setting and getting the HTTP method, version, path, query, headers, and receive time.
     */
    class HttpContext : public copyable {
    public:
        // Enum for HTTP methods
        enum Method {
            Invalid,
            Get,
            Post,
            Head,
            Put,
            Delete,
        };

        // Enum for HTTP versions
        enum Version {
            Unknown,
            HTTP10,
            HTTP11,
        };

        // Default constructor
        HttpRequest() : method_(Invalid), version_(Unknown) {}

        // Set the HTTP version
        void setVersion(Version v) { version_ = v; }

        // Get the HTTP version
        Version getVersion() const { return version_; }

        // Set the HTTP method
        bool setMethod(const char *start, const char *end);

        // Get the HTTP method
        Method method() const { return method_; }

        // Get the string representation of the HTTP method
        const char *methodString() const;

        // Set the path of the HTTP request
        void setPath(const char *start, const char *end) { path_.assign(start, end); }

        // Get the path of the HTTP request
        const string &path() const { return path_; }

        // Set the query of the HTTP request
        void setQuery(const char *start, const char *end) { query_.assign(start, end); }

        // Get the query of the HTTP request
        const string &query() const { return query_; }

        // Add a header to the HTTP request
        void addHeader(const char *start, const char *colon, const char *end);

        // Set the receive time of the HTTP request
        void setReceiveTime(TimeStamp t) { receiveTime_ = t; }

        // Get the receive time of the HTTP request
        TimeStamp receiveTime() const { return receiveTime_; }

        // Get a specific header from the HTTP request
        const string &getHeader(const string &field) const;

        // Get all headers from the HTTP request
        const std::map<string, string> &headers() const { return headers_; }

        // Swap the contents of this HTTP request with another
        void swap(HttpRequest &that);

    private:
        Method method_;
        string path_;
        string query_;
        Version version_;
        std::map<string, string> headers_;
        TimeStamp receiveTime_;
    };

    /**
     * This function sets the HTTP method for the request.
     * 
     * @param start Pointer to the start of the string representing the HTTP method.
     * @param end Pointer to the end of the string representing the HTTP method.
     * 
     * The function first asserts that the current method is Invalid. 
     * Then it creates a string from the input range and checks if it matches 
     * one of the supported HTTP methods ("GET", "POST", "HEAD", "PUT", "DELETE").
     * If a match is found, it sets the method_ member variable to the corresponding enum value.
     * If no match is found, it sets the method_ to Invalid.
     * 
     * @return Returns true if a valid method was set, false otherwise.
     */
    bool HttpContext::setMethod(const char *start, const char *end) {
        // ...
    }

    /**
     * This function returns the string representation of the current HTTP method.
     * 
     * It initializes a const char pointer 'result' with the value "UNKNOWN".
     * Then it checks the value of the member variable 'method_' in a switch statement.
     * If 'method_' matches one of the known HTTP methods (Get, Post, Head, Put, Delete),
     * 'result' is set to the corresponding string ("GET", "POST", "HEAD", "PUT", "DELETE").
     * If 'method_' does not match any known method, 'result' remains "UNKNOWN".
     * 
     * @return Returns a pointer to a string representing the current HTTP method.
     */
    const char *HttpContext::methodString() const {
        // ...
    }

    /**
     * This function adds a header to the HTTP request.
     * 
     * @param start Pointer to the start of the header field.
     * @param colon Pointer to the colon separating the header field and value.
     * @param end Pointer to the end of the header value.
     * 
     * The function creates a string for the field and value from the input range,
     * trims any leading and trailing whitespace from the value,
     * and adds the field and value to the headers_ map.
     */
    void HttpContext::addHeader(const char *start, const char *colon, const char *end) {
        // ...
    }

    /**
     * This function gets a specific header from the HTTP request.
     * 
     * @param field The header field to get.
     * 
     * The function searches the headers_ map for the field.
     * If the field is found, it returns the corresponding value.
     * If the field is not found, it returns an empty string.
     * 
     * @return Returns the value of the header field, or an empty string if the field is not found.
     */
    const string &HttpContext::getHeader(const string &field) const {
        // ...
    }

    /**
     * This function swaps the contents of this HTTP request with another.
     * 
     * @param that The other HTTP request to swap with.
     * 
     * The function swaps the method, path, query, receive time, and headers with the other HTTP request.
     */
    void HttpContext::swap(HttpRequest &that) {
        // ...
    }
}