#ifndef BEE_FISH_B_STRING__B_STRING_H
#define BEE_FISH_B_STRING__B_STRING_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <cwctype>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <random>
#include <uchar.h>

#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha3.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

#ifdef SERVER
#include <filesystem>
#endif

#include "../Miscellaneous/Base64.hpp"
#include "../power-encoding/power-encoding.h"
#include "../Size.hpp"


#include "locale.hpp"
#include "char.h"

#ifdef SERVER
using namespace std::filesystem;
#endif

namespace BeeFishBString
{

using namespace BeeFishMisc;

//typedef vector<Char> BStringBase;
typedef uint8_t Byte;

typedef std::string BStringBase;

class BString;
class Iterator;

// A string of variable length characters.
// Can be created from wide string format,
// and utf-8 format.
// Once created, holds each character as a
// power encoded vector of bits
class BString : public BStringBase
{
private:



public:
    typedef Char ValueType;

    // empty string
    BString()
    {
    }

    // standard copy constructor
    BString(const BString &source) :
        BStringBase(source)
    {
    }

    // from vector
    BString(const BStringBase &source) :
        BStringBase(source)
    {
    }

    // repeated character
    BString(Index length, char c) :
        BStringBase(length, c)
    {
    }

    // single character
    BString(char c)
    {
        push_back(c);
    }

    virtual ~BString() {

    }

#ifdef SERVER
    // from path
    BString(const std::filesystem::path &path) : BString(string(path))
    {
    }
#endif

    // from copy iterators
    BString(
        BStringBase::const_iterator first,
        BStringBase::const_iterator last) : BStringBase(first, last)
    {
    }

    // c string
    BString(const char *cstring) :
        BString(
            cstring ?
            std::string(cstring) :
            std::string("")
        )
    {
    }

    // c string with length
    BString(const char *cstring, size_t len) : BString(std::string(cstring, len))
    {
    }


    std::string str() const {
        stringstream stream;
        stream << *this;
        return stream.str();
    }

    void push_back(const Char &character)
    {
        *this += character;
    }

    void push_back(char c)
    {
        BStringBase::push_back(c);
    }

    BString &operator+=(const BString &rhs)
    {
        std::string::operator += (rhs);
        return *this;
    }

    friend BString operator+(const BString &lhs, const BString& rhs)
    {
        BString str = lhs;
        str += rhs;
        return str;
    }

    BString &operator+=(const char &rhs)
    {
        std::string::operator += (rhs);
        return *this;
    }


    virtual BString& operator = (const BString& rhs) {
        BStringBase::operator = (rhs);
        //_utf8 = rhs._utf8;
        return *this;
    }

    bool startsWith(const BString& prefix) const {
        return (rfind(prefix, 0) == 0);
    }

    bool isPunctuation() const {

        if (!size())
            return false;

        std::wstring wvalue =
            utf8_to_wstring();

        return
            std::iswpunct(wvalue[0]) &&
            !isEmoji();


    }

    bool isSpace() const {
        if (!size())
            return false;

        const std::wstring wvalue =
            utf8_to_wstring();

        return std::iswspace(wvalue[0]);

    }

    bool isEmoji() const {

        if (!size())
            return false;
            
        std::wstring wvalue =
            utf8_to_wstring();

        wchar_t codepoint = wvalue[0];

        return
            (codepoint >= 0x1F600 && codepoint <= 0x1F64F) || // Emoticons
            (codepoint >= 0x1F300 && codepoint <= 0x1F5FF) || // Misc Symbols & Pictographs
            (0x1F680 <= codepoint && codepoint <= 0x1F6FF) || // Transport/Map
            (0x1F900 <= codepoint && codepoint <= 0x1F9FF) || // Supplemental Symbols
            (0x2600  <= codepoint && codepoint <= 0x26FF);   // Misc Symbols
    }

    bool isSeperator() const
    {
        if (!size())
            return false;

        char c = (*this)[0];

        return
            (c == '.') ||
            (c == '-') ||
            (c == '_');
    }

    bool isData() const
    {
        return
            startsWith("data:") &&
            find(",") != std::string::npos;
    }

    bool isUserId() const
    {
        if (size() != USER_ID_SIZE)
            return false;

        return
            std::all_of(
                begin(),
                end(),
        [](unsigned char c) {
            return std::isxdigit(c);
        }
            );

    }

    vector<BString> tokenise() const
    {
        BString partWord;
        auto result =
            tokenise(
                partWord,
                true
            );

        assert(!partWord.size());

        return result;
    }

    // Defined below
    vector<BString> tokenise(BString& partWord, bool finalWord) const;


