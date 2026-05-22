#ifndef BEE_FISH_MISCELLANEOUS_BASE_64_HPP
#define BEE_FISH_MISCELLANEOUS_BASE_64_HPP

#include <iostream>
#include <streambuf>
#include <vector>
#include <string>


namespace BeeFishMisc
{
    
class Base64Streambuf : public std::streambuf {
private:
    std::streambuf* _dest;
    std::vector<char> _buffer;
    int _bufferIndex;
    const char base64_chars_[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    void encode_chunk(const unsigned char* chunk, int length) {
        unsigned char b1 = chunk[0];
        unsigned char b2 = (length > 1) ? chunk[1] : 0;
        unsigned char b3 = (length > 2) ? chunk[2] : 0;

        char out[4];
        out[0] = base64_chars_[b1 >> 2];
        out[1] = base64_chars_[((b1 & 0x03) << 4) | (b2 >> 4)];
        out[2] = (length > 1) ? base64_chars_[((b2 & 0x0f) << 2) | (b3 >> 6)] : '=';
        out[3] = (length > 2) ? base64_chars_[b3 & 0x3f] : '=';

        _dest->sputn(out, 4);
    }

public:
    explicit Base64Streambuf(std::streambuf* dest)
        : _dest(dest), _buffer(3), _bufferIndex(0) {}

    ~Base64Streambuf() {
        pubsync();
    }

    int overflow(int c) override {
        if (c != EOF) {
            _buffer[_bufferIndex++] = static_cast<char>(c);
            if (_bufferIndex == 3) {
                encode_chunk(reinterpret_cast<unsigned char*>(_buffer.data()), 3);
                _bufferIndex = 0;
            }
        }
        return c;
    }

    int sync() override {
        if (_bufferIndex > 0) {
            encode_chunk(reinterpret_cast<unsigned char*>(_buffer.data()), _bufferIndex);
            _bufferIndex = 0;
        }
        _dest->pubsync();
        return 0;
    }
};

class Base64EncodeStream : public std::ostream {
private:
    Base64Streambuf buf_;
public:
    explicit Base64EncodeStream(std::ostream& dest)
        :
        std::ostream(&buf_),
        buf_(dest.rdbuf())
    {
    }
};

class Base64DecodeStream
{
private:
    ostream& _output;
    int _sextets[4];
    int _count = 0;
public:
    Base64DecodeStream(ostream& output) :
        _output(output)
    {
    }
    
    friend Base64DecodeStream&
    operator << (Base64DecodeStream& decoder, const std::string& source)
    {
        for (char c : source)
        {
            if (!decoder.decodeBase64Stream(c))
                throw runtime_error("Invalid base64 character");
        }
        
        return decoder;
    }
    
protected:

    constexpr int decodeChar(char c) {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        if (c == '=') return 64; // Padding
        return -1; // Invalid character
    }

    bool decodeBase64Stream(char c) {
        

        int val = decodeChar(c);
        if (val < 0) {
            return false; // Skip whitespace/newlines or throw error depending on strictness
        }

        _sextets[_count++] = val;

        if (_count == 4) {
            _count = 0;

            // Convert 4 sextets (6 bits) into 3 octets (8 bits)
            int b0 = (_sextets[0] << 2) | (_sextets[1] >> 4);
            int b1 = ((_sextets[1] & 0xF) << 4) | (_sextets[2] >> 2);
            int b2 = ((_sextets[2] & 0x3) << 6) | _sextets[3];

            _output << static_cast<char>(b0);

            if (_sextets[2] != 64) { // Ignore padding
                _output << static_cast<char>(b1);
            }
            if (_sextets[3] != 64) { // Ignore padding
                _output << static_cast<char>(b2);
            }
        }
        return true;
    }

};
}

#endif
