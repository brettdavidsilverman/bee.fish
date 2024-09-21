#ifndef BEE_FISH_WEB_SERVER__PARSE_URI
#define BEE_FISH_WEB_SERVER__PARSE_URI

#include "Database/Database.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishApache2 {
    
   Debug debug;
   
   class IdentifierPath : public Match
   {
   protected:
      Path _properties;
      Path* _path;
   public:
      IdentifierPath(const Path& properties, Path* path) :
         _properties(properties),
         _path(path)
      {
      }
      
      virtual void success()
      override
      {
               
         if (_properties.contains(value()))
         {
            Path property =
               _properties[value()];
                
            if (_path->contains(Type::OBJECT))
            {
               *_path << Type::OBJECT;
               if (property.contains(_path->index()))
               {
                  property = property[_path->index()];
                  Size position = -1;
                  property.getData<Size>(position);
                  *_path << position;
                  return;
               }
            
            }
         }
         
         Match::fail();
         
         std::stringstream stream;
         stream
            << "Invalid property"
            << " "
            << "'" << value() << "'";
        
         throw runtime_error(stream.str());
        
         
                  
      }
      
      
   };
   
   class QuotedIdentifier : public IdentifierPath
   {
   
   public:
      QuotedIdentifier(const Path& properties, Path* path) :
         IdentifierPath(properties, path)
      {
         _match = new BeeFishJSON::String();
      }
      
   };
   
   class Alpha : public Or
   {
   public:
      Alpha() : Or(
         new Range('a', 'z'),
         new Range('A', 'Z'),
         new Character('_')
      )
      {
      }
   };
   
   class Numeric : public Range
   {
   public:
      Numeric() : Range('0', '9')
      {
      }
   };
   
   class AlphaNumeric : public Or
   {
   public:
      AlphaNumeric() : Or(
         new Alpha(),
         new Numeric()
      )
      {
      }
   };
   
   class Identifier : public IdentifierPath
   {
   public:
      Identifier(const Path& properties, Path* path) :
         IdentifierPath(properties, path)
      {
         _match =
            new Capture(
               new And(
                  new Alpha(),
                  new Repeat<AlphaNumeric>(0)
               )
            );
      }
      
   };
   
   class PropertyPath :
      public Match
   {
   public:
      PropertyPath() {
         //assert(false);
      }
      
      PropertyPath(const Path& properties, Path* start )
      {
         _match = new Or(
            new And(
               new Character('['),
               new QuotedIdentifier(properties, start),
               new Character(']')
            ),
            new And(
               new Character('.'),
               new Identifier(properties, start)
            )
            
         );
      }
   };
   
   class PropertyPaths : public Repeat<PropertyPath>
   {
   protected:
      Path _properties;
      Path* _start;
   public:
      PropertyPaths(const Path& properties, Path* start) :
         Repeat<PropertyPath>(1),
         _properties(properties),
         _start(start)
      {
      }
      
      virtual PropertyPath* createItem()
      override
      {
         PropertyPath* item =
            new PropertyPath(_properties, _start);
            
         if (_parser)
            item->setup(_parser);
               
         return item;
      }
      
     
   };
   
   class Query : public Match
   {
   public:
      Query(const Path& properties, Path* start) 
      {
         _match =
            new And(
               new Word("this"),
               new PropertyPaths(properties, start)
            );
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
         BString _args = args;
         Query query(properties, &path);
         Parser parser(query);
         parser.read(_args.decodeURI());
         parser.eof();

         if (query.result() != true)
            throw runtime_error("Invalid property");
      }
      
      return path;
      
   }
   

}

#endif
