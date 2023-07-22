#ifndef BEE_FISH__JSON_PARSER__HPP
#define BEE_FISH__JSON_PARSER__HPP
#include <vector>
#include <boost/json.hpp>
#include "../Parser/Parser.hpp"
#include "../Database/Path.hpp"

using namespace BeeFishParser;
using namespace BeeFishDatabase;

namespace BeeFishWebDB {

class JSONDBHandler
{
public:

    enum JSONType {
       INT64,
       UINT64,
       DOUBLE,
       BOOL,
       _NULL,
       STRING,
       OBJECT,
       ARRAY,
       KEY,
       ROOT
    };


    const int OBJECT_INDEXES = 0;
    const int OBJECT_KEYS = 1;

protected:
    struct JSONValue {
       JSONType _type;
       size_t _arrayIndex;
       BeeFishWeb::Path _path;
    };

    BeeFishWeb::Path _rootPath;
    vector<JSONValue> _stack;
    size_t _count {0};
public:

    JSONDBHandler(BeeFishWeb::Path path) :
        _rootPath(path)
    {
    }

    friend PowerEncoding& operator <<
    ( 
        PowerEncoding& stream,
        const JSONType& data
    )
    {
       stream << (long)data;
       return stream;
    }

    JSONValue& lastValue() {
        assert(size() > 0);
        return _stack[size() - 1];
    }

    JSONType& lastType() {
        return lastValue()._type;
    }

    size_t& lastArrayIndex() {
        return lastValue()._arrayIndex;
    }

    BeeFishWeb::Path lastPath() {
        return lastValue()._path;
    }

    size_t size() {
        return _stack.size();
    }

    void push_back(JSONValue value) {
       ++_count;
       _stack.push_back(value);
    }

    JSONValue pop_back()
    {
        assert(size() > 0);

        JSONValue value =
           _stack[size() - 1];

        _stack.pop_back();

        --_count;
        return value;
    }

    template<typename T>
    void setData(JSONType type, const T& data) {

        JSONType _lastType = lastType();
        if (_lastType == KEY) {
            lastPath()[type]
               .setData(data);

            assert(lastType() == KEY);
            pop_back();
        }
        else if (_lastType == ARRAY) {
            size_t& index = lastArrayIndex();
            lastPath()
                [index++]
                [type]
                .setData(data);
        }
  
    }

    friend ostream& operator << (ostream& out, const JSONType& type)
    {
       switch(type) {
           case INT64:
               out << "INT64";
               break;
           case UINT64:
               out << "UINT64";
               break;
           case DOUBLE:
               out << "DOUBLE";
               break;
           case BOOL:
               out << "BOOL";
               break;
           case _NULL:
               out << "_NULL";
               break;
           case STRING:
               out << "STRING";
               break;
           case OBJECT:
               out << "OBJECT";
               break;
           case ARRAY:
               out << "ARRAY";
               break;
           case KEY:
               out << "KEY";
               break;
           case ROOT:
               out << "ROOT";
               break;
           default:
              out << "<Unknown type: " << (int)type << ">";
        };

        return out;
    }

    /// The maximum number of elements allowed in an array
    static constexpr std::size_t max_array_size = -1;

    /// The maximum number of elements allowed in an object
    static constexpr std::size_t max_object_size = -1;

    /// The maximum number of characters allowed in a string
    static constexpr std::size_t max_string_size = -1;

    /// The maximum number of characters allowed in a key
    static constexpr std::size_t max_key_size = -1;

    /// Called once when the JSON parsing begins.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_document_begin( error_code& ec )
    {
        push_back(
            JSONValue {ROOT, 0, _rootPath}
        );

        return true;
    }

    /// Called when the JSON parsing is done.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_document_end( error_code& ec )
    {
        pop_back();
        assert(size() == 0);
        return true;
    }

    /// Called when the beginning of an array is encountered.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_array_begin( error_code& ec )
    {

        push_back(
           {ARRAY, 0, lastPath()[ARRAY]}
        );

        return true;
    }

    /// Called when the end of the current array is encountered.
    ///
    /// @return `true` on success.
    /// @param n The number of elements in the array.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_array_end( std::size_t n, error_code& ec )
    {
        assert(lastType() == ARRAY);
        pop_back();
        setData(ARRAY, n);
        return true;
    }

    /// Called when the beginning of an object is encountered.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_object_begin( error_code& ec )
    {
        push_back(
           {OBJECT, 0, lastPath()[OBJECT][OBJECT_KEYS]}
        );

        return true;
    }

