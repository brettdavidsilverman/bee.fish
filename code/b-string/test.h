#ifndef BEE_FISH_B_STRING__TEST_H
#define BEE_FISH_B_STRING__TEST_H

#include "b-string.h"
#include "../power-encoding/power-encoding.h"
#include "../test/test.h"

using namespace BeeFishTest;

namespace BeeFishBString
{
    inline bool testOptional();
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
    inline bool testPunctuation();
    inline bool testIsSpace();
    inline bool testWords();
    
    inline bool test()
    {
    
        bool ok = true;
      
        ok = ok && testOptional();
        ok = ok && testCharacters();
        ok = ok && testBStrings();
        ok = ok && testBitStreams();
        ok = ok && testBStringStreams();
        
        ok = ok && testSplit();
        ok = ok && testTrim();
        ok = ok && testHex();
        ok = ok && testData();
        ok = ok && testEncodeURI();
        ok = ok && testEmojis();
        ok = ok && testPunctuation();
        ok = ok && testIsSpace();
        ok = ok && testWords();
        
        
        outputSuccess(ok);
        
        return ok;
    }
    
    inline bool testOptional()
    {
        cout << "Optional" << endl;
      
        bool ok = true;
      
        optional<bool> result;
        
        result = true;
        ok &= testResult("true==true", (result == true) == true);
        ok &= testResult("true==false", (result == false) == false);
        ok &= testResult("true==nullopt", (result == nullopt) == false);
        
        result = false;
        ok &= testResult("false==true", (result == true) == false);
        ok &= testResult("false==false", (result == false) == true);
        ok &= testResult("false==nullopt", (result == nullopt) == false);
        
        result = nullopt;
        ok &= testResult("nullopt==true", (result == true) == false);
        ok &= testResult("nullopt==false", (result == false) == false);
        ok &= testResult("nullopt==nullopt", (result == nullopt) == true);
      
        
          
        result = true;
        ok &= testResult("true!=true", (result != true) == false);
        ok &= testResult("true!=false", (result != false) == true);
        ok &= testResult("true!=nullopt", (result != nullopt) == true);
        
        result = false;
        ok &= testResult("false!=true", (result != true) == true);
        ok &= testResult("false!=false", (result != false) == false);
        ok &= testResult("false!=nullopt", (result != nullopt) == true);
        
        result = nullopt;
        ok &= testResult("nullopt!=true", (result != true) == true);
        ok &= testResult("nullopt!=false", (result != false) == true);
        ok &= testResult("nullopt!=nullopt", (result != nullopt) == false);
     
     
        if (ok)
        {
             optional<bool> result;
             if (!(result != false))
                 ok = false;
                
        }
        
        BeeFishMisc::outputSuccess(ok);
        
        return ok;
    }
    
