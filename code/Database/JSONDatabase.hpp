#ifndef BEE_FISH_DATABASE__JSON_DATABASE_HPP
#define BEE_FISH_DATABASE__JSON_DATABASE_HPP

#include "JSONIndex.hpp"
#include "JSON2Path.hpp"
#include "Path2JSON.hpp"


namespace BeeFishDatabase 
{

    class JSONDatabase : public Database
    {
    protected:

    public:
        JSONDatabase(const std::string& filePath) :
            Database(filePath) 
        {
        }

        friend ostream& operator << (ostream& output, JSONDatabase& database)
        {
            Path2JSON path(database);
            path.write(output, 0);
            return output;
        }
    
        friend istream& operator >> (istream& input, JSONDatabase& database)
        {
            JSON2Path json2Path(database);
            json2Path.read(input);
            return input;
        }

    };
}


#endif
