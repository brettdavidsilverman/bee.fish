#ifndef BEE_FISH_DATABASE_JSON_PATH_HPP
#define BEE_FISH_DATABASE_JSON_PATH_HPP
#include "../Database/DatabaseBase.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"
#include "JSONIndex.hpp"

namespace BeeFishDatabase {
 
   using namespace BeeFishParser;
   using namespace BeeFishScript;
   using namespace BeeFishJSON;

   /*
   db[OBJECTS][objectPath][position][type][value]
   db[PROPERTIES][key][VALUE][type][value][objectPath]
   db[PROPERTIES][key][POSITION][objectPath] = position;
   */
   
   class JSONPath :
      public Path
   {
   protected:
      Path _properties;
      Path _objects;
      
   public:
      JSONPath()
      {
      }
      
      JSONPath(Database& database, const Path& start) :
         Path(start)
      {
         Path root(database);
         
         _properties =
            root[HOST][PROPERTIES];
            
         _objects =
            root[HOST][OBJECTS];
           
      }
      
      JSONPath(Path& start) :
         JSONPath(start.database(), start)
      {
      }
      
      JSONPath operator [] (const BString& key)
      {
         Index objectId = _index;
         Index position =
            getObjectKeyPosition(
               objectId, 
               key
            );
            
         JSONPath path = (*this);
         
         path << Type::OBJECT
              << position;
              
         path.unlock();
         
         return path;
          
      }
      
      
      // properties[key][POSITION][objectId]
      Index getObjectKeyPosition(Index objectId, const BString& key)
      {
         Path positionPath =
            _properties[key][POSITIONS][objectId];
            
         Index position;
         
         if (positionPath.hasData())
            positionPath.getData<Index>(position);
         else {
             
            MinMaxPath objectPositions =
               _objects[objectId];
            position =
               objectPositions.max<Index>()
               + 1;
            positionPath.setData<Index>(position);
         }
         
         return position;
      }
      
      //_properties[key][VALUE][type][value][objectId]
      
      JSONPath operator [] (const char* key)
      {
         return (*this)[BString(key)];
      }
      
   };
   
   

}

#endif
