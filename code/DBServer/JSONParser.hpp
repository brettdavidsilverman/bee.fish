#ifndef BEE_FISH__JSON_PARSER__HPP
#define BEE_FISH__JSON_PARSER__HPP
#include <vector>
#include <boost/json/basic_parser.hpp>
#include "../Parser/Parser.hpp"
#include "../Database/Database.hpp"
#include "Variable.hpp"
#include "JSONPath.hpp"

using namespace BeeFishParser;
using namespace BeeFishDatabase;
using namespace BeeFishJSON;

namespace BeeFishScript {
/*
   enum class BeeFishJSON::Type {
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
   
   PowerEncoding& operator <<
   ( 
      PowerEncoding& stream,
      const BeeFishJSON::Type& type
   )
   {
      stream << (int)type;
      return stream;
   }

   PowerEncoding& operator >>
   (
      PowerEncoding &output,
      BeeFishJSON::Type& type
   )
   {
      int read;
      output >> read;

      type = (BeeFishJSON::Type)read;

      return output;
   }

   ostream& operator << (ostream& out, const BeeFishJSON::Type& type);
   */
   /*
   struct JSONData {
      Size     _size;
      BeeFishJSON::Type _type;
      char     _data[];
      void* data()
      {
         assert(_size);
         return &(_data[0]);
      }
   };
*/
   class JSONDBHandler
   {
   protected:
      struct JSONStackValue {
         BeeFishJSON::Type _type;
         Size _index;
         String _key;
         BeeFishWeb::Path  _path;
      };

      BeeFishWeb::Path _rootPath;
      vector<JSONStackValue> _stack;
      
   public:

      JSONDBHandler(BeeFishWeb::Path path) :
         _rootPath(path)
      {
      }

         
      JSONStackValue& lastValue() {
         assert(size() > 0);
         return _stack[size() - 1];
      }

      BeeFishJSON::Type& lastType() {
         return lastValue()._type;
      }

      Size& lastIndex() {
         return lastValue()._index;
      }

      String& lastKey() {
         return lastValue()._key;
      }

      BeeFishWeb::Path& lastPath() {
         return lastValue()._path;
      }

      Size size() {
         return _stack.size();
      }

      bool lastIsNumber() {
         return isNumber(lastType());
      }

      bool isNumber(BeeFishJSON::Type type) {
         return 
            (type == BeeFishJSON::Type::NUMBER);

     //       ( (type == BeeFishJSON::Type::INT64) ||
     //         (type == BeeFishJSON::Type::UINT64) ||
     //         (type == BeeFishJSON::Type::DOUBLE) );
      }

      void push_back(JSONStackValue value) {
         _stack.push_back(value);
      }

      void push_back(BeeFishJSON::Type type)
      {
         Path path = lastPath();

         _stack.push_back(
            {
               type,
               0,
               "",
               path
            }
         );
      }

      void pop_back(const string& value, const BeeFishJSON::Type type) {

         assert(size() > 0);

         JSONStackValue last =
            _stack[size() - 1];

         _stack.pop_back();

         BeeFishWeb::Path
            path = lastPath();

         if (lastType() == BeeFishJSON::Type::ARRAY) {
            
            Size index =
               lastIndex()++;

            path << type << index;

         }
         else if (lastType() == BeeFishJSON::Type::KEY)
         {
            Size& index =
               lastIndex();

            path[JSONPath::OBJECT_TABLE]
                [index].setData(value);

            path[JSONPath::OBJECT_KEYS]
                [lastKey()].setData(value);

            ++index;

            _stack.pop_back();
         }
         else
            path << type;

         if (lastType() != BeeFishJSON::Type::KEY)
         {
            path.setData(value);
         }


      }

      void pop_back(const size_t value, const BeeFishJSON::Type type) {
         stringstream stream;
         stream << value;
         pop_back(stream.str(), type);
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
            JSONStackValue {BeeFishJSON::Type::ROOT, 0, "", _rootPath}
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
         assert(lastType() == BeeFishJSON::Type::ROOT);
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
            BeeFishJSON::Type::ARRAY
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

         pop_back(n, BeeFishJSON::Type::ARRAY);
         
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
            BeeFishJSON::Type::OBJECT
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
         
         pop_back(n, BeeFishJSON::Type::OBJECT);

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

         Path path = lastPath();

         push_back(
            {
               BeeFishJSON::Type::KEY,
               0,
               key,
               path
            }
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
         std::string string(s);

         push_back(
            BeeFishJSON::Type::STRING
         );

         pop_back(string, BeeFishJSON::Type::STRING);

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
         push_back(
            BeeFishJSON::Type::NUMBER
         );

         pop_back(string(s), BeeFishJSON::Type::NUMBER);

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

         push_back(
            BeeFishJSON::Type::NUMBER
         );

         pop_back(string(s), BeeFishJSON::Type::NUMBER);
 
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
 
         push_back(
            BeeFishJSON::Type::NUMBER
         );

         pop_back(string(s), BeeFishJSON::Type::NUMBER);

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
            BeeFishJSON::Type::BOOLEAN
         );

         string boolean;

         if (b)
            boolean = "true";
         else
            boolean = "false";

         pop_back(boolean, BeeFishJSON::Type::BOOLEAN);

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
            BeeFishJSON::Type::_NULL
         );

         pop_back(string("null"), BeeFishJSON::Type::_NULL);

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
      Path<Database::Encoding> _rootPath;
      std::string _buffer;
      const Size _contentLength;
      Size _bytesRead = 0;
      Size _bytesWritten {0};
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
         Path<Database::Encoding> path,
         Size contentLength
      ) :
         Parser(),
         _parser(boost::json::parse_options(), path),
         _rootPath(path),
         _contentLength(contentLength)
      {
         if (_contentLength <= 0)
            throw runtime_error("Invalid content length");
      }

      virtual ~JSONParser() {
      }

      virtual bool read(char c)
      override
      {
         ++_bytesRead;

         _buffer.push_back(c);
         if ( (_buffer.size() == _pageSize) ||
              (_bytesRead >= _contentLength) )
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

         bool more =
            (_bytesRead < _contentLength);

         Size n =
            _parser.write_some(
                more,
               _buffer.data(),
               _buffer.size(),
                ec
            );

         _bytesWritten += n;
         more = _bytesWritten < _contentLength;

         _buffer.clear();

         if (ec && (ec != boost::json::error::incomplete)) {
            setResult(false);
            return false;
         }
         else if (_parser.done()) {
            if (more) {
               setResult(false);
               return false;
            }
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