    vector<BString> split(
        const char character
    ) const
    {
        BString segment;
        vector<BString> segments;

        for (auto c : *this)
        {

            if (c == character)
            {
                segments.push_back(segment);
                segment.clear();
            }
            else
                segment.push_back(c);
        }

        segments.push_back(segment);

        return segments;
    }


    BString toBase64() const
    {
        // Set up input and output streams
        std::ostringstream output;

        // Create the encoder object
        BeeFishMisc::Base64EncodeStream
        encoder(output);

        // Decode the input stream to the output stream
        encoder << *this;
        encoder << flush;

        return output.str();


    }

    BString fromBase64() const
    {

        const std::string base64 = *this;
        std::stringstream output;
        Base64DecodeStream decoder(output);
        decoder << base64;

        return output.str();



    }
#ifdef SERVER
    BString sha3() const
    {
        using namespace CryptoPP;

        const BString& message = *this;

        BString digest;

        // Use SHA3-256 to hash the message and encode it into Hex
        SHA3_256 hash;
        StringSource ss(
            message,
            true,
            new HashFilter(
                hash,
                new HexEncoder(
                    new StringSink(digest),
                    false
                )
            )
        );

        return digest;
    }
#endif

    static BString createRandom(int length) {
        const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        // Seed with a real hardware entropy source if available
        std::random_device rd;
        // Initialize Mersenne Twister engine
        std::mt19937 generator(rd());
        // Define the range [0, charset.size() - 1]
        std::uniform_int_distribution<> distribution(0, charset.size() - 1);

        BString randomString;
        for (int i = 0; i < length; ++i) {
            randomString += charset[distribution(generator)];
        }

        return randomString;
    }


    BString toHex() const
    {
        const std::string& data = *this;

        static const char digits[] = "0123456789abcdef";

        BString hex;

        const Byte* _data =
            (const Byte*)data.data();

        for (size_t i = 0; i < data.size(); ++i) {
            hex += digits[_data[i] / 16];
            hex += digits[_data[i] % 16];
        }

        return hex;

    }

    BString fromHex() const
    {
        const BString& hex = *this;

        BString result(hex.length() / 2, '\0');

        for (size_t i = 0; i < hex.length(); i += 2)
        {
            std::string byteString = hex.substr(i, 2);
            char byte = (char) strtol(byteString.c_str(), NULL, 16);
            result[i / 2] = byte;
        }

        return result;
    }


    virtual void writeEscaped(
        ostream &out
    ) const
    {
        BString partWord;
        escape(out, partWord);
    }


    friend PowerEncoding &operator>>(
        PowerEncoding &stream,
        BString &bString)
    {
        return BeeFishPowerEncoding::
               operator>>(stream, (std::string&)bString);
    }


    friend PowerEncoding &operator<<(
        PowerEncoding &stream,
        const BString &bString)
    {
        return BeeFishPowerEncoding::
               operator<<(stream, (std::string&)bString);
    }

    friend ostream& operator<<(
        ostream &output,
        const BString  &bString)
    {
        for (auto c : bString) {
            output << c;
        }

        return output;
    }

    // trim from start
    BString ltrim()
    {
        BString s = *this;
        s.erase(
            s.begin(),
            std::find_if(
                s.begin(),
                s.end(),
                [](char ch)
        {
            return !std::isspace(ch);
        }));
        return s;
    }

    // trim from end (in place)
    BString rtrim()
    {
        BString s = *this;

        s.erase(
            std::find_if(
                s.rbegin(),
                s.rend(),
                [](char ch)
        {
            return !std::isspace(ch);
        })
        .base(),
        s.end());

        return s;
    }

    bool endsWith(const BString& ending) const {
        if (ending.size() > size())
            return false;
        return std::equal(ending.rbegin(), ending.rend(), rbegin());
    }

    BString trim()
    {
        return ltrim().rtrim();
    }


    bool contains(const Char& character) const
    {
        if (find(character) != std::string::npos)
            return true;

        return false;
    }

    bool contains(char character) const
    {
        if (find(character) != std::string::npos)
            return true;

        return false;
    }

    BString substr(Index pos, Index len = Index(-1)) const
    {
        BStringBase::const_iterator
        start = cbegin() + pos;

        BStringBase::const_iterator
        end;

        if (len != Index(-1))
            end = start + len;
        else
            end = cend();
        return BString(start, end);
    }