    inline bool testCharacters()
    {
        cout << "Characters" << endl;
        
        for (char c = 0; c < 10; c++) {
            BitStream stream;
            stream << c;
            cout << "Char: " << (int)c << '\t' << stream.bstr() << endl;
        }

        bool ok = true;
        
        
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
        
        Char at = "\u0040";
        BString value = at;

        ok &= testResult(
            "B-String character @",
            (value == Char('@'))
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


        BString mixedCase = "I 💜 Rock and roll";
        
        BString lowerCase = mixedCase.toLower();
        ok &= testResult(
            lowerCase,
            lowerCase == "i 💜 rock and roll"
        );
        
        ok &= testResult(
            "Original unchanged 1",
            mixedCase == "I 💜 Rock and roll"
        );
        
        BString upperCase = mixedCase.toUpper();
        ok &= testResult(
            upperCase,
            upperCase == "I 💜 ROCK AND ROLL"
        );
        
        ok &= testResult(
            "Original unchanged 2",
            mixedCase == "I 💜 Rock and roll"
        );
        cout << endl;
        
        BString digits = "0123";
        ok &= testResult(
             "Digits only",
             digits.isDigitsOnly()
        );
        
        digits += "a";
        ok &= testResult(
             "Digits with character",
             !digits.isDigitsOnly()
        );
        
        return ok;
    }
    
    inline bool testBitStreams() {

        cout << "BitStreams" << endl;

        bool ok = true;

        BitStream characterStream;
        characterStream << Char('1');

        ok &= testResult(
            "Char One stream",
            characterStream.count() == 0
        );

        characterStream.reset();
        Char readCharacter;
        characterStream >> readCharacter;

        ok &= testResult(
            "Char Read One stream",
            readCharacter == "1"
        );

        BitStream characterManyStream;
        for (char c = 'a'; c <= 'z'; c++) {
            Char character(c);
            characterManyStream << character;
        }

        ok &= testResult(
            "Char many stream",
            characterManyStream.count() == 0
        );

        characterManyStream.reset();

        char compare = 'a';

        bool compares = true;

        while (characterManyStream.peekBit()) {
            Char readCharacter;
            characterManyStream >> readCharacter;
            if (readCharacter[0] != compare) {
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
        stream2 << bstring;
        std::string data = stream2.toData();
        stream2.reset();
        BitStream stream2Compare = BitStream::fromData(data);
        
        ok &= testResult(
            "BitStreams compare",
            stream2 == stream2Compare
        );
      
        BeeFishMisc::outputSuccess(ok);
        
        assert(ok);
        cout << endl;

        return ok;
    }

    inline bool testBStringStreams()
    {
        cout << "B-String-Streams" << endl;
        
        bool ok = true;
        
        BStream stream;
        BString value;
        stream._onbuffer = [&value](const std::string& buffer) {
            value = buffer;
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
                (split[0] == "a")    &&
                (split[1] == "b")    &&
                (split[2] == "c")
        );
            
        cout << endl;
        
        return ok;
    }
    
    
    inline bool testTrim()
    {
        cout << "Trim" << endl;
        
        bool ok = true;
        
        BString sample = "    Hello World    ";
        
        ok &= testResult(
            "L-Trim",
            (sample.ltrim() == "Hello World    ")
        );
        
        ok &= testResult(
            "R-Trim",
            (sample.rtrim() == "    Hello World")
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
        
        std::string start = "Hello World";
        BString hex = toHex(start);
        
        ok &= testResult(
            "From Data to hex",
            (hex.size() == 22)
        );
        
        std::string endData = fromHex(hex);
        
        ok &= testResult(
            "From hex to data",
            (endData == start)
        );
        
#ifdef SERVER
        const int SESSION_ID_SIZE = 32;
        
        std::string random =
            createRandom(
                SESSION_ID_SIZE
            );
          
        BString randomHex = toHex(random);
        
        cout << "\tRandom hex: " << randomHex << endl;
        
        ok &= testResult(
            "Random hex",
            (random.size() == SESSION_ID_SIZE)
        );
                
#endif

        cout << endl;
        
        return ok;
    }
    
    inline bool testData()
    {
        cout << "Data" << endl;
        
        bool ok = true;
      
      
        unsigned long ulong = 101;
        std::string ulongData((char*)&ulong, sizeof ulong);
        unsigned long ulongCompare =
            *(unsigned long*)ulongData.data();
        ok &= testResult(
            "From unsigned long to data and back",
            ulongCompare == ulong
        );
        
        
        std::string dataStart = "Hello World";
        BString base64 = toBase64(dataStart);
        BString dataEnd = fromBase64(base64);
        ok &= testResult(
            "From data to base64 and back",
            (dataStart == dataEnd)
        );
    
    #ifdef SERVER
        std::string stringMd5 = "Hello World";
        BString md5hash = md5(stringMd5);
        cerr << "MD5 " << md5hash << endl;
        ok &= testResult(
            "Compare md5 hash",
            (md5hash == "b10a8db164e0754105b7a99be72e3fe5")
        );
    #endif
        
        cout << endl;
        
        return ok;
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
    
    inline bool testPunctuation()
    {
        cout << "Test punctuation" << endl;
        
        bool ok = true;
        
        BString punc = "";
        
        ok = ok && testValue(
            "Empty string",
            punc.isPunctuation() == false
        );
        
        punc = "*";
        ok = ok && testValue(
            "Single char punctuation",
            punc.isPunctuation() == true
        );
        
        punc = ".hello";
        ok = ok && testValue(
            "Multi char non punctuation",
            punc.isPunctuation() == false
        );
        
        punc = ".\"";
        ok = ok && testValue(
            "Multi char punctuation",
            punc.isPunctuation() == true
        );
        
        punc = "hello";
        ok = ok && testValue(
            "Multi char non punctuation",
            punc.isPunctuation() == false
        );
        
        return ok;
    }
    
    inline bool testIsSpace()
    {
        cout << "Test is space" << endl;
        
        bool ok = true;
        
        BString space = "";
        
        ok = ok && testValue(
            "Empty string",
            space.isSpace() == true
        );
        
        space  = " ";
        ok = ok && testValue(
            "Single char space",
            space.isSpace() == true
        );
        
        space = "  \t\n";
        ok = ok && testValue(
            "Multi char space",
            space.isSpace() == true
        );
        
        
        space  = "c";
        ok = ok && testValue(
            "Single char non space",
            space.isSpace() == false
        );
        
        space = "hello";
        ok = ok && testValue(
            "Multi char non space",
            space.isSpace() == false
        );
        
        space = "\u2003";
        ok = ok && testValue(
            "Single unicode char space 1",
            space.isSpace() == true
        );
        
        space = "\u2009";
        ok = ok && testValue(
            "Single unicode char space 2",
            space.isSpace() == true
        );
        
        space = "\u2003\u2009";
        ok = ok && testValue(
            "Multi unicode char space",
            space.isSpace() == true
        );
        
        return ok;
    }
    
    inline bool testWords()
    {
        cout << "Test words" << endl;
        bool ok = true;

        BString text;

        
        std::vector<BString> words;
        
        text = "one.two.three";
        words = text.tokenise();
        
        ok = ok && testValue(
            text,
            words.size() == 6
        );
        
        ok = ok && testValue(
            words[0],
            words[0] == "one"
        );
        
        ok = ok && testValue(
            words[1],
            words[1] == "one.two"
        );
        
        ok = ok && testValue(
            words[2],
            words[2] == "one.two.three"
        );
        
        ok = ok && testValue(
            words[3],
            words[3] == "three"
        );
        
        ok = ok && testValue(
            words[4],
            words[4] == "two"
        );
        
        ok = ok && testValue(
            words[5],
            words[5] == "two.three"
        );
        
        
        if (ok)
        {
            text = "https://bee.fish";
            words = text.tokenise();
        }
        
        ok = ok && testValue(
            text,
            words.size() == 6
        );
        
        ok = ok && testValue(
            words[0],
            words[0] == "bee"
        );
        
        ok = ok && testValue(
            words[1],
            words[1] == "bee.fish"
        );
        
        ok = ok && testValue(
            words[2],
            words[2] == "fish"
        );
        
        ok = ok && testValue(
            words[3],
            words[3] == "https"
        );
        
        ok = ok && testValue(
            words[4],
            words[4] == "https://bee"
        );
        
        ok = ok && testValue(
            words[5],
            words[5] == "https://bee.fish"
        );
        
        if (ok)
        {
            text = "Hello!";
            words = text.tokenise();
        }
        
        ok = ok && testValue(
            text,
            words.size() == 1
        );
        
        ok = ok && testValue(
            words[0],
            words[0] == "hello"
        );
        
        if (ok)
        {
            text = "file.txt";
            words = text.tokenise();
        }
        
        ok = ok && testValue(
            text,
            words.size() == 3
        );
        
        ok = ok && testValue(
            words[0],
            words[0] == "file"
        );
        
        ok = ok && testValue(
            words[1],
            words[1] == "file.txt"
        );
        
        ok = ok && testValue(
            words[2],
            words[2] == "txt"
        );
        
        if (ok)
        {
            text = "one two.three";
            words = text.tokenise();
        }

        ok = ok && testValue(
            text,
            words.size() == 4
        );
        
        ok = ok && testValue(
            words[0],
            words[0] == "one"
        );
        
        ok = ok && testValue(
            words[1],
            words[1] == "three"
        );
        
        ok = ok && testValue(
            words[2],
            words[2] == "two"
        );
        
        ok = ok && testValue(
            words[3],
            words[3] == "two.three"
        );
        
        if (ok)
        {
            text = "“one two.three”";
            words = text.tokenise();
        }

        ok = ok && testValue(
            text,
            words.size() == 4
        );
        
        ok = ok && testValue(
            words[0],
            words[0] == "one"
        );
        
        ok = ok && testValue(
            words[1],
            words[1] == "three"
        );
        
        ok = ok && testValue(
            words[2],
            words[2] == "two"
        );
        
        ok = ok && testValue(
            words[3],
            words[3] == "two.three"
        );
        
        if (ok)
        {
            text = "Hello World 🌎";
            words = text.tokenise();
        }
        
        ok = ok && testValue(
            text,
            words.size() == 3
        );
        
        ok = ok && testValue(
            words[0],
            words[0] == "hello"
        );
        
        ok = ok && testValue(
            words[1],
            words[1] == "world"
        );
        
        ok = ok && testValue(
            words[2],
            words[2] == "🌎"
        );
        
        if (ok)
        {
            text = " ";
            words = text.tokenise();
        }
        
        ok = ok && testValue(
            "[space]",
            words.size() == 0
        );
        
        if (ok)
        {
            text = "!";
            words = text.tokenise();
        }
        
        ok = ok && testValue(
            text,
            words.size() == 0
        );
        
        return ok;

    }

        
}

#endif