    /// Called when the end of the current object is encountered.
    ///
    /// @return `true` on success.
    /// @param n The number of elements in the object.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_object_end( std::size_t n, error_code& ec )
    {
        assert(lastType() == OBJECT);
        pop_back();

        setData(OBJECT, n);

        return true;
    }

    /// Called with characters corresponding to part of the current key.
    ///
    /// @return `true` on success.
    /// @param s The partial characters
    /// @param n The total size of the key thus far
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_key_part( string_view s, std::size_t n, error_code& ec )
    {

        return true;
    }

    /// Called with the last characters corresponding to the current key.
    ///
    /// @return `true` on success.
    /// @param s The remaining characters
    /// @param n The total size of the key
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_key( string_view s, std::size_t n, error_code& ec )
    {
        std::string key(s);

        push_back(
           {KEY, 0, lastPath()[key]}
        );

        return true;
    }

    /// Called with the characters corresponding to part of the current number.
    ///
    /// @return `true` on success.
    /// @param s The partial characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_number_part( string_view s, error_code& ec )
    {
        return true;
    }

    /// Called with characters corresponding to part of the current string.
    ///
    /// @return `true` on success.
    /// @param s The partial characters
    /// @param n The total size of the string thus far
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_string_part( string_view s, std::size_t n, error_code& ec )
    {
        return true;
    }

    /// Called with the last characters corresponding to the current string.
    ///
    /// @return `true` on success.
    /// @param s The remaining characters

    /// @param ec Set to the error, if any occurred.
    ///
    bool on_string( string_view s, std::size_t n, error_code& ec )
    {
        std::string data(s);

        setData(STRING, data);

        return true;
    }


    /// Called when a signed integer is parsed.
    ///
    /// @return `true` on success.
    /// @param i The value
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_int64( int64_t i, string_view s, error_code& ec )
    {
        setData(INT64, i);
        return true;
    }

    /// Called when an unsigend integer is parsed.
    ///
    /// @return `true` on success.
    /// @param u The value
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_uint64( uint64_t u, string_view s, error_code& ec )
    {
        setData(UINT64, u);
 
        return true;
    }

    /// Called when a double is parsed.
    ///
    /// @return `true` on success.
    /// @param d The value
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_double( double d, string_view s, error_code& ec )
    {
        setData(DOUBLE, d);

        return true;
    }

    /// Called when a boolean is parsed.
    ///
    /// @return `true` on success.
    /// @param b The value
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_bool( bool b, error_code& ec )
    {
        setData(BOOL, b);

        return true;
    }

    /// Called when a null is parsed.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_null( error_code& ec )
    {
        setData(_NULL, 0);


        return true;
    }

    /// Called with characters corresponding to part of the current comment.
    ///
    /// @return `true` on success.
    /// @param s The partial characters.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_comment_part( string_view s, error_code& ec )
    {
        return true;
    }

    /// Called with the last characters corresponding to the current comment.
    ///
    /// @return `true` on success.
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_comment( string_view s, error_code& ec )
    {
        return true;
    }

};

   class JSONParser : public Parser
   {
   protected:
      boost::json::basic_parser<JSONDBHandler> _parser;
      BeeFishWeb::Path _rootPath;
      std::string _buffer;
      const size_t _contentLength;
      size_t _bytesRead = 0;
      const size_t _pageSize = getpagesize();

      JSONParser(const JSONParser& source) :
         Parser(),
         _parser(boost::json::parse_options(), source._rootPath),
         _rootPath(source._rootPath),
         _contentLength(source._contentLength)
      {
      }

   public:

      JSONParser(
         BeeFishWeb::Path path,
         size_t contentLength
      ) :
         Parser(),
         _parser(boost::json::parse_options(), path),
         _rootPath(path),
         _contentLength(contentLength)
      {
      }

      virtual ~JSONParser() {
      }

      virtual bool read(char c)
      override
      {
         ++_bytesRead;

         _buffer.push_back(c);
         if (_buffer.size() == _pageSize ||
             _bytesRead == _contentLength)
         {
            return flush();
         }

         return true;

      }

      virtual bool flush()
      override
      {

         if (!Parser::flush())
            return false;

         boost::json::error_code ec;

         size_t n =
            _parser.write_some(
                true,
               _buffer.c_str(),
               _buffer.size(),
                ec
            );

         _buffer.clear();

         if (ec) {
            setResult(false);
            return false;
         }
         else if (_parser.done()) {
            setResult(true);
         }
         else if (_contentLength > 0 && 
                  _bytesRead >= _contentLength)
         {
#warning "Make sure json is complete"
            setResult(true);
         }

         return true;

      }

      virtual Parser* copy() const
      override
      {
         return new JSONParser(*this);
      }
   };

}


#endif