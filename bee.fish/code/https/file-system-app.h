#ifndef BEE_FISH_SERVER__FILE_SYSTEM_APP_H
#define BEE_FISH_SERVER__FILE_SYSTEM_APP_H

#include <filesystem>
#include <algorithm>
#include "../config.h"
#include "session.h"
#include "../web-request/web-request.h"
#include "response.h"
#include "authentication.h"
#include "app.h"

using namespace std;
using namespace std::filesystem;
using namespace BeeFishWeb;

namespace BeeFishHTTPS {

   class FileSystemApp : public App {
   

      static bool comparePaths (
         const path& i,
         const path& j
      ) 
      {
         if (is_directory(i) && !is_directory(j))
            return true;
         else if (!is_directory(i) && is_directory(j))
            return false;
         else
            return (i<j); 
      }

   public:
      struct MimeType
      {
         string contentType;
         string cacheControl;
      };
      
      inline static string _noCacheControl =
         "no-store, max-age=0";

#if defined(DEBUG) || defined(DISABLE_CACHE)
      inline static string _defaultCacheControl =
         _noCacheControl;

#else
      inline static string _defaultCacheControl =
         "public, max-age=60";
#endif

      inline static map<string, MimeType>
         _mimeTypes{
            {
               ".txt",
               {
                  "text/plain; charset=UTF-8",
                  _defaultCacheControl
               }
            },
            {
               ".html",
               {
                  "text/html; charset=UTF-8",
                  "no-store, max-age=0"
               }
            },
            {
               ".js",
               {
                  "text/javascript; charset=UTF-8",
                  _defaultCacheControl
               }
            },
            {
               ".json",
               {
                  "application/json; charset=UTF-8",
                  _defaultCacheControl
               }
            },
            {
               ".css", 
               {
                  "text/css; charset=UTF-8",
                  _defaultCacheControl
               }
            },
            {
               ".jpg",
               {
                  "image/jpeg",
                  "public, max-age=31536000, immutable"
               }
            },
            {
               ".gif",
               {
                  "image/gif",
                  "public, max-age=31536000, immutable"
               }
            },
            {
               ".ico",
               {
                  "image/x-icon",
                  "public, max-age=31536000, immutable"
               }
            },
            {
               ".h",
               {
                  "text/plain; charset=UTF-8",
                  _defaultCacheControl
               }
            },
            {
               ".cpp",
               {
                  "text/plain; charset=UTF-8",
                  _defaultCacheControl
               }
            },
            {
               ".c",
               {
                  "text/plain; charset=UTF-8",
                  _defaultCacheControl
               }
            },
            {
               ".ino",
               {
                  "text/plain; charset=UTF-8",
                  _defaultCacheControl
               }
            },
            {
               ".bin",
               {
                  "application/octet-stream",
                  _noCacheControl
               }
            },
            {
               ".sh",
               {
                  "application/json; charset=UTF-8",
                  _defaultCacheControl
               }
            }
         };
         


         
   public:
      FileSystemApp(
         Session* session,
         ResponseHeaders& responseHeaders
      ) : App(session, responseHeaders)
      {

      }


      virtual void handleResponse()
      {
   
         _status = 200;
         WebRequest* request = _session->request();

         const BString& requestPath = request->path();
         
         // Get the file path from the request path
         try
         {
            
            _filePath = getFilePath(requestPath);
           
            // Make sure file path is under
            // file system root
            if (!pathIsChild(_filePath, fileSystemPath()))
               throw runtime_error("Invalid path accessing beyond root.");
    
         }
         catch (filesystem_error& err)
         {
            // Default error of not found
            _status = 404;
            _statusText = "Not Found";
         }
               
         // Redirect to add trailing slashes
         // to directories
         if ( redirectDirectories(
                 *request,
                 _filePath
              ) )
         {
            return;
         }
         
         string contentType = "text/plain; charset=UTF-8";
         string cacheControl = _defaultCacheControl;
         
         if ( _status == 200 )
         {
            if (is_directory(_filePath) )
            {
               // Directory listing
               _serve = App::SERVE_CONTENT;
               contentType = "text/html; charset=UTF-8";
               _content = getDirectoryListing(
                  requestPath,
                  _filePath
               );
              
            }
            else if ( _mimeTypes.count(
                    _filePath.extension()
                 ) )
            {
               // File content
               MimeType mimeType = _mimeTypes[
                  _filePath.extension()
               ];
               contentType = mimeType.contentType;
               cacheControl = mimeType.cacheControl;
               _serve = SERVE_FILE;
            }
            else if ( _filePath.filename() ==
                      "Makefile" )
            {
               _serve = SERVE_FILE;
            }
            else
            {
               // Not found
               _status = 404;
               _statusText = "Not found";
            }
         }
         
         if ( _status != 200 )
         {

            stringstream contentStream;
            
            write(contentStream, _status, _statusText, requestPath, _filePath);

            contentType = "application/json; charset=UTF-8";
            _content = contentStream.str();
            _serve = App::SERVE_CONTENT;
            
         }

         _responseHeaders.replace(
            "content-type",
            contentType
         );
         
         _responseHeaders.replace(
           "cache-control",
            cacheControl
         );
            
         _responseHeaders.replace(
            "connection",
            "keep-alive"
         );
/*      
         _responseHeaders.replace(
            "access-control-allow-origin",
            "*"
         );
*/         
      }
      
