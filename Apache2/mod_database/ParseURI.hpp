#ifndef BEE_FISH_WEB_SERVER__PARSE_URI
#define BEE_FISH_WEB_SERVER__PARSE_URI

#include "Database/Database.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishWebServer {
    
   class This : public Word
   {
   public:
      This() : Word("this")
      {
      }
   };
   
   class BeginProperty : public Character
   {
   public:
      BeginProperty() : Character('[')
      {
      }
   };
   
   class EndProperty : public Character
   {
   public:
      EndProperty() : Character(']')
      {
      }
   };
   
   class Property : public BeeFishJSON::String
   {
   };
   
   class PropertyPath :
      public Path,
      public And
   {
   protected:
      Path _properties;
      
   public:
      PropertyPath(Path start, Path properties) :
         Path(start),
         And(
            new Invoke(
               new This(),
               [this](Match* match)
               {
                  *this << Type::OBJECT;
               }
            ),
            new BeginProperty(),
            new Invoke(
               new Property(),
               [this](Match* match) {
   
                  
                  Path property =
                     _properties[match->value()];
                  
                  if (property.contains(_index))
                  {
                     
                     property = property[_index];
                     Size position = 0;
                     property.getData<Size>(position);
                     *this << position;
                  }
               }
            ),
            new EndProperty()
         ),
         _properties(properties)
      {
      }
   };

   Path parseURI(Database& database, const char* clientIP, const char* uri, const char* args) {
      

      Path path(database);
      path = path[HOST]; //[clientIP];
      Path properties = path[PROPERTIES];

      string _uri = uri;
      
      BString segment;
      std::stringstream segments(_uri);
      while(std::getline(segments, segment, '/'))
      {
         if (segment.size())
         {
            path = path[segment.decodeURI()];
         }
      }

      if (args) {
         BString query = args;
         query = query.decodeURI();
         PropertyPath propertyPath(path, properties);
         Parser parser(propertyPath);
         parser.read(query);
         path = propertyPath;
      }
      
      return path;
      
   }
   

}

#endif