    BString encodeURI() const {

        using namespace std;

        ostringstream escaped;
        escaped.fill('0');
        escaped << hex;

        for (std::string::const_iterator i = cbegin(), n = cend(); i != n; ++i) {
            char c = (*i);

            // Keep alphanumeric and other accepted characters intact
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '{' || c == '}' || c == '\'') {
                escaped << c;
                continue;
            }

            // Any other characters are percent-encoded
            escaped << uppercase;
            escaped << '%' << setw(2) << int((unsigned char) c);
            escaped << nouppercase;
        }

        return escaped.str();
    }

    BString decodeURI() const {
        BString decoded;
        char character;
        const BString& encoded = *this;
        int i, ii, len = length();

        for (i=0; i < len; i++) {
            if (encoded[i] != '%') {
                //if((*this)[i] == '+')
                //    decoded.push_back(' ');
                //else
                decoded.push_back(encoded[i]);
            }
            else {
                std::string part = encoded.substr(i + 1, 2);
                sscanf(part.c_str(), "%x", &ii);
                character = static_cast<char>(ii);
                decoded.push_back(character);
                i = i + 2;
            }
        }

        return decoded;

    }

    bool isDigitsOnly() const {
        if (empty()) {
            return false;
        }
        // Check if all characters are digits
        return
            std::all_of(
                begin(),
                end(),
                ::isdigit
            );
    }

    friend istream &getline(istream &in, BString &line)
    {
        string str;
        getline(in, str);
        line = str;
        return in;
    }

    static wchar_t utf8towchar(const BString& utf8)
    {
        std::string hex = utf8.toHex();
        // Can also be just "0041"

        // Convert base-16 string to unsigned long, then cast to wchar_t
        wchar_t wc = static_cast<wchar_t>(std::stoul(hex, nullptr, 16));

        return wc;
    }

