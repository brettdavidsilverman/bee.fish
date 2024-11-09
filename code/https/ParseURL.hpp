#ifndef BEE_FISH_HTTPS__PARSE_URL
#define BEE_FISH_HTTPS__PARSE_URL

#include "../Database/Database.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishHTTPS {
    
   
   Debug debug;
   
   class IdentifierPath : public Match
   {
   protected:
      JSONPath* _path;

   public:
      IdentifierPath(JSONPath* path) :
         _path(path)
      {
      }
      
      void setMatch(Match* match)
      {
         _match = new Invoke(
            match,
            [this](Match* match)
            {
               bool matched = onmatch();
               if (!matched)
               {
                  return false;
               }
               return true;
            }
         );
            
      }
      
      
      virtual bool onmatch()

      {
         BString& key = value();
         if (_path->contains(key))
         {
            (*_path) = (*_path)[key];
           
            return true;
            
         }

         std::stringstream stream;
         stream
            << "Invalid property"
            << " "
            << "'" << key << "'"
            << ". Expected one of";
            
         Path properties =
            _path->properties()
            [BY_OBJECT];
         
         MinMaxPath object =
            properties[_path->id()];
            
         Stack stack(object);
         Index position;
         while (object.next(stack, position))
         {
            optional<BString> key =
               _path->getObjectKey(position);
               
            if (key.has_value()) {
               stream << "\r\n   '"
                      << key.value()
                      << "'";
            }  
         }
         
         BString error = stream.str();

         fail(error);
        
         return false;
      }
      
      

      
   };
   
   class ArrayIndexPath : public IdentifierPath
   {
   
   public:
      ArrayIndexPath(JSONPath* path) :
         IdentifierPath(path)
      {
         
      }
      
      virtual bool onmatch()
      override

      {

         BString& stringIndex = value();
         Index index = atol(stringIndex.c_str());
  
         JSONPath path = *_path;
         if (path.contains(index))
         {
            (*_path) = path[index];
           
            return true;
            
         }
         std::stringstream stream;
         stream
            << "Invalid index"
            << " "
            << stringIndex
            << ". Expected index range is [0, "
            << path[Type::ARRAY].max<Index>() << "]";
        
         BString error = stream.str();
         
         fail(error);

         return false;
      }
      
      virtual void setup(Parser* parser)
      {
         setMatch(new BeeFishJSON::Integer());
         IdentifierPath::setup(parser);
      }
   };
   
   class QuotedIdentifier : public IdentifierPath
   {
   
   public:
      QuotedIdentifier(JSONPath* path) :
         IdentifierPath(path)
      {
         
      }
      
      virtual void setup(Parser* parser)
      {
         setMatch(new BeeFishJSON::String());
         IdentifierPath::setup(parser);
         
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
      Identifier(JSONPath* path) :
         IdentifierPath(path)
      {
         
      }
      
      virtual void setup(Parser* parser)
      {
         setMatch(
            new Capture(
               new And(
                  new Repeat<AlphaNumeric>(0)
               )
            )
         );
         IdentifierPath::setup(parser);
      }
   };
   
   class PropertyPath :
      public Match
   {
   protected:
      JSONPath* _start;
      ArrayIndexPath* _arrayIndex;
      QuotedIdentifier* _quotedIdentifier;
      Identifier* _identifier;
      
   public:
      PropertyPath() {
         assert(false);
      }
      
      PropertyPath(JSONPath* start) :
         _start(start)
      {
         _match = new Or(
            new And(
               new Character('['),
               _arrayIndex =
                  new ArrayIndexPath(_start),
               new Character(']')
            ),
            new And(
               new Character('['),
               _quotedIdentifier =
                  new QuotedIdentifier(_start),
               new Character(']')
            ),
            new And(
               new Character('.'),
               _identifier =
                  new Identifier(_start)
            )
            
         );
      }
      
   };
   
   class PropertyPaths : public Repeat<PropertyPath>
   {
   protected:
      JSONPath* _start;
      PropertyPath* _last;
     
      
   public:
      PropertyPaths(JSONPath* start) :
         Repeat<PropertyPath>(0),
         _start(start)
      {
      }
      
      virtual PropertyPath* createItem()
      override
      {
         PropertyPath* item =
            new PropertyPath(_start);
            
         if (_parser)
            item->setup(_parser);
               
         _last = item;
         
         return item;
      }
      
   };
   
   class Query : public Match
   {
     
   protected:
      Word* _wordThis;
      PropertyPaths* _propertyPaths;
      JSONPath* _start;
      
   public:
      Query(JSONPath* start) :
         _start(start)
      {
         _match =
            new And(
               _wordThis = new Word("this"),
               _propertyPaths =
                  new PropertyPaths(_start)
            );
      }
      
      
      virtual void eof(Parser* parser)
      override
      {
         setup(parser);
         
         if (_wordThis->result() == true)
         {
            if (_propertyPaths->result() == nullopt)
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

   optional<JSONPath> parseURL(Path userData, BString& error, const BString& clientIP, const BString& method, const BString& uri, const BString& args) {
      Debug debug;
      
      Path path = userData[URLS];
      
      
      BString segment;
      std::stringstream segments(uri);
      while(std::getline(segments, segment, '/'))
      {
         if (segment.size())
         {
            path = path[segment.decodeURI()];
         }
      }
      
      JSONPath jsonPath = path;
      
      if (method == "GET")
      {
         if (jsonPath.isDeadEnd())
         {
            error = "Path not found";
            return nullopt;
         }
         
      }

      
      if (args.length())
      {
         Query query(&jsonPath);
         Parser parser(query);
         parser.read(args.decodeURI());
         parser.eof();
         
         if (parser.result() == false)
         {
            error = parser.getError();
            if (!error.length())
               error = "Unkown error";
            return nullopt;
         }
      }
      
      return jsonPath;
      
   }
   

}

#endif
