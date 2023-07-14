#ifndef BEE_FISH__JSON_PARSER__HPP
#define BEE_FISH__JSON_PARSER__HPP

#include <boost/json.hpp>

#include "../Parser/Parser.hpp"
#include "../JSON/JSON.hpp"

using namespace BeeFishParser;

namespace BeeFishWebDB {

   class JSONParser : public Parser
   {
   };

}

struct handler
{
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
    bool on_document_begin( error_code& ec );

    /// Called when the JSON parsing is done.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_document_end( error_code& ec );

    /// Called when the beginning of an array is encountered.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_array_begin( error_code& ec );

    /// Called when the end of the current array is encountered.
    ///
    /// @return `true` on success.
    /// @param n The number of elements in the array.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_array_end( std::size_t n, error_code& ec );

    /// Called when the beginning of an object is encountered.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_object_begin( error_code& ec );

    /// Called when the end of the current object is encountered.
    ///
    /// @return `true` on success.
    /// @param n The number of elements in the object.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_object_end( std::size_t n, error_code& ec );

    /// Called with characters corresponding to part of the current string.
    ///
    /// @return `true` on success.
    /// @param s The partial characters
    /// @param n The total size of the string thus far
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_string_part( string_view s, std::size_t n, error_code& ec );

    /// Called with the last characters corresponding to the current string.
    ///
    /// @return `true` on success.
    /// @param s The remaining characters
    /// @param n The total size of the string
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_string( string_view s, std::size_t n, error_code& ec );

    /// Called with characters corresponding to part of the current key.
    ///
    /// @return `true` on success.
    /// @param s The partial characters
    /// @param n The total size of the key thus far
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_key_part( string_view s, std::size_t n, error_code& ec );

    /// Called with the last characters corresponding to the current key.
    ///
    /// @return `true` on success.
    /// @param s The remaining characters
    /// @param n The total size of the key
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_key( string_view s, std::size_t n, error_code& ec );

    /// Called with the characters corresponding to part of the current number.
    ///
    /// @return `true` on success.
    /// @param s The partial characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_number_part( string_view s, error_code& ec );

    /// Called when a signed integer is parsed.
    ///
    /// @return `true` on success.
    /// @param i The value
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_int64( int64_t i, string_view s, error_code& ec );

    /// Called when an unsigend integer is parsed.
    ///
    /// @return `true` on success.
    /// @param u The value
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_uint64( uint64_t u, string_view s, error_code& ec );

    /// Called when a double is parsed.
    ///
    /// @return `true` on success.
    /// @param d The value
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_double( double d, string_view s, error_code& ec );

    /// Called when a boolean is parsed.
    ///
    /// @return `true` on success.
    /// @param b The value
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_bool( bool b, error_code& ec );

    /// Called when a null is parsed.
    ///
    /// @return `true` on success.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_null( error_code& ec );

    /// Called with characters corresponding to part of the current comment.
    ///
    /// @return `true` on success.
    /// @param s The partial characters.
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_comment_part( string_view s, error_code& ec );

    /// Called with the last characters corresponding to the current comment.
    ///
    /// @return `true` on success.
    /// @param s The remaining characters
    /// @param ec Set to the error, if any occurred.
    ///
    bool on_comment( string_view s, error_code& ec );
};

#endif