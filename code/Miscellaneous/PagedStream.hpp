#ifndef BEEFISH_MISCELLANEOUS_PAGED_STREAM_H
#define BEEFISH_MISCELLANEOUS_PAGED_STREAM_H

#include <iostream>
#include <streambuf>
#include <vector>
#include <string>
#include "../b-string/b-string-base.h"

namespace BeeFishMisc
{
    
    using namespace BeeFishBString;

class PagedStreamBuffer : public std::streambuf {
private:
    std::vector<char> _inBuffer;
public:

    typedef std::function<void(const BString& page)> OnPage;
private:
    OnPage _onPage;
    const size_t _pageSize = getPageSize();
    

    
    void onPage(const char* in, int len)
    {
        const BString data(in, len);
        _onPage(data);
    }
    

protected:
    int overflow(int c) override {
        if (c != EOF) {
            _inBuffer.push_back(static_cast<char>(c));
            if (_inBuffer.size() == _pageSize) {
                onPage(_inBuffer.data(), _pageSize);
                _inBuffer.clear();
            }
        }
        return c;
    }

    std::streamsize xsputn(const char* s, std::streamsize n) override {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(s);
        std::streamsize processed = 0;

        while (processed < n) {
            // Fill up the 3-byte input block
            while (_inBuffer.size() < _pageSize && processed < n) {
                _inBuffer.push_back(ptr[processed++]);
            }

            if (_inBuffer.size() == _pageSize) {
                onPage(_inBuffer.data(), _pageSize);
                _inBuffer.clear();
            }
        }
        return n;
    }

    int sync() override {
        if (!_inBuffer.empty()) {
            onPage(_inBuffer.data(), _inBuffer.size());
            _inBuffer.clear();
        }
       // return pubsync();
       return 0;
    }

public:
    /*
    explicit Base64Streambuf(std::streambuf* dest) 
        : dest_(dest), buffer_(3), buffer_index_(0) {}

    ~Base64Streambuf() {
        pubsync();
    }
    */
    explicit PagedStreamBuffer(
        OnPage onPage
    ) :
        _onPage(onPage)
    {
    }

    ~PagedStreamBuffer() override {
        pubsync();
    }
};

class PagedStream : public std::ostream {
private:
    PagedStreamBuffer _buf;
public:
    typedef PagedStreamBuffer::OnPage OnPage;
    
    explicit PagedStream(
        OnPage onPage
    ) :
        std::ostream(&_buf),
        _buf(
            onPage
        )
    {
    }
    


};

// --- Example Usage ---
/*
int main() {
    // Pipe base64 output directly to stdout
    Base64OutputStream b64out(std::cout);

    b64out << "Hello, Base64 Streaming in C++ on Linux!" << std::flush;

    return 0;
}
*/

}

#endif
