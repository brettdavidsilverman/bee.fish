#ifndef BEE_FISH__JSON_IN_STREAM_H
#define BEE_FISH__JSON_IN_STREAM_H

#include <stack>
#include "../json/json-parser.h"
#include "../json/json.h"
#include "../database/database.h"
//#include "../database/path.h"
#include "b-script-parser.h"

namespace BeeFishBScript {

   using namespace BeeFishBString;
   using namespace BeeFishJSON;
   
   class JSONInStream :
      public JSONParser
   {
   private:
      struct StackItem {
         Path _path;
         size_t _nextIndex;
         StackItem(Path path) :
            _path(path),
            _nextIndex(0)
         {
         }
      };
      
   protected:
      JSON _json;
      Path _path;
      std::stack<StackItem> _stack;
   public:
      JSONInStream(
         Path& path
      ) :
         JSONParser(_json),
         _path(path)
      {
      }
      
      friend
      JSONInStream& operator << (
         JSONInStream& stream,
         const BString& json
      )
      {
         stream.read(json);
         return stream;
      }
      
      friend
      JSONInStream& operator << (
         JSONInStream& stream,
         const char* json
      )
      {
         stream << BString(json);
         return stream;

      }
      
      virtual void onbeginobject(Match* match)
      {
         _stack.push(StackItem(_path));
         _path <<
            (float)BeeFishJSON::Type::OBJECT;
      }

      virtual void onendobject(Match* match) {
         _path = _stack.top()._path;
         _stack.pop();
      }

      virtual void onbeginarray(Match* match) {
      }

      virtual void onarrayvalue(Match* match) {
      }

      virtual void onendarray(Match* match) {
      }

      virtual void onobjectvalue(const BString& key, const JSON* value) {
         _path = _stack.top()._path;
         Path bookmark = _path;
         
         _path << 'K' << key <<
            _stack.top()._nextIndex;
            
         _path = bookmark;
         _path << 'V' <<
            _stack.top()._nextIndex++;
      }

      virtual void onvalue(JSON* json) {
         BeeFishJSON::Type type = json->type();
         _path << (float)type;
         switch (type) {
            case UNDEFINED:
               break;
            case __NULL:
               break;
            case BOOLEAN:
               _path.setData(
                  json->value() == "true"
               );
               break;
               case NUMBER: 
               {
                  std::string str = json->value().str();

                  _path.setData(
                     atof(str.c_str())
                  );

                  break;
               }
            case STRING:
               _path.setData(json->value());
               break;
            case ARRAY:
               throw std::logic_error("ARRAY not implemented yet");
               break;
            case OBJECT:
               break;
            default:
               throw std::logic_error("Invalid json");
            
         }
      }
      
      

     
   };
   
   
}

#endif