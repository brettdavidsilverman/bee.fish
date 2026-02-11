#ifndef BEE_FISH__DATABASE__NULL_STREAM_HPP
#define BEE_FISH__DATABASE__NULL_STREAM_HPP

#include <iostream>
#include <streambuf>

namespace BeeFishDatabase {

    // 1. Define a custom stream buffer class (NullBuffer)
    class NullBuffer : public std::streambuf {
    public:
        // Override the virtual overflow function to do nothing
        // 'c' is the character to be output, cast to an int
        virtual int overflow(int c) {
            return c; // Simply return the character to indicate success, but don't output it
        }
        // Override xsputn for efficiency with larger blocks of data
        virtual std::streamsize xsputn(const char* s, std::streamsize n) {
            return n; // Report all characters as "successfully written"
        }
    };

    // 2. Define a custom ostream class that uses NullBuffer
    class NullStream : public std::ostream {
    public:
        NullStream() : std::ostream(&m_buffer) {}
    private:
        NullBuffer m_buffer;
    };

    // 3. Create a global instance for use
    inline static NullStream cnull;

// Example usage:
// cnull << "This message will be discarded." << std::endl;

}

#endif
