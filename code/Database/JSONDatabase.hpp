#ifndef BEE_FISH_DATABASE__JSON_DATABASE_HPP
#define BEE_FISH_DATABASE__JSON_DATABASE_HPP

#include "Database.hpp"
#include "JSONIndex.hpp"

namespace BeeFishDatabase
{

   class JSONDatabase : public Database
   {
   protected:
      Path _properties;
      Path _objects;
      Path _originPath;
      BString _origin;
   public:
      JSONDatabase(const BString& origin, const BString& filePath = "")
         : Database(filePath)
      {
         setOrigin(origin);
      }
      
      inline void setOrigin(const BString& origin)
      {
          
         if (_origin == origin)
            return;
            
         _origin = origin;
         
         Path root(*this);
  
         _originPath =
            root[origin];
            
         _properties =
            _originPath[PROPERTIES];
            
         _objects =
            _originPath[OBJECTS];
            
         
           
      }
      
      const BString& getOrigin() const
      {
         return _origin;
      }
      
      Path objects() const
      {
         return _objects;
      }
      
      Path properties() const
      {
         return _properties;
      }
      
      Path origin() const
      {
         return _originPath;
      }
      
      
   };
   
}

#endif