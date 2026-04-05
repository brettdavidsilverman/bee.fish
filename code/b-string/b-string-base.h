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
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>

#ifdef SERVER
#include <filesystem>
#endif

#include "../power-encoding/power-encoding.h"
#include "../Database/Index.hpp"

#include "char.h"

#ifdef SERVER
using namespace std::filesystem;
#endif

namespace BeeFishBString
{

using namespace BeeFishDatabase;

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

        struct Locale
        {
            std::locale _locale;
            
            Locale() 
            {
                std::setlocale(LC_ALL, "");
                boost::locale::generator gen;
                std::locale::global(
                   // _locale = gen("en_US.UTF-8")
                    _locale = gen("")
                );
            }
            
        } inline static _locale;
        
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
        BString(size_t length, char c) :
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
            
            static boost::u32regex pattern = 
                boost::make_u32regex(
                    L"[[:punct:]]+"
                );
                //    regex_constants::icase
               // );
            
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
                    if (character == ".")
                    {
                        if (split.size())
                            tokens.push_back(
                                {
                                    Type::Word,
                                    split.toLower()
                               }
                            );
                            
                        tokens.push_back(
                            {
                                Type::Punctuation,
                                character
                            }
                        );
                        
                        split = "";
                    }
                    else
                        split += character;
                        
                }
                
                if (split.size())
                    tokens.push_back(
                        {
                            Type::Word,
                            split
                        }
                    );
                
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
                    //words.push_back(token._word.toLower());
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
            
            
        /*
            Index from = 0;
            std::vector<BString> words;
            for (Index i = 0;
                 i < tokens.size();
                 ++i)
            {
                Token& token = tokens[i];
            
                if (token._type == Type::Blankspace)
                {
                    // Blankspace seperates words
                    from = i + 1;
                }
                else
                {
                    
                    for (Index j = from; j < tokens.size(); j++)
                    {
                        // Concatenate all tokens in
                        // this blankspace
                        BString word;
                    
                        bool trailingPunctuation = true;
                        for (Index k = j + 1; k > from; --k)
                        {
                            Token& token = tokens[k - 1];
                            if (token._type == Type::Word)
                            {
                                words.push_back(
                                    token._word
                                );
                            }
                        
                            if (!trailingPunctuation ||
                                token._type == Type::Word)
                            {
                                word = token._word + word;
                                trailingPunctuation = false;
                            }
                        }
                        
                        if (word.size())
                        {
                            words.push_back(word);
                        }
                    
            
                    }
                
                }
                
            }
            */
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

        virtual void writeEscaped(
             ostream &out
        ) const
        {
            for (auto character : *this)
            {
                out << BeeFishMisc::escape(character);
            }
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
                if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
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
        
        BString decodeURI() const{
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
        
        bool isDigitsOnly() {
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
            return BeeFishMisc::escape(this->str());
        }
        
        BString unescape() const
        {
            return BeeFishMisc::unescape(this->str());
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


    inline BString operator + (const char* bstr1, const BString& bstr2) {
        BString _bstr = bstr1;
        return _bstr + bstr2;
    }
    

    
}

#endif