    static std::wstring hexToWString(const BString& utf8) {
        std::wstring result;
        std::string hex = utf8.toHex();
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            wchar_t byte = static_cast<wchar_t>(std::stoul(byteString, nullptr, 16));
            result += byte;
        }
        return result;
    }

    // convert UTF-8 string to wstring
    std::wstring utf8_to_wstring() const
    {
        /*
        return hexToWString(*this);
        
        std::wstringstream wstream;
        BString utf8;
        Index position = 0;
        // mbstate_t state = {0};

        while (position < size())
        {
            utf8 = nextUTF8(position);
            assert(utf8.size());

            wstream << hexToWString(utf8);


            
            if (utf8 == "💜") {
                cerr << "SIZE " << utf8.size() << endl;
            }

            //  std::reverse(utf8.begin(), utf8.end());

            switch (utf8.size())
            {
            case 0:
                assert(false);
                break;
            case 1:
                utf8 = BString(3, '\0') + utf8;
                break;
            case 2:
                utf8 = BString(2, '\0') + utf8;
                break;
            case 3:
                utf8 = BString(1, '\0') + utf8;
                break;
            case 4:
                break;
            default:
                assert(false);
            }


            assert(utf8.size() == 4);
            assert(sizeof(wchar_t) == 4);

            // uint32_t ui32;
            wstream.write((const wchar_t*)(const byte*)utf8.data(), 1);

            // wstream << (wchar_t)ui32;

            //cerr << to_string(ui32) << endl;
            
        }

        // cerr << endl;

        return wstream.str();
        */
        std::wstring output;
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        
        try {
            output = myconv.from_bytes(data(), data() + size());
        }
        catch(const std::range_error& ex)
        {
            cerr << "BSTRING BASE CONV ERROR " << endl;
            cerr << *this << ":" << encodeURI() << endl;
            throw ex;
        }
        
        return output;
    }

    // convert wstring to UTF-8 string
    static BString wstring_to_utf8 (const std::wstring& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(str);
    }

    BString toLower() const {
        wstring wvalue = utf8_to_wstring();

        std::transform(
            wvalue.begin(),
            wvalue.end(),
            wvalue.begin(),
        [](wchar_t c) {
            return std::towlower(c);
        }
        );
        return wstring_to_utf8(wvalue);
    }

    BString toUpper() const {

        wstring wvalue = utf8_to_wstring();

        std::transform(
            wvalue.begin(),
            wvalue.end(),
            wvalue.begin(),
        [](wchar_t c) {
            return std::towupper(c);
        }
        );
        return wstring_to_utf8(wvalue);

    }

    BString escape() const
    {
        std::stringstream stream;
        Index position = 0;
        BString partUTF8;

        while (position < size())
        {
            BString utf8 = nextUTF8(
                               position,
                               partUTF8
                           );
            if (partUTF8.size() == 0)
                stream << escape(utf8);
        }

        return stream.str();
    }

    void escape(ostream& out, BString& partUTF8) const
    {
        Index position = 0;
        BString value = partUTF8 + *this;

        while (position < value.size())
        {
            BString utf8 = value.nextUTF8(
                               position,
                               partUTF8
                           );
            if (partUTF8.size() == 0)
                out << escape(utf8);
        }

    }

    static BString escape(
        const BString& c
    )
    {

        if (c == "\0")
            return "\\0";

        if (c == "\"")
            return "\\\"";

        if (c == "\\")
            return "\\\\";

        if (c == "\b")
            return "\\b";

        if (c == "\f")
            return "\\f";

        if (c == "\r")
            return "\\r";

        if (c == "\n")
            return "\\n";

        if (c == "\t")
            return "\\t";

        if (c == "\v")
            return "\\v";

        if (c.size() == 1 && c[0] >= 0)
            return c;
        else if (c.size() == 1 && c[0] < 0)
        {
            BString unicode;
            if (c.size() % 2 > 0)
                unicode = BString(2 - c.size() % 2, char(0)) + c;
            else
                unicode = c;

            assert(unicode.size() % 2 == 0);

            stringstream stream;
            for (Index i = 0; i < unicode.size(); i += 2)
            {
                assert(i < unicode.size() - 1);
                stream << "\\u";
                stream << setfill('0') << setw(2) << hex << (int)(unsigned char)unicode[i];
                stream << setfill('0') << setw(2) << hex << (int)(unsigned char)unicode[i + 1];
            }

            return stream.str();
        }
        return c;

    }


    BString unescape() const
    {
        const BString& input = *this;
        stringstream stream;
        for (Index i = 0; i < input.size();) {
            char c = input[i++];
            if (c == '\\' && i < input.size()) {
                char c2 = input[i++];
                if (c2 == 'u')
                {

                    Index count = 0;
                    while (count < 2 &&
                            i < input.size() &&
                            input[i] == '0')
                    {
                        ++count;
                        ++i;
                    }

                    BString str;
                    for (; count < 4 && i < input.size(); ++count)
                    {
                        str += input[i++];
                    }

                    stream << str.fromHex();

                }
                else
                    stream << BString::unescape(c2);
            }
            else
                stream << c;
        }

        return stream.str();
    }

    static char unescape(
        char c
    )
    {
        switch (c)
        {
        case '0':
            return '\0';
        case '\"':
            return '\"';
        case '\\':
            return '\\';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'r':
            return '\r';
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        default:
            return c;
        }


    }

    BString nextUTF8(
        Index& position
    ) const
    {
        BString partUTF8;
        BString utf8 = nextUTF8(
                           position,
                           partUTF8
                       );
        assert(partUTF8.size() == 0);

        return utf8;
    }

    inline static Index expectedBytes(char c)
    {
        if ((c &      0b11110000) == 0b11110000)
            return 4;
        else if ((c & 0b11100000) == 0b11100000)
            return 3;
        else if ((c & 0b11000000) == 0b11000000)
            return 2;
        else if ((c & 0b10000000) == 0b10000000)
            return 1;
        else
            return 0;
    }

    inline static bool expectNextByte(char c)
    {
        return ((c & 0b10111111) == c);
    }

    BString nextUTF8(
        Index& position,
        BString& partUTF8
    )
    const
    {
        BString value =
            partUTF8 +
            *this;

        if (!value.size())
            return "";


        partUTF8 = "";

        char c = value[position++];

        Index expectedBytes =
            BString::expectedBytes(c);

        partUTF8.push_back(c);

        while (expectedBytes &&
               --expectedBytes > 0 &&
               position < value.size())
        {

            c = value[position++];

            partUTF8.push_back(c);

            if (BString::expectNextByte(c))
            {
                ++expectedBytes;
            }


        }

        if (expectedBytes == 0)
        {
            BString completeUTF8 = partUTF8;
            partUTF8 = "";
            return completeUTF8;

        }

        position += partUTF8.size();

        return "";

    }


    // Defined below
    Iterator utf8Begin(
        BString& partUTF8
    ) const;

    // Defined below
    Iterator utf8End() const;

};


class Iterator {
protected:
    const BString& _bString;
    BString _value;
    bool _isEnd = true;
    Index _position = 0;
    BString& _partU8;
public:


    // Iterator traits (required for STL compatibility in C++17 and earlier)
    using iterator_category = std::forward_iterator_tag;
    using value_type        = BString;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const BString*;
    using reference         = const BString&;

    Iterator(const BString& string, BString& partUTF8, bool isEnd = false)
        : _bString(string),
          _partU8(partUTF8)
    {

        if (isEnd) {
            _isEnd = true;
        }
        else {
            _isEnd = !setValue();
        }
    }


