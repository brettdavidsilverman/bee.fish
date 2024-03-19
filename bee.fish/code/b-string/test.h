#ifndef BEE_FISH_B_STRING__TEST_H
#define BEE_FISH_B_STRING__TEST_H

#include "string.h"
#include "../test/test-result.h"

using namespace BeeFishTest;

namespace BeeFishBString
{
   inline bool testCharacters();
   inline bool testBStrings();
   inline bool testBitStreams();
   inline bool testBStringStreams();
   inline bool testSplit();
   inline bool testTrim();
   inline bool testHex();
   inline bool testData();
   inline bool testEncodeURI();
   inline bool testEmojis();
   
   
   inline bool test()
   {
   
      bool ok = true;
     
      ok &= testCharacters();
      ok &= testBStrings();
      ok &= testBitStreams();
      ok &= testBStringStreams();
      
      ok &= testSplit();
      ok &= testTrim();
      ok &= testHex();
      ok &= testData();
      ok &= testEncodeURI();
      ok &= testEmojis();
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   
   inline bool testCharacters()
   {
      cout << "Characters" << endl;
      
      for (char c = 0; c <= 10; c++) {
         BitStream stream;
         stream << Character(c);
         cout << "Character: " << (int)c << '\t' << stream.bstr() << endl;
      }

      bool ok = true;
      
      cout << "Zero Character" << endl;
      Character zero = 0;

      ok &= testResult(
         "Zero character",
         zero == Character{0}
      );

      Character zeroValue{0};
      ok &= testResult(
         "Zero character value",
         zeroValue == 0
      );


      Character c = 'a';
      Character value = c;
      ok &= testResult(
         "From value to character and back",
         ( value == 'a' )
      );
      
      Character b = c;
      ok &= testResult(
          "Copy and compare character",
          ( b == c )
      );
      
      Character d = 'z';
      d = b;
      ok &= testResult(
          "Assign and compare character",
          ( d == b )
      );
      
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testBStrings()
   {
      cout << "B-Strings" << endl;
      
      bool ok = true;
      
      BString start = "Hello World";
      BString next = start;
      BString compare = "Hello World";
      
      ok &= testResult(
         "B-String compare",
         (next == compare)
      );
      
      Character at = Character(0x0040);
      BString value = at;

      ok &= testResult(
         "B-String character @",
         (value == Character('@'))
      );

      BString bstr2 = "from Bee";
      BString bstr = "Hello " + bstr2;

      ok &= testResult(
         "Const char + bstr",
         (bstr == "Hello from Bee")
      );

      ok &= testResult(
         "Ends With true",
         BString("String").endsWith("ing")
      );

      ok &= testResult(
         "Ends With false",
         !BString("String").endsWith("nng")
      );

      ok &= testResult(
         "Ends With too large",
         !BString("String").endsWith("MyString")
      );

      ok &= testResult(
         "Compare two equal strings",
         BString("String") == "String"
      );

      ok &= testResult(
         "Compare two unequal strings",
         BString("String") != "unequal"
      );

      cout << endl;

      return ok;
   }
   
   inline bool testBitStreams() {

      cout << "BitStreams" << endl;

      bool ok = true;

      BitStream characterStream;
      characterStream << Character(1);

      ok &= testResult(
         "Character One stream",
         characterStream.count() == 0
      );

      characterStream.reset();
      Character readCharacter;
      characterStream >> readCharacter;

      ok &= testResult(
         "Character Read One stream",
         readCharacter == 1
      );

      BitStream characterManyStream;
      for (char c = 'a'; c <= 'z'; c++) {
         characterManyStream << Character(c);
      }

      ok &= testResult(
         "Character many stream",
         characterManyStream.count() == 0
      );

      characterManyStream.reset();

      Character compare = 'a';

      bool compares = true;

      while (characterManyStream.peekBit()) {
         Character readCharacter;
         characterManyStream >> readCharacter;
         if (readCharacter != compare) {
            compares = false;
            break;            
         }
         ++compare;
      }
      ok &= testResult(
         "Read characters",
         compares
      );

      BString bstring = "Hello World";
      BitStream stream;
      stream << bstring;

      ok &= testResult(
         "BitStream count",
         stream.count() == 0
      );

      BString bstring2;
      stream.reset();
      stream >> bstring2;

      ok &= testResult(
         "BitStream count 2",
         stream.count() == 0
      );

      ok &= testResult(
         "BString compare",
         bstring == bstring2
      );

      BitStream stream2;
      stream2 << "Hello World";
      Data data = stream2.toData();
      stream2.reset();
      BitStream stream2Compare = BitStream::fromData(data);
      
      ok &= testResult(
         "BitStreams compare",
         stream2 == stream2Compare
      );

      cout << endl;

      return ok;
   }

   inline bool testBStringStreams()
   {
      cout << "B-String-Streams" << endl;
      
      bool ok = true;
      
      BStream stream;
      BString value;
      stream._onbuffer = [&value](const Data& buffer) {
         std::string string((const char*)buffer._data, buffer.size());
         value = string;
      };

      stream << "Hello World";

      stream.flush();

      ok &= testResult(
         "B-String stream",
         (value == "Hello World")
      );

      value = "";
      stream << 101;
      stream.flush();
      ok &= testResult(
         "B-String stream int",
         value == "101"
      );

      cout << endl;

      return ok;
      
   }

   inline bool testSplit()
   {
      cout << "Split" << endl;
      
      bool ok = true;
      cout << "Here 0" << endl;
      BString test = "a:b:c";
      cout << "Here 1" << endl;
      
      vector<BString> split = test.split(':');
      cout << "Here 2" << endl;
      ok &= testResult(
         "Split abc",
            (split.size() == 3) &&
            (split[0] == "a")   &&
            (split[1] == "b")   &&
            (split[2] == "c")
      );
         
      cout << endl;
      
      return ok;
   }
   
   
   inline bool testTrim()
   {
      cout << "Trim" << endl;
      
      bool ok = true;
      
      BString sample = "   Hello World   ";
      
      ok &= testResult(
         "L-Trim",
         (sample.ltrim() == "Hello World   ")
      );
      
      ok &= testResult(
         "R-Trim",
         (sample.rtrim() == "   Hello World")
      );
      
      ok &= testResult(
         "Trim",
         (sample.trim() == "Hello World")
      );
      
      cout << endl;
    
      return ok;
   }
   
   inline bool testHex()
   {
      cout << "Hex" << endl;
      
      bool ok = true;
      
      Data start = "Hello World";
      BString hex = start.toHex();
      cout << "Hello World hex: " << hex << endl;
      ok &= testResult(
         "From Data to hex",
         (hex.size())
      );
      
#ifdef SERVER
      const int SESSION_ID_SIZE = 32;
      
      Data random =
         Data::fromRandom(
            SESSION_ID_SIZE
         );
        
      BString randomHex = random.toHex();
      
      cout << "Random hex: " << randomHex << endl;
      
      ok &= testResult(
         "Random hex",
         (random.size() == SESSION_ID_SIZE)
      );
            
#endif

      cout << endl;
      
      return ok;
   }
   
   inline Data testCopyData();

   inline bool testData()
   {
      cout << "Data" << endl;
      
      bool ok = true;
     
     
      unsigned long ulong = 101;
      Data ulongData(ulong);
      unsigned long ulongCompare =
         (unsigned long)ulongData;
      ok &= testResult(
         "From unsigned long to data and back",
         ulongCompare == ulong
      );
      
      // BString "ᛒᚢᛞᛖ"
      BString bstring = "Hello World";
      BitStream stream;
      stream << bstring;

      Data dataFromBString = bstring.toData();

      BString bstring2 = BString::fromData(dataFromBString);

      ok &= testResult(
         "From b-string to data and back",
         bstring == bstring2
      );
      
      Data dataStart = "Hello World";
      BString base64 = dataStart.toBase64();
      Data dataEnd = Data::fromBase64(base64);
      ok &= testResult(
         "From data to base64 and back",
         (dataStart == dataEnd)
      );
   
   #ifdef SERVER
      std::string stringMd5 = "Hello World";
      Data md5data = stringMd5;
      BString md5hash = md5data.md5();

      ok &= testResult(
         "Compare md5 hash",
         (md5hash == "b10a8db164e0754105b7a99be72e3fe5")
      );
   #endif
   
      Data copy = testCopyData();
      char buffer[copy.size()];
      memset(buffer, 0, sizeof(buffer));
      memcpy(buffer, copy.data(), copy.size());
      std::string string = buffer;
      cout << "STRING: " << string << endl;
      ok &= testResult(
         "Test Copy Data",
         string == "Hello World"
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline Data testCopyData() {
      Data data = Data::create();
      const char* string = "Hello World";
      memcpy(data.data(), string, strlen(string));
      Data data2;
      data2 = data;
      return data2;
   }

   inline bool testEncodeURI()
   {
      cout << "Encode URI" << endl;
      
      bool ok = true;
      
      BString string = "Emoji 😀";
      BString uri = string.encodeURI();

      ok &= testResult(
         "Encode URI 😀",
         (uri == "Emoji%20%F0%9F%98%80")
      );


      BString decodedURI = uri.decodeURI();
      ok &= testResult(
         "Decode URI 😀",
         (decodedURI == string)
      );

      cout << endl;
      
      return ok;
   }

   inline bool testEmojis()
   {
      cout << "Emojis" << endl;
      
      bool ok = true;
      
      std::string str = "Emoji 😀";
      BString bstr(str);
      std::string str2 = bstr;
      ok &= testResult(
         "Emoji 😀",
         (str == str2)
      );
      
      cout << endl;
      
      return ok;
   }

      
}

#endif
