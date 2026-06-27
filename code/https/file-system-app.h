#ifndef BEE_FISH_SERVER__FILE_SYSTEM_APP_H
#define BEE_FISH_SERVER__FILE_SYSTEM_APP_H

#include <filesystem>
#include <algorithm>
#include "../Config.hpp"
#include "../Miscellaneous/MimeTypes.hpp"
#include "../web-request/web-request.h"
#include "session.h"
#include "response.h"
#include "authentication.h"
#include "app.h"


#define DISABLE_CACHE

namespace BeeFishHTTPS {
using namespace std;
using namespace std::filesystem;
using namespace BeeFishWeb;
using namespace BeeFishMiscellaneous;

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
    




public:
    FileSystemApp(
        Session* session,
        ResponseHeaders& responseHeaders,
        BeeFishHTTPS::Authentication& authentication
    ) : App(session, responseHeaders, authentication)
    {

    }


    virtual void handleResponse()
    {

        WebRequest* request = _session->request();
        
        if (request->search().size() &&
            !request->searchObject().contains("redirect"))
            return;

        BString requestPath = request->path();

        // Get the file path from the request path
        try
        {
            _filePath =
                getFilePath(requestPath);

            // Make sure file path is under
            // file system root
            if (!pathIsChild(_filePath, fileSystemPath()))
                throw runtime_error("Invalid path accessing beyond root.");

        }
        catch (filesystem_error& err)
        {
            // Default error of not found
           // _status = -1;
           // _statusText = "Not Found";
            return;
        }


        // Redirect to add trailing slashes
        // to directories
        if (redirectDirectories(
                    *request,
                    _filePath
                ) )
        {
            return;
        }
        
        if (is_directory(_filePath))
        {
            try
            {
                _filePath =
                    getFilePath(
                        requestPath + 
                        BString("/index.html")
                    );
            }
            catch(filesystem_error& err)
            {
            }
        }

        string contentType = "text/plain; charset=utf-8";
        string cacheControl = BeeFishMiscellaneous::_defaultCacheControl;
        string extension = _filePath.extension();
        if (!extension.length())
            extension = _filePath.filename();
            
        if (is_directory(_filePath) )
        {
            // Directory listing
            _serve = App::SERVE_CONTENT;
            contentType = "text/html; charset=utf-8";
            _content = getDirectoryListing(
                    requestPath,
                    _filePath
            );

        }
        else if ( _mimeTypes.count(
                    extension
                )
        )
        {
            // File content
            MimeType mimeType = 
                _mimeTypes[
                    _filePath.extension()
                ];
            contentType = mimeType.contentType;
            cacheControl = mimeType.cacheControl;
            _serve = SERVE_FILE;
        }
        else
        {
            return;
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
        
        _status = 200;

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
            if (!extension.length())
                extension = filename;
                
            if (filename[0] == '\"')
                filename = filename.substr(0, filename.size() - 2);

            stream
                    << "      <li>";


            if ( is_directory(file) ||
                 _mimeTypes.count(extension))
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
        string filename = WWW_ROOT_DIRECTORY;
        filename += child;
        return path(filename);
    }


    bool redirectDirectories(const WebRequest& request, const path& filePath)
    {
        
        BString path = request.path();
               
        if ( is_directory(filePath))
        {

            if (!path.endsWith("/"))
            {
                BString newPath =
                    path + BString("/") ;

                BString search = request.url().search().value();

                if (search.length())
                    newPath += BString("?") + search;

                redirect(newPath, false);
                return true;
            }
        }
        return false;
    }

    void write(ostream& headerStream, const int status, const BString& statusText, const BString& requestPath, const path& filePath)
    {
        BeeFishScript::Object output;

        output["status"] = status;
        output["statusText"] = statusText;

        _authentication.write(output);

        output["requestPath"] = requestPath;

        if ((std::string)filePath != "")
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
        return "File system app";
    }
    
};



}

#endif
