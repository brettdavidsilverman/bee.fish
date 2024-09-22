#ifndef BEE_FISH_HTTPS__RESPONSE_H
#define BEE_FISH_HTTPS__RESPONSE_H

#include <filesystem>

#include "app.h"

using namespace std;
using namespace std::filesystem;

namespace BeeFishHTTPS {

   class Session;
   
   class Response {
   protected:
      const bool _log = true;

      Session* _session;
      
      int _status;
      string _statusText;

      string _headers;
      
      size_t _headersLength = 0;
      size_t _bytesTransferred = 0;
      size_t _contentLength = 0;
      const size_t _pageSize = getPageSize();
      App*      _app = nullptr;
   public:
      Response(
         Session* session
      )
      {
         
         ResponseHeaders headers;

         App* app = nullptr;


         for ( auto factory : appFactories )
         {

            app = factory->create(
               session,
               headers
            );
            
            app->handleResponse();
            
            _status = app->status();
            _statusText = app->statusText();

            if (_status != -1)
               break;
  
            delete app;
            
            app = nullptr;

         }

         if (app)
         {
            if (_log) {
               clog << _status
                  << " " << _statusText
                  << " Served by "
                  << app->name()
                  << endl;
            }
                 
           
            _app = app;

            switch (_app->serve()) {
               case App::SERVE_FILE:
                  _contentLength = file_size(_app->_filePath);
                  break;
               case App::SERVE_CONTENT:
                  _contentLength = _app->_content.size();
                  break;
               case App::SERVE_DATA:
                  _contentLength = _app->_contentLength;
                  break;
               default:
                  _contentLength = _app->_contentLength;

            }
         
            headers.replace(
               "content-length",
               std::to_string(_contentLength)
            );
         }

         stringstream headersStream;
         
         headersStream 
            << "HTTP/1.1 " 
            << _status 
            << " "
            << _statusText
            << "\r\n";
            
         for (auto pair : headers)
         {
            headersStream
               << pair.first
               << ": "
               << pair.second.str()
               << "\r\n";
         }
            
         headersStream << "\r\n";
         _headers = headersStream.str();
         _headersLength = _headers.size();
       
      }
      
      virtual ~Response()
         {
         if (_app) {
            delete _app;
            _app = nullptr;
         }
      }
      
      virtual Data getNext(size_t& length)
      { 
         
         Data response;
         
         if (_bytesTransferred < _headersLength)
         {
            // Serve headers
            if ( (_bytesTransferred + length) 
                 > _headersLength )
            {
               length =
                  _headersLength -
                  _bytesTransferred;
            }
            
/*            
            string response =
               _headers.substr(
                  _bytesTransferred,
                  length
               );
*/            
            response = 
               Data(
                  (const Byte*)(_headers.data() + _bytesTransferred), 
                  length
               );

         }
         else {

            // Calculate largest length (truncating if required)
            if ( ( _bytesTransferred + length ) >
               (_headersLength + _contentLength ) )
            {              
               length =
                  _headersLength +
                  _contentLength -
                  _bytesTransferred;
            }
                  
            switch (_app->serve())
            {
               case App::SERVE_DATA:
               {
                  size_t pageIndex =
                     (_bytesTransferred - _headersLength) /
                     _pageSize;

                  Data page =
                     _app->_bookmark[pageIndex];
                  
                  length = page.size();

                  response = page;
                  break;
               }
               case App::SERVE_CONTENT:
               {
                  response = Data(
                     (const Byte*)
                        (
                           _app->_content.data()
                              + _bytesTransferred 
                              - _headersLength
                        ),
                     length
                  );

                  break;
               }
               case App::SERVE_FILE:
               {
                  Data buffer = Data::create(length);

                  ifstream input(_app->_filePath);
                     input.seekg(
                        _bytesTransferred -
                        _headersLength
                     );
                  
                  input.read((char*)buffer.data(), length);
                  
                  input.close();
               
                  response = buffer;   
                  break;
               }
               default:
                  throw std::logic_error("Invalid Serve enum value");
             }
         }

         _bytesTransferred += length;

         return response;
      }
   
      virtual bool end() 
      {
         bool end =
            _bytesTransferred ==
            ( _headersLength + _contentLength );
            
         if ( end )
         {
            _headers.clear();

            if (_app) {
               delete _app;
               _app = nullptr;

            }
         }
         
         return end;
      }

   

   };
   

   
}

#endif