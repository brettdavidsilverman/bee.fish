#ifndef BEE_FISH_WEB_SERVER__PARSE_URI
#define BEE_FISH_WEB_SERVER__PARSE_URI

#include "Database/Database.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishApache2 {
    
   
   
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
                  Match::success();
                  return;
               }
            
            }
         }
         
         Match::fail();
         
         std::stringstream stream;
         stream
            << "Invalid property"
            << " "
            << "\"" << escape(value()) << "\"";
        
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
   
   class AlphaNumeric : public Not
   {
   public:
      AlphaNumeric() : Not(
         new Or(
            new Character('.'),
            new Character('[')
         )
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
                  new Repeat<AlphaNumeric>(0)
               )
            );
      }
      
   };
   
   class PropertyPath :
      public Match
   {
   protected:
      Path _properties;
      Path* _start;
      
   public:
      PropertyPath() {
         assert(false);
      }
      
      PropertyPath(const Path& properties, Path* start)
         :
            _properties(properties),
            _start(start)
      {
         _match = new Or(
            new And(
               new Character('['),
               new QuotedIdentifier(_properties, _start),
               new Character(']')
            ),
            new And(
               new Character('.'),
               new Identifier(_properties, _start)
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
         Repeat<PropertyPath>(0),
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
     
   protected:
      Word* _wordThis;
      PropertyPaths* _propertyPaths;
      protected:
      Path _properties;
      Path* _start;
      
   public:
      Query(const Path& properties, Path* start) 
         :
         _properties(properties),
         _start(start)
      {
         _match =
            new And(
               _wordThis = new Word("this"),
               _propertyPaths =
                  new PropertyPaths(_properties, _start)
            );
      }
      
      
      virtual void eof(Parser* parser)
      override
      {
         setup(parser);
         
         if (_wordThis->result() == true)
         {
            if (_propertyPaths->result() != false)
            {
               _propertyPaths->eof(parser);
               if (_propertyPaths->result() == true)
               {
                  success();
                  return;
               }
            }
         }
         
         fail();
       
      }
      
     
   };

   Path parseURI(Database& database, const char* clientIP, const char* uri, const char* args) {
      Debug debug;
      
      Path root(database);
      Path path = root[HOST][OBJECTS];
      Path properties = root[HOST][PROPERTIES];

      debug << database << endl;
      debug.flush();
      
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

      if (args && strlen(args)) {
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
