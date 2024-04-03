#ifndef BEE_FISH__JSON__TEST_H
#define BEE_FISH__JSON__TEST_H

#include "../test/test.h"
#include "../parser/test.h"
#include "json.h"
#include "json-parser.h"
#include "../Script/Type.hpp"

using namespace BeeFishParser;

namespace BeeFishJSON
{
   
   inline bool test();


   inline bool testIntrinsics();
   inline bool testNumbers(); 
   inline bool testStrings();
   inline bool testSets();
   inline bool testKeyedSets();
   inline bool testArrays();
   inline bool testObjects();
#ifdef SERVER
   inline bool testStreams();
   
#endif

   inline bool testEmojis();
   inline bool testTypes();
   
   inline bool test()
   {
      
      bool ok = true;

      ok &= testIntrinsics();
      ok &= testNumbers();
      ok &= testStrings();
      ok &= testSets();
      ok &= testArrays();
      ok &= testKeyedSets();
      ok &= testObjects();
#ifdef SERVER
      ok &= testStreams();
#endif
      ok &= testEmojis();
      ok &= testTypes();
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testIntrinsics()
   {
      cout << "Intrinsics" << endl;
 
      bool ok = true;
      
      ok &= testMatchDelete("Undefined", new Capture(new Undefined()), "undefined", true, "undefined");

      ok &= testMatchDelete("Undefined JSON", new Capture(new JSON()), "undefined", true, "undefined");
      
      ok &= testMatchDelete("True", new Capture(new JSON()), "true", true, "true");

      ok &= testMatchDelete("False", new Capture(new JSON()), "false", true, "false");
      
      ok &= testMatchDelete("Null", new Capture(new JSON()), "null", true, "null");

      ok &= testMatchDelete("False a", new Capture(new JSON()), "a");
      
      cout << endl;
      
      return ok;
   }
  
   inline bool testNumbers()
   {
      cout << "Numbers" << endl;
      
      bool ok = true;
      
      ok &= testMatchDelete("Capture",  new Capture(new Integer()), "80000", true, "80000");
      assert(ok);
      ok &= testMatchDelete("Integer", new Capture(new Integer()), "800", true, "800");
      ok &= testMatchDelete("Negative", new Capture(new Number()), "-800", true, "-800");
      ok &= testMatchDelete("Decimal", new Capture(new Number()), "800.01", true, "800.01");
      ok &= testMatchDelete("Short exponent", new Capture(new Number()), "800e10", true, "800e10");
      ok &= testMatchDelete("Full exponent", new Capture(new Number()), "800E-10", true, "800E-10");
      ok &= testMatchDelete("False positive", new Number(), "+800");
      //ok &= testMatchDelete("NaN", new CaptureNumber(), "NaN", true, "NaN");
      assert(ok);
      
      cout << endl;
      
      return ok;
   }    

   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;

      PlainCharacter plainCharacter;
      ok &= testMatch("Plain character", &plainCharacter, "a", true);
      ok &= testResult("Plain character value", (plainCharacter.character() == Char('a')));

      Hex hex;
      ok &= testMatch("Hex", &hex, "0040", true, "0040");
      ok &= testResult("Hex value", (hex.character() == Char('@')));
      
      EscapedCharacter backSlash;
      ok &= testMatch("Escaped character back slash", &backSlash, "\\\\", true);
      ok &= testResult("Escaped character back slash value", (backSlash.character() == "\\"));
      
      EscapedCharacter hexCharacter;
      ok &= testMatch("Escaped character hex", &hexCharacter, "\\u0040", true);
      ok &= testResult("Escaped character hex value", (hexCharacter.character() == Char('@')));

      StringCharacter stringCharacterPlain;
      ok &= testMatch("String character plain", &stringCharacterPlain, "a", true);
      ok &= testResult("String character plain value", (stringCharacterPlain.character() == Char('a')));

      StringCharacter stringCharacterEscaped;
      ok &= testMatch("String character escaped", &stringCharacterEscaped,  "\\u0040", true);
      ok &= testResult("String character escaped value", (stringCharacterEscaped.character() == Char('@')));
      
      StringCharacters stringCharacters;
      ok &= testMatch("String characters", &stringCharacters, "hello world\\\\", nullopt);
      ok &= testResult("String characters value", (stringCharacters == "hello world\\"));
      ok &= testMatchDelete("String", new String(), "\"hello world\"", true, "hello world");
      assert(ok);

      ok &= testMatchDelete("Empty string", new JSON(), "\"\"", true, "");
      ok &= testMatchDelete("Simple string", new JSON(), "\"hello\"", true, "hello");
      ok &= testMatchDelete("Unquoted", new JSON(), "hello", false);
      ok &= testMatchDelete("Single quote", new JSON(), "\"", nullopt);
      ok &= testMatchDelete("Escaped quote", new JSON(), "\"\\\"\"", true, "\"");

#ifdef SERVER  
      BeeFishJSON::String largeString;
      JSONParser stringParser(largeString);
      ok &= testFile(
         stringParser,
         "Large json string",
         "server/json/tests/large-string.json",
         largeString,
         true
      );

      ok &= testResult("Large json string length", (largeString.value().size() == 5013));
#endif

      cout << endl;
      
      return ok;
   }

