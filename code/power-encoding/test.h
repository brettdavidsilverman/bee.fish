#include <iostream>
#include "../test/test.h"
#include "power-encoding-base.h"
#include "../b-string/b-string.h"
#include "../b-string/misc.h"
#include "encoding.h"
#include "stack.h"

using namespace std;
using namespace BeeFishTest;
using namespace BeeFishBString;

namespace BeeFishPowerEncoding
{

inline bool testStack();

inline bool test() {

    bool ok = true;

    std::stringstream stream1;
    EncodeToStream encodingOut(stream1, cout);

    for (int i = 0; i < 10; i++)
    {
        cout << i << " ";
        encodingOut << (unsigned char)i;
        cout << endl;
    }

    cout << "Count: " << encodingOut.count() << endl;
    ok &= testResult("Number count", encodingOut.count() == 0);

    stringstream strstream;
    EncodeToStream encoding(strstream, strstream);
    BString test;

    encoding << BString("Hello Bee");

    ok &= testResult(
              "Hello Bee write Count",
              ( encoding.count() == 0 )
          );

    encoding.reset();

    encoding >> test;

    cout << "Encoding count: " << encoding.count() << endl;

    ok &= testResult(
              "Hello Bee",
              ( test == "Hello Bee" )
          );

    ok &= testResult(
              "Hello Bee read Count",
              ( encoding.count() == 0 )
          );


    test.clear();

    encoding << BString("🍄");
    encoding >> test;
    ok &= testResult(
              "String 🍄",
              ( test == "🍄" ) &&
              ( encoding.count() == 0 )
          );

    encoding << 8;
    int i = 0;
    encoding >> i;

    ok &= testResult(
              "Number 8",
              ( i == 8 ) &&
              ( encoding.count() == 0 )
          );

    bool testBool;

    encoding << true;
    encoding >> testBool;
    ok &= testResult(
              "True",
              testBool == true
          );


    encoding << false;
    encoding >> testBool;
    ok &= testResult(
              "False",
              testBool == false
          );

    
    ok = ok && testStack();
    
    if (ok)
        cout << endl << "SUCCESS" << endl;
    else
        cout << endl << "FAIL" << endl;


    
    return ok;

}

inline bool testStack() {

    cout << "Stack" << endl;

    bool ok = true;
    {
        BString start = "ABC Hello World";
    
        Stack stream;
        stream << start;
        stream.reset();
        BString finish;
        stream >> finish;
    
        ok &= testResult(
            "Stack start-finish",
            start == finish
        );
    }
          

    Stack characterStream;
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

    Stack characterManyStream;
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
    Stack stream;
    stream << bstring;

    ok &= testResult(
              "Stack count",
              stream.count() == 0
          );

    BString bstring2;
    stream.reset();
    stream >> bstring2;

    ok &= testResult(
              "Stack count 2",
              stream.count() == 0
          );

    ok &= testResult(
              "BString compare",
              bstring == bstring2
          );

    Stack stream2;
    stream2 << bstring;
    std::string data = stream2.toData();
    stream2.reset();
    Stack stream2Compare = Stack::fromData(data);

    ok &= testResult(
              "BitStreams compare",
              stream2 == stream2Compare
          );

    BeeFishMisc::outputSuccess(ok);

    assert(ok);
    cout << endl;

    return ok;
}


}