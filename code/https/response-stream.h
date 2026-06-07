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
        
        assert(app);

        Authentication::ScopedDatabase database(app->authentication());

        BeeFishQuery::Words words = database->words();
        Path bookmark(*database, app->_bookmark);
        Size bytesTransferred = 0;
        Size pageSize = getPageSize();
        Size contentLength = app->contentLength();
        JSONPath jsonPath = bookmark;

        if (app->serve() == App::SERVE_JSON)
        {
            *this << jsonPath;
            flush();

            return;
        }
        else if (app->serve() == App::SERVE_QUERY)
        {
            BString search =
                app->request()->search();

            bool getCount = false;
            if (search.endsWith("$"))
            {
                getCount = true;
                search = search.substr(0, search.size() - 1);
            }

            BeeFishQuery::Expression
            expression(bookmark, search);

            Index count = 0;
            if (!getCount)
                *this << "[" << endl;

            BeeFishQuery::AndPath path =
                expression
                .getPath();

            BeeFishQuery::Iterable
                matches(
                    app->authentication(),
                    *database,
                    path
                );

            for (auto it = matches.begin();
                    it != matches.end();
                )
            {

                if (!getCount)
                {
                    *this << "   \""
                          << it->escape()
                          << "\"";

                    if (++it != matches.end())
                        *this << ",";

                    *this << endl;

                }
                else {
                    ++it;
                }
                
                ++count;
                
                if (count == 1)
                    flush();

            }

            if (!getCount)
                *this << "]";
            else
                *this << count;

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
                        
                    if (index == 1 &&
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
                        *this << data;
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

            assert(length <= buffer.size());

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

        std::ostream::flush();


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