   inline bool testSets()
   {
      cout << "Sets" << endl;
      
      bool ok = true;
      
      class OpenBrace : public BeeFishParser::Character {
      public:
         OpenBrace() : Character('{') {

         }

      };

 
      class CloseBrace : public BeeFishParser::Character {
      public:
         CloseBrace() : Character('}') {

         }

      };

      class Item : public Capture {
      public:
         Item(void* params = nullptr) :
            Capture(new Word("item"))
         {

         }
         
         
         virtual void success()
         {
            cerr << endl << "Item: " << _match->value() << endl;
         }
      };
      
      typedef Repeat<Item> Items;
      Items* items = new Items();
      ok &= testMatchDelete("Items", new Capture(items), "itemitemitem", nullopt, "itemitemitem");
      
      assert(ok);
      

      class Seperator : public BeeFishParser::Character {
      public:
         Seperator() : Character(',') {

         }
      };

      typedef
         Set<OpenBrace, Item, Seperator, CloseBrace> 
         _Set;
      
      
 
      ok &= testMatchDelete("Set", new Capture(new _Set()), "{item,item,item}", true, "{item,item,item}");
      
      assert(ok);
      
      ok &= testMatchDelete("Set empty", new Capture(new _Set()), "{}", true, "{}");
      ok &= testMatchDelete("Set blanks", new Capture(new _Set()), "{item, item ,item }", true);


      Capture object(
         new Set<OpenBrace, LoadOnDemand<Item>, Seperator, CloseBrace>()
      );
      

      ok &= testMatch("Set LoadOnDemand", &object, "{item,item}", true, "{item,item}");
assert(ok);

      class MySet;
      
      class MySetItem : public Capture {
      public:
         MySetItem(Match* set) : Capture()
         {
            _match = new Word("myset");
         }
         /*
         virtual void capture(Parser* parser, char c)
         {
             Capture::capture(parser, c);
             
             //_match->capture(parser, c);
         }
         */
      };

      class MySet : public Set<OpenBrace, MySetItem, Seperator, CloseBrace>
      {
      public:
         int _count = 0;
         
         MySet() : Set()
         {
         }
         
         virtual void onsetvalue(Match* item)
         override
         {
            cerr << endl << "matchedSetItem: " << item->value() << endl;
             
  
            if (item->value() == "myset")
               ++_count;
               
         }
         
         virtual Match* createItem()
         override
         {
            Match* item = new MySetItem(this);
            if (_parser)
               item->setup(_parser);
            return item;
         }
         /*
         virtual void capture(Parser* parser, char c)
         override
         {
             if (!_parser)
                setup(parser);
                
             //cerr << "{" << c << "}" << flush;
             
             _match->capture(parser, c);
         }
         */
         
      };
     
      MySet* mySet = new MySet();
      Capture cap = Capture(mySet);
      ok &= testMatch("Set with overload", &cap, "{myset,myset}", true, "{myset,myset}");
      ok &= testResult("Set with overload result", (mySet->_count == 2));

      
      BeeFishMisc::outputSuccess(ok);
 
      
      cout << endl;
      
      return ok;
      
   }
   
