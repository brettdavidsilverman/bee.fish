#ifndef BEE_FISH_DATABASE__STORAGE_H
#define BEE_FISH_DATABASE__STORAGE_H
#include <exception>
#include <iomanip>
#include "../b-string/string.h"
#include "../power-encoding/power-encoding.h"
#include "authentication.h"
#include "../Database/Database.hpp"
#include "../Id/Id.hpp"

using namespace BeeFishHTTPS;
using namespace BeeFishId;

namespace BeeFishDatabase {

   class Storage
   {
      Authentication& _auth;
      Path _bookmark;
   public:
      Storage(Authentication& auth, BString name) :
         _auth(auth),
         _bookmark(_auth.userData()[name])
      {
      }
      
      template<typename Key>
      bool has(const Key& key)
      {
         BeeFishDatabase::
            Path path(_bookmark);
            
         seek(path, key);
         
         return 
            path.hasData();
      }
      
      template<typename Key>
      BeeFishMisc::optional<BString> getContentType(
         const Key& key
      )
      {
         
         BeeFishDatabase::
            Path path(_bookmark);
            
         seek(path, key);
         
         if (path.hasData())
         {
            BeeFishMisc::optional<BString> contentType;
            contentType = getContentType(path);
            return contentType;
         }

         return BeeFishMisc::nullopt;
      }

      template<typename Key>
      BeeFishMisc::optional<BString> getItem(
         const Key& key,
         BeeFishMisc::optional<BString>& contentType
      )
      {
         
         BeeFishDatabase::
            Path path(_bookmark);
            
         seek(path, key);
         
         if (path.hasData())
         {
            BString value;
            path.getData(value);
            contentType = getContentType(path);

            return value;
         }

         return BeeFishMisc::nullopt;
      }


      template<typename Key>
      void getItem(
         const Key& key,
         BeeFishMisc::optional<BString>& contentType,
         std::string& data
      )
      {
         
         BeeFishDatabase::
            Path path(_bookmark);
            
         seek(path, key);
         
         if (path.hasData())
         {
            path.getData(data);

            contentType = getContentType(path);
         }
      }

      template<typename Key>
      void setItem(
         const Key& key,
         BeeFishMisc::optional<BString> contentType,
         const std::string& data
      )
      {
      
         BeeFishDatabase::
            Path path(_bookmark);
            
         path = path[key];
         
         path.setData(
            data
         );
         
         if (contentType.has_value()) {
            setContentType(path, contentType.value());
         }

      }

      template<typename Key>
      void setItem(
         const Key& key,
         const BeeFishBString::BString& value
      )
      {
         return setItem(key, BeeFishMisc::nullopt, value);
      }

      virtual BeeFishMisc::optional<BString> getContentType(Path path) {
         if (path.contains("content-type")) {
            path = path["content-type"];
            if (path.hasData()) {
               std::string data =
                  path.getData();
               BString contentType;
               path.getData(contentType);
               return contentType;
            }
         }
         return BeeFishMisc::nullopt;
      }

      virtual void setContentType(Path path, BString contentType) {
         path["content-type"].setData(contentType);
      }

      template<typename Key>
      void removeItem(const Key& key)
      {
         BeeFishDatabase::
            Path path(_bookmark);
         
         seek(path, key);
         path.deleteData();

         if (path.contains("content-type")) {
            path["content-type"].deleteData();
         }

      }
      
      void clear()
      {
         BeeFishDatabase::
            Path path(_bookmark);
         path.clear();
      }
      
      virtual BString name()
      {
         return "Storage";
      }
      
      template<typename Key>
      void seek(
         Path& path,
         const Key& key
      )
      {
         path << key;
      }
   };

};

#endif