    bool setValue() {

        BString word;

        // Skip blanks and punctuation
        // testing for emojis along the way
        while (_position < _bString.size())
        {
            BString character =
                _bString.nextUTF8(
                    _position,
                    _partU8
                );
                

            if (_partU8.size() == 0)
            {
                if (character.isEmoji())
                {
                    _value = character;
                    return true;
                }

                if (!character.isSpace() &&
                    !character.isPunctuation())
                {
                    word = character;
                    break;
                }

            }

        }

        // Read the rest of the word
        while (_position < _bString.size())
        {
            Index lastPosition = _position;

            BString character =
                _bString.nextUTF8(
                    _position,
                    _partU8
                );

            if (_partU8.size() == 0)
            {
                if (character.isEmoji())
                {
                    if (word.size())
                        // Emoji split us,
                        // revert position so we can
                        // consume the character later
                        _position = lastPosition;
                    else 
                        // First emoji
                        // consider as word
                        word = character;
                    
                    break;
                }
                else if ( character.isSpace() ||
                     ( character.isPunctuation() &&
                       !character.isSeperator() )
                   )
                {
                    break;
                }

                word += character;
            }
        }

        if (_partU8.size() == 0 &&
            word.size())
        {
            _value = word;
            return true;
        }

        return false;

    }



    // Dereference operator (*)
    reference operator*() const
    {
        return _value;
    }

    // Reference operator -*.
    pointer operator -> () const
    {
        return &_value;
    }

    // Prefix increment operator (++)
    Iterator& operator++() {

        _isEnd = _isEnd or not
                 setValue();

        return *this;
    }

    // Postfix increment operator (++)
    Iterator operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator == (
        const Iterator& a,
        const Iterator& b
    )
    {
        return  (a._isEnd == b._isEnd &&
                 &a._bString == &b._bString);
    }

    friend bool operator != (
        const Iterator& a,
        const Iterator& b
    )
    {
        return (a._isEnd != b._isEnd ||
                &a._bString != &b._bString);
    }

    Index position() const {
        return _position;
    }


};

// Declared above
Iterator BString::utf8Begin(
    BString& partUTF8
) const
{
    return Iterator(*this, partUTF8);
}

// Declared above
Iterator BString::utf8End() const
{
    static BString partUTF8 = "";

    return Iterator(*this, partUTF8, true);
}

// Declared above
vector<BString> BString::tokenise(
    BString& partWord,
    bool finalWord
) const
{
    std::vector<BString> words;

    auto splitOnDot =
        [&words](BString word)
    {
        BString split;
        Index index = 0;

        while (index < word.size())
        {
            // Add token and type
            BString character = word.nextUTF8(index);

            if (character.isSeperator())
            {
                if (split.size())
                    words.push_back(
                        split.toLower()
                    );

                split = "";
            }
            else
                split.push_back(character);

        }

        if (split.size())
            words.push_back(
                split.toLower()
            );

    };

    std::vector<BString> tokens;

    BString value = partWord + *this;

    partWord = "";

    Iterator end = value.utf8End();
    BString partUTF8;

    // Iterate over words correctly
    // utf-8 aligned
    for (auto it = value.utf8Begin(partUTF8);
            it != end;
            ++it)
    {
        assert(partUTF8.size() == 0);

        // Add token and type
        BString token = *it;

        auto itTestEnd = it;
        ++itTestEnd;
        if (!finalWord &&
                itTestEnd == end)
        {
            // last word or part of
            // next page
            partWord = token;

            break;
        }


        splitOnDot(
            token
        );

        words.push_back(token.toLower());


    }

    partWord += partUTF8;


    // 1. Sort words
    std::sort(words.begin(), words.end());

    // 2. Remove consecutive duplicates: v becomes {1, 2, 3, 4, 5, ?, ?, ?, ?, ?}
    //    'last' points to the first '?'
    auto last = std::unique(words.begin(), words.end());

    // 3. Erase the extra elements
    words.erase(last, words.end());

    return words;
}

inline bool operator==(const char* _1, const BStringBase& _2)
{
    return BString(_1) == BString(_2);
}

inline bool operator==(const BStringBase& _1, const char *_2)
{
    return BString(_1) == BString(_2);
}

inline bool operator!=(const BStringBase& _1, const char *_2)
{
    return BString(_1) != BString(_2);
}


inline BString operator + (const char* str1, const BString& bstr2) {
    BString bstr1 = str1;
    return bstr1 + bstr2;
}

inline BString operator + (const BString& bstr1, const char* str2) {
    BString bstr2 = str2;
    return bstr1 + bstr2;
}

}

#endif
