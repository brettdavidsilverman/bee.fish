#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "config.h"
#include "b-string.h"
#include "test.h"
#include "version.h"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishBString;

int main(int argc, const char* argv[]) {
/*
    BString heart = "❤️";
    Index pos = 0;
    
    BString part1 = heart.nextUTF8(pos);
    BString part2 = heart.nextUTF8(pos);
    
    cout << part1.toHex() << endl;
    cout << part2.toHex() << endl;
    
    if (part1.isEmoji())
        cout << "1 😀😀😀😀" << endl;
    else
        cout << "1 😭😭😭😭" << endl;
        
    if (part2.isEmoji())
        cout << "2 😀😀😀😀" << endl;
    else
        cout << "2 😭😭😭😭" << endl;
        
    assert(false);
    */
    cout << "bee.fish.b-string"
         << endl
         << "C++ run time: "
         << __cplusplus
         << endl
         << "Version: "
         << BEE_FISH_B_STRING_VERSION
         << endl;

    bool test =
        (hasArg(argc, argv, "-test") != -1);

    if (test)
    {
        cout << "Testing B-Strings" << endl;
        if (BeeFishBString::test())
            return 0;

        return 1;
    }

    cout << "Enter string" << endl;

    while (!cin.eof())
    {

        BString line;
        getline(cin, line);

        if (line == "")
            break;

        cout << line << endl;
#ifdef SERVER
        cout << line.sha3() << endl;
#endif

    }

    cout << "Bye" << endl;



    return 0;
}
