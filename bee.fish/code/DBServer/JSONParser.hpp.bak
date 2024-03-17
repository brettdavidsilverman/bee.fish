#ifndef BEE_FISH__JSON_PARSER__HPP
#define BEE_FISH__JSON_PARSER__HPP
#include <vector>
#include <boost/json/basic_parser.hpp>
#include "../Parser/Parser.hpp"
#include "../PowerEncoding/PowerEncoding.hpp"
#include "../Database/Database.hpp"
#include "JSONPath.hpp"

using namespace BeeFishParser;
using namespace BeeFishDatabase;
using namespace BeeFishJSON;
using namespace BeeFishPowerEncoding;

namespace BeeFishDBServer {

   class JSONDBHandler
   {
   protected:
      struct JSONStackValue {
         Type _type;
         Size _index;
         Path _path;
      };

      Path _rootPath;
      vector<JSONStackValue> _stack;
      std::string _string;
      std::string _key;
      std::string _number;
      
   public:

      JSONDBHandler(Path path) :
         _rootPath(path)
      {
      }

         
      JSONStackValue& lastValue() {
         assert(size() > 0);
         return _stack[size() - 1];
      }

      Type& lastType() {
         return lastValue()._type;
      }

      Size& lastIndex() {
         return lastValue()._index;
      }

      Path& lastPath() {
         return lastValue()._path;
      }

      Size size() {
         return _stack.size();
      }

      bool lastIsNumber() {
         return isNumber(lastType());
      }

      bool isNumber(Type type) {
         return 
            (type == Type::NUMBER);

     //       ( (type == Type::INT64) ||
     //         (type == Type::UINT64) ||
     //         (type == Type::DOUBLE) );
      }

      void push_back(JSONStackValue value) {
         _stack.push_back(value);
      }

      void push_back(Type type)
      {
cerr << "JSONPATSER.HPP: " << type << endl;
         Path path = lastPath();

         if (lastType() == Type::ARRAY)
            path = path[lastIndex()++];

         path = path[type];
         
         _stack.push_back(
            {
               type,
               0,
               path
            }
         );
      }

      void pop_back(const string& value) {

         assert(size() > 0);
         
         
         Path
            path = lastPath();
         
         _stack.pop_back();

         if (lastType() == Type::KEY)
         {
            _stack.pop_back();

            assert(lastType() == Type::OBJECT);

         }

         path.setData(value);


      }

      void pop_back(const size_t value) {
         stringstream stream;
         stream << value;
         pop_back(stream.str());
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
            JSONStackValue {Type::ROOT, 0, _rootPath}
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
         assert(lastType() == Type::ROOT);
         _stack.pop_back();
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
            Type::ARRAY
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

         pop_back(n);
         
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
            Type::OBJECT
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
         
         pop_back(n);

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
         _key += string(s);
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
         std::string string(s);
         _key += string;

         if (_key.length() < n)
            return true;

         assert(_key.length() == n);
         
         JSONPath path = lastPath();

         path = path[_key];

         push_back(
            {
               Type::KEY,
               0,
               path
            }
         );

         _key.clear();
         
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
         _number += string(s);
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
         _string += std::string(s);
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
         _string += std::string(s);
         
         assert(_string.length() == n);
         
         push_back(
            Type::STRING
         );

         pop_back(_string);
         
         _string.clear();

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
         _number += string(s);
         
         push_back(
            Type::NUMBER
         );

         pop_back(_number);
         
         _number.clear();

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

         _number += string(s);
         
         push_back(
            Type::NUMBER
         );

         pop_back(_number);
         
         _number.clear();
         
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
 
         _number += string(s);
         
         push_back(
            Type::NUMBER
         );

         pop_back(_number);
         
         _number.clear();
         
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
         push_back(
            Type::BOOLEAN
         );

         string boolean;

         if (b)
            boolean = "true";
         else
            boolean = "false";

         pop_back(boolean);

         return true;
      }

      /// Called when a null is parsed.
      ///
      /// @return `true` on success.
      /// @param ec Set to the error, if any occurred.
      ///
      bool on_null( error_code& ec )
      {
         push_back(
            Type::NULL_
         );

         pop_back(string("null"));

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
      Path _rootPath;
      std::string _buffer = std::string(getpagesize(), '\0');
      const long _contentLength;
      Size _bytesWritten {0};
      Size _position = 0;
      const Size _pageSize = getpagesize();

      JSONParser(const JSONParser& source) :
         Parser(),
         _parser(boost::json::parse_options(), source._rootPath),
         _rootPath(source._rootPath),
         _contentLength(source._contentLength)
      {
      }

   public:

      JSONParser(
         Path path,
         long contentLength = -1
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

         Parser::read(c);

         _buffer[_position++] = c;

         if ( (_position == _pageSize) ||
              (_byteCount >= _contentLength &&
               _contentLength > 0) )
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

         bool more;
         if (_contentLength > 0)
            more = (_byteCount < _contentLength);
         else
            more = true;

         Size n =
            _parser.write_some(
               more,
               _buffer.data(),
               _position,
                ec
            );

         _bytesWritten += n;

         _position = 0;

         if (ec && (ec != boost::json::error::incomplete)) {
            setResult(false);
            return false;
         }
         else if (_parser.done()) {
            setResult(true);
         }
         else if (!more)
         {
            // Numbers can continue,
            // So the parser is never
            // done() even though we
            // are past end of content
            // length
            
            if ( _parser.handler()
                 .lastIsNumber() )
            {
               setResult(true);
            }
            else {
               setResult(false);
               return false;
            }
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