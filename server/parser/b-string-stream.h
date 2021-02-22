#ifndef BEE_FISH_PARSER__B_STRING_STREAM_H
#define BEE_FISH_PARSER__B_STRING_STREAM_H

#include <iostream>
#include <string>
#include "match.h"
#include "utf-8.h"
#include "b-string.h"

namespace bee::fish::parser {

   class BStringStream
   {
   private:
      stringstream _backingStream;
      wstringstream _backingWStream;
      
   protected:
      istream& _input;
      ostream& _output;
      
   public:
   
      BStringStream() :
         _input(_backingStream),
         _output(_backingStream)
      {
      }
          
      // utf-8 stream
      BStringStream(iostream& stream) :
         _input(stream),
         _output(stream)
      {
      }
      
      
      // utf-8 string 
      BStringStream(const std::string& string) :
         _backingStream(string),
         _input(_backingStream),
         _output(_backingStream)
      {
      }
      
      // output stream
      BStringStream(ostream& output) :
         _input(_backingStream),
         _output(output)
      {
      }
      
      // input stream
      BStringStream(istream& input) :
         _input(input),
         _output(_backingStream)
      {
      }
      
      // input / output streams
      BStringStream(
         istream& input,
         ostream& output
      ) :
         _input(input),
         _output(output)
      {
      }

      
      virtual bool eof()
      {
         return _input.eof();
      }

      istream& operator >> (Char& character)
      {
         UTF8Character temporary;
         char nextChar;
         while (_input.get(nextChar) &&
                temporary.match(nextChar) &&
                temporary.result() == nullopt)
         {
         }
        
         if (temporary.result())
            character = temporary.character();
         
         return _input;
      }
      
      
      
      friend BStringStream& operator <<
      (BStringStream& output, const Char& character)
      {
         UTF8Character::writeUTF8Character(
            output._output,
            character
         );
         
         return output;
      }
      
      friend BStringStream& operator <<
      (BStringStream& output, const string& string)
      {
         output._output << string;

         return output;
      }
      
      friend BStringStream& operator <<
      (BStringStream& output, const BString& bstring)
      {
         
         output._output << bstring;

         return output;
      }

   };
   
   
}

#endif


