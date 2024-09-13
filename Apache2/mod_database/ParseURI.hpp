#ifndef BEE_FISH_WEB_SERVER__PARSE_URI
#define BEE_FISH_WEB_SERVER__PARSE_URI

#include "Database/Database.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishWebServer {
    
   Debug debug;
   
   class This : public Word
   {
   protected:
      Path* _path;
      
   public:
      This(Path* path) :
         Word("this"),
         _path(path)
      {
          
      }
      
      virtual void success()
      override
      {
         *_path << Type::OBJECT;
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
   protected:
      Path& _properties;
      Path* _path;
      
   public:
      Property(Path& properties, Path* path) :
         _properties(properties),
         _path(path)
      {
      }
      
      virtual void success()
      override
      {
         Path property =
            _properties[value()];
                  
         if (property.contains(_path->index()))
         {
            property = property[_path->index()];
            Size position = 0;
            property.getData<Size>(position);
            *_path << position;
            BeeFishJSON::String::success();
         }
         else
            fail();
                  
      }
   };
   
   class LatinCharacter : public Or
   {
   public:
      LatinCharacter() : Or(
         new Range('a', 'z'),
         new Range('A', 'Z'),
         new Range('0', '9')
      )
      {
      }
   };
   
   class Identifier : public Capture
   {
   protected:
      Path& _properties;
      Path* _path;
      
   public:
      Identifier(Path& properties, Path* path) :
         Capture(
            new And(
               new Optional(
                  new Character('_')
               ),
               new Repeat<LatinCharacter>(1)
            )
         ),
         _properties(properties),
         _path(path)
      {
      }
      
      virtual void success()
      override
      {
         
         Path property =
            _properties[value()];
                  
         if (property.contains(_path->index()))
         {
            
            property = property[_path->index()];
            Size position = 0;
            property.getData<Size>(position);
            
            *_path << position;
            
            Capture::success();
         }
         else {
            fail();
         }
                  
      }
      
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
            new This(this),
            new Or(
               new And(
                  new Character('.'),
                  new Identifier(properties, this)
               ),
               new And(
                  new BeginProperty(),
                  new Property(properties, this),
                  new EndProperty()
               )
            )
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
            
         if (parser.result() == nullopt) 
            parser.eof();
         
         if (propertyPath.result() == true)
            path = propertyPath;
         else
            throw runtime_error("Invalid property");
      }
      
      return path;
      
   }
   

}

#endif
