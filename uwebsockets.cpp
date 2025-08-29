#include <iostream>
#include <phpcpp.h>
#include <App.h>

class HttpRequest : public Php::Base {
private:
    uWS::HttpRequest* _req;

public:
    HttpRequest(uWS::HttpRequest* req)
        : _req(req)
    {
    }
    virtual ~HttpRequest() = default;

    Php::Value getUrl()
    {
        return std::string(_req->getUrl().data(), _req->getUrl().size());
    }

    Php::Value getMethod()
    {
        return std::string(_req->getMethod().data(), _req->getMethod().size());
    }

    Php::Value getQuery()
    {
        return std::string(_req->getQuery().data(), _req->getQuery().size());
    }

    Php::Value getParameter(Php::Parameters& params)
    {
        std::string name = params[0];
        auto parameter = _req->getParameter(name);

        return std::string(parameter.data(), parameter.size());
    }

    Php::Value getHeader(Php::Parameters& params)
    {
        std::string name = params[0];
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        auto header = _req->getHeader(name);

        return std::string(header.data(), header.size());
    }
};

class HttpResponse : public Php::Base {
private:
    uWS::HttpResponse<false>* _res;
    bool _ended = false;

public:
    HttpResponse(uWS::HttpResponse<false>* res)
        : _res(res)
    {
    }
    virtual ~HttpResponse() = default;

    void write(Php::Parameters& params)
    {
        std::string chunk = params[0];
        _res->write(chunk);
    }

    void setStatus(Php::Parameters& params)
    {
        std::string status = params[0];
        _res->writeStatus(status);
    }

    void setHeader(Php::Parameters& params)
    {
        std::string key = params[0];
        std::string value = params[1];
        _res->writeHeader(key, value);
    }
};

class UServer : public Php::Base {
private:
    uWS::App _app;

public:
    UServer() = default;
    virtual ~UServer() = default;

    void __construct(Php::Parameters& params)
    {
    }

    Php::Value get(Php::Parameters& params)
    {
        std::string path = params[0];
        Php::Value handler = params[1];

        _app.get(path, [handler](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
            Php::Value resObj = Php::Object("HttpResponse", new HttpResponse(res));
            Php::Value reqObj = Php::Object("HttpRequest", new HttpRequest(req));

            Php::Value result = handler(reqObj, resObj);
            if (result.isString()) {
                res->end(result.stringValue());
            } else {
                res->end("");
            }
        });

        return this;
    }

    Php::Value listen(Php::Parameters& params)
    {
        if (params.size() < 1 || !params[0].isNumeric()) {
            throw Php::Exception("Port must be a number");
        }

        int port = params[0].numericValue();

        _app.listen(port, [port](auto* listen_socket) {
            if (listen_socket) {
                std::cout << "Listening " << port << "\n";
            } else {
                std::cout << "Failed to listen " << port << "\n";
            }
        });

        return this;
    }

    void run()
    {
        _app.run();
    }
};

extern "C" {
PHPCPP_EXPORT void* get_module()
{
    // Create extension
    static Php::Extension extension("uwebsockets", "0.0.1");

    // Declare HttpRequest
    Php::Class<HttpRequest> httpRequest("HttpRequest", Php::Final);
    httpRequest.method<&HttpRequest::getUrl>("getUrl");
    httpRequest.method<&HttpRequest::getMethod>("getMethod");
    httpRequest.method<&HttpRequest::getQuery>("getQuery");
    httpRequest.method<&HttpRequest::getParameter>("getParameter", { Php::ByVal("name", Php::Type::String) });
    httpRequest.method<&HttpRequest::getHeader>("getHeader", { Php::ByVal("name", Php::Type::String) });

    extension.add(std::move(httpRequest));

    // Declare HttpResponse
    Php::Class<HttpResponse> httpResponse("HttpResponse", Php::Final);
    httpResponse.method<&HttpResponse::write>("write", { Php::ByVal("chunk", Php::Type::String) });
    httpResponse.method<&HttpResponse::setStatus>("setStatus", { Php::ByVal("status", Php::Type::String) });
    httpResponse.method<&HttpResponse::setHeader>("setHeader", { Php::ByVal("key", Php::Type::String), Php::ByVal("value", Php::Type::String) });

    extension.add(std::move(httpResponse));

    // Declare UServer
    Php::Class<UServer> userver("UServer", Php::Final);
    userver.method<&UServer::__construct>("__construct");
    userver.method<&UServer::get>("get", { Php::ByVal("path", Php::Type::String), Php::ByVal("handler", Php::Type::Callable) });
    userver.method<&UServer::listen>("listen", { Php::ByVal("port", Php::Type::Numeric) });
    userver.method<&UServer::run>("run");

    extension.add(std::move(userver));

    return extension.module();
}
}
