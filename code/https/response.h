#ifndef BEE_FISH_HTTPS__RESPONSE_H
#define BEE_FISH_HTTPS__RESPONSE_H

#include <filesystem>

#include "app.h"

using namespace std;
using namespace std::filesystem;

namespace BeeFishHTTPS {

class Session;

class Response {
protected:
    const bool _log = true;

    Session* _session;
    Size _bytesTransferred = 0;
    Size _contentLength = 0;
    const Size _pageSize = getPageSize();
    BeeFishHTTPS::Authentication* _authentication;

    // App*      _app = nullptr;
public:
    Response(
        Session* session
    ) :
        _session(session)
    {

        _authentication =
            new BeeFishHTTPS::Authentication(_session);

    }

    virtual void handleResponse()
    {

        ResponseHeaders headers(_session);
        App* app = nullptr;

        for ( auto factory : appFactories )
        {

            app = factory->create(
                      _session,
                      headers,
                      *_authentication
                  );

            try {

                app->handleResponse();

            }
            catch (const std::exception& exception)
            {
                BString name = app->name();
                delete app;
                logException(
                    BString("Response::handleResponse:") +
                    name,
                    exception
                );

                // Should never reach here
                assert(false);
            }

            if (app->status() != -1)
                break;

            delete app;

            app = nullptr;

        }

        if (app)
        {
            if (_log) {
                clog << BeeFishDate::getDateTime()
                     << " " << ipAddress()
                     << " " << app->status()
                     << " " << app->statusText()
                     << " Served by "
                     << app->name()
                     << endl;
            }

            _contentLength = app->contentLength();

            if (_contentLength == Size(-1)) {
                headers.erase("content-length");
                headers.replace(
                    "transfer-encoding",
                    "chunked"
                );
            }
            else {
                headers.replace(
                    "content-length",
                    std::to_string(_contentLength)
                );
            }

            write(app);

            delete app;
            app = nullptr;

            closeOrRestart();

        }



    }

    // Defined in session.h
    void logException(const BString& where, const std::exception& exception);


    virtual ~Response()
    {
        delete _authentication;
    }

    // Defined in session.h
    const BString& ipAddress() const;

    // Defined in session.h
    void closeOrRestart();


    // Defined in response-stream.h
    void write(App* app);


};

}

#endif