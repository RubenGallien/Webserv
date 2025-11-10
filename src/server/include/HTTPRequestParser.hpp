#ifndef HTTPREQUESTPARSER
#define HTTPREQUESTPARSER

#include <locale>
#include <exception>
#include <string>

#include "HTTPRequest.hpp"

class HTTPRequestParser
{

    private:
        HTTPRequestParser();
        HTTPRequestParser(const HTTPRequestParser& src);
        HTTPRequestParser& operator=(const HTTPRequestParser& other);
        ~HTTPRequestParser();
    public:
        static void fillRequest(HTTPRequest& request, std::string& buffer);
        static void checkIfComplete(HTTPRequest& request, std::string& buffer);
        class requestException: public std::exception
        {
            virtual const char* what() const throw()
            {
                return "My exception request happened";
            }
        };

        class methodException: public std::exception
        {
            virtual const char* what() const throw()
            {
                return "method";
            }
        };

        class versionException: public std::exception
        {
            virtual const char* what() const throw()
            {
                return "version";
            }
        };

        class payloadException: public std::exception
        {
            virtual const char* what() const throw()
            {
                return "payload";
            }
        };


};


#endif