      string getDirectoryListing(const BString& requestPath, const path& directory)
      {
         stringstream stream;
         stream
            << "<!DOCTYPE html>" << endl
            << "<html lang=\"en\">" << endl
            << "<head>" << endl
            << "   <script src=\"/head.js\"></script>" << endl
            << "   <meta charset=\"utf-8\"/>" << endl
            << "   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>" << endl
            << "   <script src=\"/client/console/console.js\"></script>" << endl
            << "   <link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\" />" << endl
            << "   <title>Template</title>" << endl
            << "</head>" << endl
            << "<body>" << endl
            << "   <h1>" << requestPath.str() << "</h1>" << endl
            << "   <ul>" << endl;
            
         // store paths
         vector<path> paths;

         copy(
            directory_iterator(directory),
            directory_iterator(),
            back_inserter(paths)
         );
         
        
         sort(
            paths.begin(),
            paths.end(),
            comparePaths
         );
         
         for (const auto & entry : paths)
         {
            
            const path& file = entry;
            string filename = file.filename();
            string extension = file.extension();
            
            if (filename[0] == '\"')
               filename = filename.substr(0, filename.size() - 2);
               
            stream
               << "      <li>";
                  
           
            if ( is_directory(file) ||
                 _mimeTypes.count(extension) ||
                 filename == "Makefile" )
            {
               stream
                  << "<a href=\"" << filename << "\">";
                  
               if ( is_directory(file) )
               {
                  stream << "+";
               }
               
               stream 
                  << filename << "</a>" << endl;
            }
            else
               stream
                  << filename << endl;

             stream
                  << "</li>" << endl;
         }
         
         stream
            << "   </ul>" << endl
            << "</body>" << endl
            << "</html>" << endl;

         return stream.str();
      }
      
      bool pathIsChild(const path & child, const path & prefix)
      {
         auto pair = std::mismatch(child.begin(), child.end(), prefix.begin(), prefix.end());
         return pair.second == prefix.end();
      }
      
      path fileSystemPath(string child = "") const
      {
         return path(FILE_SYSTEM_PATH + child);
      }
      

      bool redirectDirectories(const WebRequest& request, const path& filePath)
      {
         if ( is_directory(filePath) &&
              request.path() != "/" )
         {
            const BString& path =
               request.path();
               
            if (path[path.size() - 1] != '/')
            {
               BString newPath =
                  path + "/" +
                  request.query();
                  
               redirect(newPath, true);
               
               return true;
            }
         }
         return false;
      }
      
      void write(ostream& headerStream, const int status, const string& statusText, const BString& requestPath, const path& filePath)
      {
         BeeFishBScript::Object output;

         output["status"] = status;
         output["statusText"] = statusText;

         Authentication::write(output);
         
         output["requestPath"] = requestPath;
                    
         if (filePath != "")
         {
            BString path(filePath);
            
            output["filePath"] = path;
            
            // extension
            if ( _mimeTypes.count(
                  filePath.extension()
                ) )
            {
               MimeType mimeType =
                  _mimeTypes[
                     filePath.extension()
                  ];

               output["contentType"] = BString(mimeType.contentType);
               output["cacheControl"] = BString(mimeType.cacheControl);

            }
         }

         headerStream << output << endl;

      }
      


      virtual BString name()
      {
         return "File System";
      }
   };

   

}

#endif
