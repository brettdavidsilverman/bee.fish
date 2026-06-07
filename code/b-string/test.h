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
inline bool testStack();
inline bool testBStringStreams();
inline bool testSplit();
inline bool testTrim();
inline bool testSubstr();
inline bool testHex();
inline bool testData();
inline bool testEncodeURI();
inline bool testEmojis();
inline bool testPunctuation();
inline bool testIsSpace();
inline bool testUTF8();
inline bool testTokenise();
inline bool testEscape();

inline bool test()
{
    BString string;
    //return true;
    bool ok = true;

    ok = ok && testOptional();
    ok = ok && testCharacters();
    ok = ok && testBStrings();
    ok = ok && testBStringStreams();

    ok = ok && testSplit();
    ok = ok && testTrim();
    ok = ok && testSubstr();
    ok = ok && testHex();
    ok = ok && testData();
    ok = ok && testEncodeURI();
    ok = ok && testEmojis();
    ok = ok && testUTF8();
    ok = ok && testPunctuation();
    ok = ok && testIsSpace();
    ok = ok && testTokenise();
    ok = ok && testEscape();

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
        Stack stream;
        stream << c;
        cout << "Char: " << (int)c << '\t' << stream.str() << endl;
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

inline bool testSubstr()
{
    cout << "Substr" << endl;

    bool ok = true;

    BString sample = "0123456789";
    
    ok = ok && testResult(
        "substr(0, 0)",
        sample.substr(0, 0) == ""
    );
    
    ok = ok && testResult(
        "substr(0, 1)",
        sample.substr(0, 1) == "0"
    );
    
    ok = ok && testResult(
        "substr(0)",
        sample.substr(0) == "0123456789"
    );
    
    ok = ok && testResult(
        "substr(1, 1)",
        sample.substr(1, 1) == "1"
    );
    
    ok = ok && testResult(
        "substr(9, 1)",
        sample.substr(9, 1) == "9"
    );
    
    cout << endl;
    
    
    return ok;
  
}

inline bool testHex()
{
    cout << "Hex" << endl;

    bool ok = true;

    BString start = "Hello World";
    BString hex = start.toHex();

    ok &= testResult(
              "From Data to hex",
              (hex.size() == 22)
          );

    BString endData = hex.fromHex();

    ok &= testResult(
              "From hex to data",
              (endData == start)
          );


    const int SESSION_ID_SIZE = 32;

    BString random = BString::createRandom(SESSION_ID_SIZE);

    cout << "\tRandom string: " << random << endl;

    ok &= testResult(
              "Random string",
              (random.size() == SESSION_ID_SIZE)
          );


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

    BString dataStart = "Hello World";
    BString base64 = dataStart.toBase64();
    BString dataEnd = base64.fromBase64();

    ok &= testResult(
              "From data to base64 and back",
              (dataStart == dataEnd)
          );
    assert(ok);

    std::string data;

    BeeFishMisc::PagedStream output(
        [&data](const std::string& encoded)
    {
        data = encoded;
    }
    );

    BeeFishMisc::Base64EncodeStream
    encoder(output);

    encoder << "a";

    ok = ok && testResult(
             "Encoder before flush",
             data == ""
         );

    encoder << flush;

    ok = ok && testResult(
             "Encoder with flush",
             data == "YQ=="
         );

    assert(ok);

#ifdef SERVER
    BString message = "Hello World";
    BString hash = message.sha3();
    cerr << "SHA-3 " << hash << endl;
    ok &= testResult(
              "Compare sha-3 hash",
              (hash == "e167f68d6563d75bb25f3aa49c29ef612d41352dc00606de7cbd630bb2665f51")
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


    BString string2 = "{braces}";
    BString uri2 = string2.encodeURI();

    ok &= testResult(
              "Encode {braces}",
              (uri2 == "{braces}")
          );


    BString decodedURI2 = uri2.decodeURI();

    ok &= testResult(
              "Decode {braces}",
              (decodedURI2 == string2)
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

    punc = "hello";
    ok = ok && testValue(
             "Multi char non punctuation",
             punc.isPunctuation() == false
         );

    punc = "&\"";
    ok = ok && testValue(
             "Multi char punctuation",
             punc.isPunctuation() == true
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
             space.isSpace() == false
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
    /*
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
    */
    return ok;
}

inline ostream& operator <<
(ostream& output, const vector<BString>& values)
{
    for (auto string : values)
        output << string << endl;

    return output;
}

inline bool testUTF8()
{
    cout << "Test utf-8" << endl;
    bool ok = true;
    auto testUTF8 =
    [](const BString& value, const BString& expected, BString& partWord, BString& partUTF8, bool isFinal)
    {
        
        Index position = 0;
        
        bool ok = true;
        while (position < value.size())
        {
            BString utf8 =
                value.nextUTF8(
                    position,
                    partUTF8
                );
                
            if (partUTF8.size() == 0)
                partWord += utf8;
        }
        
        ok = ok && testValue(value + " part word", partWord == expected);
    
        if (isFinal)
        {
            ok = ok && testValue(value + " part utf8 empty", partUTF8.size() == 0);
            
            if (ok) {
                partWord = "";
            }
        }
        
        if (!ok) {
            cout << "Expected \"" << expected << "\"" << endl
                << "Part utf-8 hex " << "\"" << partUTF8.toHex() << "\"" << endl
                << "Part word " << "\"" << partWord.encodeURI() << "\"" << endl;
        }

        return ok;
    };
    
    BString partWord;
    BString partUTF8;
    
    
    ok = ok && testUTF8("ASCII", "ASCII", partWord, partUTF8, true);
    ok = ok && testUTF8("😀❤️😭🤣", "😀❤️😭🤣", partWord, partUTF8, true);
    ok = ok && testUTF8("Full \xF0\x9F\x98\x80",  "Full 😀", partWord, partUTF8, true);
    ok = ok && testUTF8("Part \xF0", "Part ", partWord, partUTF8, false);
    ok = ok && testValue("First part UTF8", partUTF8 == "\xF0");
    ok = ok && testUTF8("\x9F\x98\x80", "Part 😀", partWord, partUTF8, true);
    
    assert(ok);
    
    return ok;
}

inline bool testTokenise()
{
    cout << "Test words" << endl;
    bool ok = true;

    BString text;


    std::vector<BString> words;

    text = "one.two.three";
    words = text.tokenise();

    ok = ok && testValue(
             text,
             words.size() == 4
         );

    if (!ok)
    {
        cout << words << endl;
        assert(false);
    }

    ok = ok && testValue(
             words[0],
             words[0] == "one"
         );

    ok = ok && testValue(
             words[1],
             words[1] == "one.two.three"
         );

    ok = ok && testValue(
             words[2],
             words[2] == "three"
         );

    ok = ok && testValue(
             words[3],
             words[3] == "two"
         );

    if (ok)
    {
        text = "https://bee.fish";
        words = text.tokenise();
    }

    ok = ok && testValue(
             text,
             words.size() == 4
         );
         
    if (!ok)
    {
        cout << words << endl;
        assert(false);
    }
    

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

    if (!ok)
    {
        cerr << words << endl;
        assert(false);
    }
    
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

inline bool testEscape()
{
    cout << "Test Escape" << endl;
    bool ok = true;

    stringstream stream;
    for (unsigned int i = 0; i < 128; ++i)
    {
        stream << (char)i;
    }

    BString data = stream.str();

    BString escaped = data.escape();

    BString unescaped = escaped.unescape();

    for (unsigned int i = 0; i < 128; ++i)
    {
        char c = unescaped[i];
        if ((unsigned char)c != i)
        {
            cerr << "Failed " << (char)i << " at position " << i << endl;
            ok = false;
            break;
        }
    }
    
    data = "❤️🌍🌈😭";
    escaped = data.escape();
    unescaped = escaped.unescape();
    
    ok = ok && testValue(
        data,
        unescaped == data
    );

    return ok;
}

}

#endif
