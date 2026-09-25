#ifndef BEE_FISH_HTTPS__RESPONSE_STREAM_H
#define BEE_FISH_HTTPS__RESPONSE_STREAM_H

#include <filesystem>
#include "../query/Query.hpp"
#include "session.h"
#include "app.h"
#include "response.h"

using namespace std;
using namespace std::filesystem;

namespace BeeFishHTTPS {

class ResponseStream :
    private std::streambuf,
    public std::ostream
{
private:
    Size _pageSize;
    char* _buffer;
    Size _count;
    Size _bytesTransferred;
    Session* _session;
    bool _chunkedEncoding = false;
    bool _writingHeaders = true;

public:
    ResponseStream(Session* session, App* app) : std::ostream(this)
    {
        _pageSize = getPageSize();
        _buffer = new char[_pageSize];
        _count = 0;
        _bytesTransferred = 0;
        _session = session;
        setChunkedEncoding(app);

    }

    virtual ~ResponseStream()
    {
        delete[] _buffer;
    }

    void put(int c)
    {
        _buffer[_count++] = (char)c;

        if (_count == _pageSize)
            flush();
    }

    Size bytesTransferred() const
    {
        return _bytesTransferred;
    }

    void writeHeaders(App* app)
    {
        _writingHeaders = true;

        ResponseStream& stream = *this;

        stream
                << "HTTP/1.1 "
                << app->status()
                << " "
                << app->statusText()
                << "\r\n";

        const ResponseHeaders& headers =
            app->responseHeaders();

        for (auto pair : headers)
        {
            stream
                    << pair.first
                    << ": "
                    << pair.second.str()
                    << "\r\n";

        }

        stream << "\r\n";

        flush();

        _writingHeaders = false;
    }

    void writeContent(App* app)
    {

        Authentication::ScopedDatabase database(app->authentication());

        BeeFishQuery::Words words = database->words();
        Path bookmark(*database, app->_bookmark);
        Size bytesTransferred = 0;
        Size pageSize = getPageSize();
        Size contentLength = app->contentLength();
        JSONPath jsonPath = bookmark;

        if (app->serve() == App::SERVE_JSON)
        {
            jsonPath.write(app->authentication(), *this);

            flush();

            return;
        }
        else if (app->serve() == App::SERVE_QUERY)
        {
            const BString& origin = _session->origin();
            const BString& host = _session->host();
               
            BeeFishWeb::URL url =
                URL(
                    app->request()->url(),
                    host
                );
                
            BString search;
            BeeFishWeb::URL::Search& searchObject =
                url.search();

            if (searchObject.contains("q"))
            {
                search =
                    searchObject["q"];
            }
            else
            {
                // Remove preceeding ?
                search =
                    app
                    ->request()
                    ->search()
                    .decodeURI()
                    .substr(1);
            }

            bool getCount = false;
            if (search.endsWith("$"))
            {
                getCount = true;
                search = search.substr(0, search.size() - 1);
            }

            BeeFishQuery::Expression
                expression(bookmark, search);

            BeeFishQuery::PathBase* path =
                expression
                .getPath().copy();
                
            if (url.path() == "/")
            {

                BeeFishWeb::URL
                    myURL(
                        url.origin() +
                        BString("/my")
                    );
                        
                JSONPath myPath =
                    JSONPath::fromString(
                        app->authentication(),
                        *database,
                        myURL
                    );
                    
                BeeFishQuery::Expression
                    expression(myPath, search);
                    
                BeeFishQuery::AndPath path2 =
                expression
                   .getPath();
                   
                BeeFishQuery::PathBase*
                   tmpPath = new BeeFishQuery::OrPath(
                        *path,
                        path2
                    );
                    
                delete path;
                
                path = tmpPath;
            }

            BeeFishQuery::Iterable
            matches(
                app->authentication(),
                *database,
                *path
            );
            
            Index count = 0;
            
            if (getCount)
            {
                *this <<
                    to_string(
                        matches.count()
                    );
                
                flush();
                return;
            }

            BeeFishQuery::Iterable::Iterator it;

            if (searchObject.contains("next")) {
                try
                {
                    it =
                        matches.begin(std::stol(searchObject["next"]));
                }
                catch (const BeeFishDatabase::InvalidStackException& exception)
                {
                    *this << "\""
                          << "Invalid query parameters"
                          << "\"";
                    flush();
                    return;
                }
            }
            else
                it = matches.begin();

            *this << "[" << endl;
            
            while (it != matches.end() &&
                    (count < 10))
            {
                if (*it == host)
                {
                    ++it;
                    continue;
                }
                    
                BString contentType;
                
                try {
                    JSONPath path =
                        JSONPath::fromString(
                            app->authentication(),
                            *database,
                            *it +
                                BString("/{HTTP}/content-type")
                        );
                    contentType = path.getString();
                }
                catch (JSONPath::PathNotFoundException& ex)
                {
                    contentType =
                        "application/json; charset=utf-8";
                }

                *this << "    [" 
                      << endl
                      << "       \""
                      << it->escape()
                      << "\""
                      << "," << endl
                      << "       \""
                      << contentType.escape()
                      << "\""
                      << endl
                      << "    ]";

                if (++it != matches.end() && 
                    count < 10 &&
                    *it != host)
                    *this << ",";

                *this << endl;

                ++count;

            }

            if (it != matches.end())
            {
stringstream stream;
stream
    << "?q="
    << search.encodeURI()
    << "&next="
    << to_string(it.index())
    << endl;
cerr << "NEXT " << stream.str() << endl;

                *this << "    \""
                      << JSONPath(bookmark).toString(
                          app->authentication()
                      )
                      << "?q="
                      << search.encodeURI()
                      << "&next="
                      << to_string(it.index())
                      << "\""
                      << endl;

            }

            *this << "]";
            
            delete path;

            flush();

            return;

        }
        else if (app->serve() == App::SERVE_HTTP)
        {

            JSONPath content =
                jsonPath
                ["{HTTP}"]
                ["content"];

            if (content.type() == Type::STRING)
            {
                Path stringPath = content;
                stringPath =stringPath[JSONPath::VALUE];
                Iterable<Index> strings(stringPath);

                Base64DecodeStream decoder(*this);
                bool isData = false;

                for (auto index : strings)
                {


                    BString data =
                        stringPath[index]
                        .getStringData();

                    if (index == 0 &&
                            data.isData())
                    {
                        data = data.substr(
                                   data.find(",") + 1
                               );
                        isData = true;
                    }

                    if (isData)
                        decoder << data;
                    else
                    {
                        *this << data;
                    }
                }

                flush();
                return;
            }

            return;
        }

        Size length = 0;
        std::string buffer(pageSize, '\0');


        while (bytesTransferred < contentLength)
        {
            if ((contentLength - bytesTransferred)
                    > pageSize)
                length = pageSize;
            else
                length = contentLength - bytesTransferred;

            switch (app->serve())
            {
            case App::SERVE_DATA:
            {

                Size pageIndex =
                    bytesTransferred  /
                    pageSize;

                BString data =
                    bookmark[pageIndex].getStringData();

                length = data.size();

                memcpy(buffer.data(), data.data(), length);

                break;
            }
            case App::SERVE_CONTENT:
            {

                memcpy(
                    buffer.data(),
                    (const Byte*)
                    (
                        app->_content.data()
                        + bytesTransferred
                    ),
                    length
                );

                break;
            }
            case App::SERVE_FILE:
            {
                ifstream input(app->_filePath);

                input.seekg(
                    bytesTransferred
                );

                input.read(
                    buffer.data(),
                    length
                );

                input.close();

                break;
            }


            default:
            {
                throw std::logic_error("Invalid Serve enum value");
            }
            }

            if (length == 0)
                break;

            write(
                buffer.data(),
                length
            );

            bytesTransferred += length;

        }


        flush();
    }

    bool chunkedEncoding()
    {
        return _chunkedEncoding;
    }

    bool writingHeaders()
    {
        return _writingHeaders;
    }

    void setChunkedEncoding(App* app)
    {
        const ResponseHeaders& headers =
            app->responseHeaders();

        _chunkedEncoding =
            (   !headers.contains("content-length") &&
                ( headers.contains("transfer-encoding") &&
                  headers["transfer-encoding"]
                  == "chunked"
                )
            );

    }

    virtual void flush()
    {

// std::ostream::flush();


        if (chunkedEncoding() &&
                !writingHeaders())
        {

            std::stringstream stream;

            stream.imbue(std::locale("C"));

            stream << std::hex << std::uppercase << _count << std::dec << "\r\n";
            std::string str = stream.str();
            _write(str.data(), str.size());
        }

        if (_count > 0)
        {
            _write(_buffer, _count);
            _bytesTransferred += _count;
        }

        if (
            chunkedEncoding() &&
            !writingHeaders())
        {
            std::string str = "\r\n";
            _write(str.data(), str.size());
        }

        _count = 0;


    }

    void _write(
        const char* buffer,
        Size size
    )
    {

        boost::asio::write(
            *_session,
            boost::asio::buffer(
                buffer,
                size
            )
        );

    }

private:
    int overflow(int c) override
    {
        put(c);
        return 0;
    }
};


// Declared in response.h
void Response::write(App* app)
{

    ResponseStream stream(_session, app);

    stream.writeHeaders(app);

    stream.writeContent(app);

    stream.flush();

    _bytesTransferred =
        stream.bytesTransferred();
}

}

#endif