   inline bool testArrays()
   {
      cout << "Arrays" << endl;
      
      bool ok = true;
      
      ok &= testMatchDelete("Empty array", new Capture(new JSON()), "[]", true, "[]");
      ok &= testMatchDelete("Single array", new Capture(new JSON()), "[0]", true, "[0]");
      ok &= testMatchDelete("Double array", new Capture(new JSON()), "[true,false]", true, "[true,false]");
      ok &= testMatchDelete("Triple array", new Capture(new JSON()), "[1,2,3]", true, "[1,2,3]");
      ok &= testMatchDelete("Embedded array", new Capture(new JSON()), "[0,[]]", true, "[0,[]]");
      ok &= testMatchDelete("Array with blanks", new Capture(new JSON()), " [ 1, true ,null, false]", true," [ 1, true ,null, false]" );

      cout << endl;
      
      return ok;
      
   }

   inline bool testKeyedSets()
   {
      cout << "Keyed Sets" << endl;

      bool ok = true;
      
      class OpenBrace : public BeeFishParser::Character {
      public:
         OpenBrace() : Character('\"') {

         }

      };
      
      class CloseBrace : public BeeFishParser::Character {
      public:
         CloseBrace() : Character('\"') {

         }
      };

      class KeyValueSeperator : public BeeFishParser::Character {
      public:
         KeyValueSeperator() : Character('=') {

         }

      };

      class Seperator : public BeeFishParser::Character {
      public:
         Seperator() : Character(';') {

         }
      };

      class StyleCharacter : public Or {
      public:
         StyleCharacter() : Or (
            new Range('0', '9'),
            new Range('a', 'z'),
            new Range('A', 'Z')
         )
         {

         }
      };

      class StyleCharacters : 
         public Repeat<StyleCharacter>,
         public BString
     {
      
      public:
         StyleCharacters() : Repeat<StyleCharacter>()
         {

         }

         virtual void matchedItem(StyleCharacter* item) {
            push_back(item->character());
            Repeat::matchedItem(item);
         }
      };

      class Key : public StyleCharacters {

      };

      class Value : public StyleCharacters {

      };

      class StyleSet : public
         KeyedSet<
            OpenBrace, 
            Key, 
            KeyValueSeperator, 
            Value,
            Seperator,
            CloseBrace
         >
      {
      public:
         int _keyCount = 0;
         virtual void matchedKey(Key& key, Value& value) {
            _keyCount++;
         }

      };

      StyleSet* styleSet = new StyleSet();
      Capture capture(styleSet);

      ok &= testMatchDelete("Empty style", new Capture(new StyleSet()), "\"\"", true,  "\"\"");
      ok &= testMatchDelete("Style with one key", new Capture(new StyleSet()), "\"key=value\"", true, "\"key=value\"");
      ok &= testMatch("Style with two keys", &capture, "\"key1=value1;key2=value2\"", true, "\"key1=value1;key2=value2\"");
      ok &= testResult("Style matched two keys", styleSet->_keyCount == 2);
      cout << endl;

      return ok;
      
   }

