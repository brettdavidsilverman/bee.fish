#ifndef BEE_FISH_HTTPS__RESPONSE_HEADERS_H
#define BEE_FISH_HTTPS__RESPONSE_HEADERS_H
#include <vector>
#include <filesystem>

using namespace std;

namespace BeeFishHTTPS {

    class Session;
    
    typedef std::multimap<BString, BString>
        ResponseHeadersBase;
    
    class ResponseHeaders :
        public ResponseHeadersBase
    {
    public:
        // Defined in session.h
        ResponseHeaders(Session* session);
        
        void replace(const BString& key, const BString& value)
        {
            erase(key);
            emplace(key, value);
        }
        
        bool contains(const BString& key) const
        {
            return count(key) > 0;
        }
        
        BString operator[] (const BString& key) const
        {
            BString value;
            
            for (auto search = begin();
                  search != end();
                  ++search)
            {
                if (search->first == key)
                    value = search->second;
            }
            
            return value;
        }
        
        friend ostream& operator << (ostream& out, const ResponseHeaders& headers)
        {
            out << "{" << endl;
            
            for (auto it = headers.cbegin();
                  it != headers.cend();
                  ++it)
            {
                out << "\t" << it->first << ": "
                    << it->second
                    << endl;
            }
            
            out << "}";
            
            return out;
        }
        
        
    };
    

}

#endif