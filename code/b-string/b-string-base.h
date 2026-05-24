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
#include <locale>
#include <codecvt>
#include <filesystem>
#include <random>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha3.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>

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

        boost::u32regex pattern =
            boost::make_u32regex(
                L"[[:punct:]]+"
            );

        if (length() < 1)
            return false;

        std::wstring wstr = boost::locale::conv::to_utf<wchar_t>(
                                *this,
                                _locale._locale
                            );


        return boost::u32regex_match(
                   wstr,
                   pattern
               );


    }

    bool isSpace() const {

        std::wstring wstr = boost::locale::conv::to_utf<wchar_t>(
                                *this,
                                _locale._locale
                            );


        for (const auto& wch : wstr)
        {
            if (!std::iswspace(wch))
                return false;
        }

        return true;

    }
/*
    bool isValidCharacter() const
    {
        // Create a segment index for characters
        boost::locale::boundary::ssegment_index map(
            boost::locale::boundary::character, 
            begin(),
            end(),
            _locale._locale
        );
        
        for (auto it = map.begin();
                    it != map.end();
                    ++it)
        {
            BString token = it->str();
            if (it->rule() & boost::locale::boundary::word_letter ||
                it->rule() & boost::locale::boundary::word_number)
            {
            }
            else if (token.isSpace())
            {
    
            }
            else if (token.isPunctuation())
            {
                
            }
            else
                return false;

        }
        
        return true;
    }
*/
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
        enum class Type {
            Blankspace,
            Punctuation,
            Word
        };

        struct Token {
            Type _type;
            BString _word;
        };


        auto splitOnDot =
            [](
                BString word,
                std::vector<Token>& tokens
            )
        {

            boost::locale::boundary::ssegment_index map(
                boost::locale::boundary::character,
                word.begin(),
                word.end()
            );

            // Iterate over words correctly
            // utf-8 aligned
            BString split;
            for (auto it = map.begin();
                    it != map.end();
                    ++it)
            {
                // Add token and type
                BString character = it->str();
                if (character == "." ||
                        character == "_" ||
                        character == "~")
                {
                    if (split.size())
                        tokens.push_back({
                        Type::Word,
                        split.toLower()
                    });

                    tokens.push_back({
                        Type::Punctuation,
                        character
                    });

                    split = "";
                }
                else
                    split += character;

            }

            if (split.size())
                tokens.push_back({
                Type::Word,
                split
            });

        };

        std::vector<Token> tokens;

        boost::locale::boundary::ssegment_index map(
            boost::locale::boundary::word,
            begin(),
            end()
        );

        // Iterate over words correctly
        // utf-8 aligned
        for (auto it = map.begin();
                it != map.end();
                ++it)
        {
            // Add token and type
            BString token = it->str();

            if (it->rule() & boost::locale::boundary::word_letter ||
                    it->rule() & boost::locale::boundary::word_number)
                /*
                it->rule() & boost::locale::boundary::character)*/
            {
                splitOnDot(
                    token,
                    tokens
                );
            }
            else if (token.isSpace())
            {
                tokens.push_back
                (
                {
                    Type::Blankspace,
                    token
                }
                );
            }
            else if (token.isPunctuation())
            {
                tokens.push_back
                (
                {
                    Type::Punctuation,
                    token
                }
                );
            }
            else
            {
                tokens.push_back
                (
                {
                    Type::Word,
                    token
                }
                );
            }



        }

        Index i = 0;

        std::vector<BString> words;
        while (i < tokens.size())
        {
            Token token = tokens[i];

            // strip leading non-words
            while (token._type != Type::Word)
            {
                if (++i >= tokens.size())
                    break;
                token = tokens[i];
            }

            // No words
            if (token._type != Type::Word)
                break;

            // Find next blank space
            Index nextBlankspace = i;


            while (nextBlankspace < tokens.size())
            {
                token = tokens[nextBlankspace];


                if (token._type == Type::Blankspace)
                    break;

                ++nextBlankspace;

            }

            Index trailingPunctuation = nextBlankspace;
            token = tokens[trailingPunctuation - 1];
            while (token._type == Type::Punctuation)
            {
                --trailingPunctuation;
                token = tokens[trailingPunctuation - 1];
            }

            // Add combinations of words
            for (Index j = i; j < trailingPunctuation; ++j)
            {
                token = tokens[j];

                // Skip start punctuation

                while (token._type == Type::Punctuation &&
                        j < trailingPunctuation)
                {
                    token = tokens[++j];
                }


                assert(token._type == Type::Word);

                BString word;
                for (Index k = j; k < trailingPunctuation; ++k)
                {

                    token = tokens[k];
                    word += token._word;

                    if (token._type == Type::Word)
                    {
                        words.push_back(word.toLower());
                    }
                }




            }

            i = nextBlankspace + 1;


        }

        // 1. Sort words
        std::sort(words.begin(), words.end());

        // 2. Remove consecutive duplicates: v becomes {1, 2, 3, 4, 5, ?, ?, ?, ?, ?}
        //    'last' points to the first '?'
        auto last = std::unique(words.begin(), words.end());

        // 3. Erase the extra elements
        words.erase(last, words.end());

        return words;



    }

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

        /*
        std::stringstream stream;
        Base64OutputStream base64(stream);
        base64 << *this;
        base64.flush();

        return stream.str();
        */
        /*
        const std::string& data = *this;
        BString encoded;
        CryptoPP::StringSource(
            data,
            true,
            new CryptoPP::Base64Encoder(
                new CryptoPP::StringSink(encoded),
                false // Do not insert line breaks
            )
        );
        return encoded;
        */
    }

    BString fromBase64() const
    {

        const std::string base64 = *this;
        std::stringstream output;
        Base64DecodeStream decoder(output);
        decoder << base64;

        return output.str();

        /*

            // The Base64 encoded string
            const std::string&
                encoded_string = *this;

            // Set up input and output streams
            std::istringstream iss(encoded_string);
            std::ostringstream oss;

            // Create the libb64 decoder object
            base64::decoder decoder;

            // Decode the input stream to the output stream
            decoder.decode(iss, oss);

            return oss.str();
            */
        /*
        const BString& base64 = *this;
        BString decoded;

        CryptoPP::StringSource(
            base64,
            true,
            new CryptoPP::Base64Decoder(
                new CryptoPP::StringSink(decoded)
            )
        );
        return decoded;
        */

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
        out << escape();
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

    BString substr(size_t pos, size_t len = 0) const
    {
        BStringBase::const_iterator
        start = cbegin() + pos;

        BStringBase::const_iterator
        end;

        if (len)
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

    // convert UTF-8 string to wstring
    std::wstring utf8_to_wstring ()
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(*this);
    }

    // convert wstring to UTF-8 string
    static BString wstring_to_utf8 (const std::wstring& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(str);
    }

    BString toLower() const {
        std::string lower =
            boost::locale::to_lower(
                *this,
                _locale._locale
            );
        return lower;
    }

    BString toUpper() const {

        std::string upper =

            boost::locale::to_upper(
                *this,
                _locale._locale
            );

        return upper;

    }

    BString escape() const
    {
        std::stringstream stream;
        for (Index i = 0; i < size(); ++i)
        {
            char c = (*this)[i];
            Index expectedBytes;
            if ((c &      0b11110000) == 0b11110000)
                expectedBytes = 4;
            else if ((c & 0b11100000) == 0b11100000)
                expectedBytes = 3;
            else if ((c & 0b11000000) == 0b11000000)
                expectedBytes = 2;
            else
                expectedBytes = 1;

            BString character;

            while (1)
            {

                character.push_back(c);

                if (--expectedBytes > 0)
                {
                    if (++i >= size())
                        break;

                    c = (*this)[i];

                    if ((c & 0b10111111) == c)
                    {
                        ++expectedBytes;
                    }
                }

                if (expectedBytes == 0)
                    break;
            }

            stream << BString::escape(character);

        }

        return stream.str();
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



};

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