   inline bool testObjects()
   {
      cout << "Objects" << endl;
      
      bool ok = true;
      
      
 
      ok &= testMatchDelete("Empty object", new Capture(new JSON()), "{}", true, "{}");
      ok &= testMatchDelete("Single field", new Capture(new JSON()), "{\"field\":true}", true, "{\"field\":true}");
      ok &= testMatchDelete("Double fields", new Capture(new JSON()), "{\"a\":1,\"b\":2}", true, "{\"a\":1,\"b\":2}");
      ok &= testMatchDelete("Triple object", new Capture(new JSON()), "{\"a\":1,\"b\":2,\"c\":[]}", true, "{\"a\":1,\"b\":2,\"c\":[]}");
      ok &= testMatchDelete("Embedded object", new Capture(new JSON()), "{\"obj\":{\"embedded\":true}}", true, "{\"obj\":{\"embedded\":true}}");
      ok &= testMatchDelete("Object with blanks", new Capture(new JSON()), "{ \"field\" : \"string value\" }", true, "{ \"field\" : \"string value\" }");
      /*
      Object* test = new Object();
      bool hit = false;
      BString _key;
      BString value;
      Capture* capture = new Capture(test);
      JSONParser parser(*capture);

      parser._onvalues["hello"] = [&hit, &_key, &value](const BString& key, JSON& json) {
         hit = true;
         _key = key;
         value = json.value();
      };

      ok &= testMatch(parser, "Object field", capture, "{\"hello\":1}", true, "{\"hello\":1}");
      
      ok &= testResult("Object field key hit", hit);
      ok &= testResult("Object key value", (_key == "hello"));
      ok &= testResult("Object field value", (value == "1"));
      
      delete capture;

      class ObjectTest : public Object {
      protected:
         BString _value;
      public:
         ObjectTest() : Object(
         )
         {
         }
         virtual void setup(Parser* parser) {

            JSONParser* parser_ = static_cast<JSONParser*>(parser);

            parser_->invokeValue(
               "key",
               [this](const BString& key, JSON& json) 
               {
                  _value = json.value();
               }
            );

            Object::setup(parser);
         }

         virtual BString value() {
            return _value;
         }
      } ;

      ObjectTest* objectTest = new ObjectTest();

      ok &= testMatch("Object test constructor", capture = new Capture(objectTest), "{\"key\":\"hello world\"}", true, "{\"key\":\"hello world\"}");
      ok &= testResult("Object test value", objectTest->value() == "hello world");
      delete capture;
*/
      cout << endl;
      
      return ok;
      
   }

#ifdef SERVER
   inline bool testStreams() 
   {
      
      cout << "Streams" << endl;
      
      bool ok = true;

      JSON jsonImage;
      JSONParser parser(jsonImage);

      BString last;
      parser.streamValue("image",
         [&last](const Data& buffer) {
            cerr << "Size : " << buffer.size() << endl;
            std::string str((const char*)buffer.data(), buffer.size());
            last = str;
         }
      );

      bool secretOk = false;
      parser.invokeValue("secret",
         [&secretOk](const BString& key, JSON& json) {
            secretOk = (key == "secret") && (json.value() == "mysecret");
         }
      );

      ok &= testFile(
         parser,
         "JSON with buffered image",
         "server/json/tests/stream.json",
         jsonImage,
         true
      );

      ok &= testResult(
         "JSON with invoke secret",
         secretOk
      );

      cout << endl;
      
      return ok;
   }
#endif

   inline bool testEmojis()
   {
      cout << "Emojis" << endl;
      
      bool ok = true;
      
      JSON parser1;
      JSON parser2;

     // ok &= testMatch("Double unicode", &parser1, "\"\\uD83D\\uDE00\"", true, "😀");
     // ok &= testMatch("Double unicode", &parser1, "\"😀\"", true, "😀");
      
      ok &= testMatch("Emoji 😀", &parser2, "\"😀\"", true, "😀");

      cout << endl;
      
      return ok;
   }
   
   inline bool testTypes()
   {
      cout << "Test Types" << endl;
      
      bool ok = true;
      
      JSON undefined;
      JSON _null;
      JSON boolean;
      JSON integer;
      JSON number;
      JSON string;
      JSON array;
      JSON object;
      
      ok &= testMatch("undefined", &undefined, "undefined", true, "");
      ok &= testResult("undefined type", undefined.type() == Type::UNDEFINED);

      ok &= testMatch("null", &_null, "null", true, "");
      ok &= testResult("null type", _null.type() == Type::NULL_);
      
      ok &= testMatch("boolean", &boolean, "true", true, "true");
      ok &= testResult("boolean type", boolean.type() == Type::BOOLEAN);
      
      ok &= testMatch("integer", &integer, "1", true, "1");
      ok &= testResult("integer type", integer.type() == Type::INTEGER);
      
      ok &= testMatch("number", &number, "1.1", true, "1.1");
      ok &= testResult("number type", number.type() == Type::NUMBER);

      ok &= testMatch("string", &string, "\"❤️\"", true, "❤️");
      ok &= testResult("string type", string.type() == Type::STRING);
      
      ok &= testMatch("array", &array, "[]", true);
      ok &= testResult("array type", array.type() == Type::ARRAY);
     
      ok &= testMatch("object", &object, "{}", true);
      ok &= testResult("object type", object.type() == Type::OBJECT);
     
      assert(ok);
      
      cout << endl;
      
      return ok;
   }
   
      
}

#endif
