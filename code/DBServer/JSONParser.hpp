#ifndef BEE_FISH__JSON_PARSER__HPP
#define BEE_FISH__JSON_PARSER__HPP
#include <vector>
#include <boost/json/basic_parser.hpp>
#include "../Parser/Parser.hpp"
#include "../Database/Database.hpp"

using namespace BeeFishParser;
using namespace BeeFishDatabase;

namespace BeeFishWebDB {

   enum class JSONType {
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

   ostream& operator << (ostream& out, const JSONType& type);
   
   struct JSONValue {
      Size     _size;
      JSONType _type;
      char     _data[];
   };

   class JSONDBHandler
   {
   public:
      const int OBJECT_INDEXES = 0;
      const int OBJECT_KEYS = 1;

   protected:
      struct JSONStackValue {
         JSONType _type;
         size_t _arrayIndex;
         string _key;
         Path<Database::Encoding> _path;
      };

      Path<Database::Encoding> _rootPath;
      vector<JSONStackValue> _stack;
      size_t _count {0};
      JSONType _lastType = (JSONType)-1;

   public:

      JSONDBHandler(Path<Database::Encoding> path) :
         _rootPath(path)
      {
      }

         
      JSONStackValue& lastValue() {
         assert(size() > 0);
         return _stack[size() - 1];
      }

      JSONType& lastType() {
         return lastValue()._type;
      }

      size_t& lastArrayIndex() {
         return lastValue()._arrayIndex;
      }

      const std::string& lastKey() {
         return lastValue()._key;
      }

      BeeFishWeb::Path lastPath() {
         return lastValue()._path;
      }

      size_t size() {
         return _stack.size();
      }

      bool lastIsNumber() {
         return isNumber(_lastType);
      }

      bool isNumber(JSONType type) {
         return
            ( (type == JSONType::INT64) ||
              (type == JSONType::UINT64) ||
              (type == JSONType::DOUBLE) );
      }

      void push_back(JSONStackValue value) {
         ++_count;
         _stack.push_back(value);
      }

      JSONStackValue pop_back()
      {
         assert(size() > 0);

         JSONStackValue value =
            _stack[size() - 1];

         _stack.pop_back();

         --_count;
         return value;
      }

      void setData(JSONType type, Size size = 0, void* value = nullptr) {

         Path<Database::Encoding> path
            = lastPath();
         
         JSONType _lastType = lastType();

         if (_lastType == JSONType::KEY) {
            string key = lastKey();
            pop_back();
            assert(lastType() == JSONType::OBJECT);
            path = path[key];
         }
         else if (_lastType == JSONType::ARRAY) {
            Size& index = lastArrayIndex();
            path = path[index++];
         }

         Size totalSize = sizeof(JSONValue) + size;

         Data* data =
            path.createData(totalSize);

         JSONValue* jsonValue = (JSONValue*) data->data();
         jsonValue->_size = size;
         jsonValue->_type = type;

         if (size > 0 && value)
            memcpy(&(jsonValue->_data[0]), value, size);


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
            JSONStackValue {JSONType::ROOT, 0, "", _rootPath}
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
            {JSONType::ARRAY, 0, "", lastPath()}
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
         assert(lastType() == JSONType::ARRAY);
         pop_back();
         setData(JSONType::ARRAY, sizeof(n), &n);
         
         return true;
      }

      /// Called when the beginning of an object is encountered.
      ///
      /// @return `true` on success.
      /// @param ec Set to the error, if any occurred.
      ///
      bool on_object_begin( error_code& ec )
      {
         Path path = lastPath();

         push_back(
            {JSONType::OBJECT, 0, "", path}
         );

         //setData(JSONType::OBJECT);

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
         assert(lastType() == JSONType::OBJECT);
         
         pop_back();

         setData(JSONType::OBJECT, sizeof(n), &n);
         
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
            {JSONType::KEY, 0, key, lastPath()}
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

         setData(JSONType::STRING, data.size(), data.data());

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
         setData(JSONType::INT64, sizeof(i), &i);
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
         setData(JSONType::UINT64, sizeof(u), &u);
 
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
         setData(JSONType::DOUBLE, sizeof(d), &d);

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
         setData(JSONType::BOOL, sizeof(b), &b);

         return true;
      }

      /// Called when a null is parsed.
      ///
      /// @return `true` on success.
      /// @param ec Set to the error, if any occurred.
      ///
      bool on_null( error_code& ec )
      {
         setData(JSONType::_NULL, 0, nullptr);

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
      const size_t _contentLength;
      size_t _bytesRead = 0;
      size_t _bytesWritten {0};
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
         Path<Database::Encoding> path,
         size_t contentLength
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

         size_t n =
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

   ostream& operator << (ostream& out, const JSONType& type)
   {
      switch(type) {
         case JSONType::INT64:
            out << "INT64";
            break;
         case JSONType::UINT64:
            out << "UINT64";
            break;
         case JSONType::DOUBLE:
            out << "DOUBLE";
            break;
         case JSONType::BOOL:
            out << "BOOL";
            break;
         case JSONType::_NULL:
            out << "_NULL";
            break;
         case JSONType::STRING:
            out << "STRING";
            break;
         case JSONType::OBJECT:
            out << "OBJECT";
            break;
         case JSONType::ARRAY:
            out << "ARRAY";
            break;
         case JSONType::KEY:
            out << "KEY";
            break;
         case JSONType::ROOT:
            out << "ROOT";
            break;
         default:
            out << "<Unknown type: " << (int)type << ">";
      };

      return out;
   }
   
   PowerEncoding& operator <<
   ( 
      PowerEncoding& stream,
      const JSONType& data
   )
   {
      stream << (int)data;
      return stream;
   }

}



#endif