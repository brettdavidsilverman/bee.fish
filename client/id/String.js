String.prototype.escape = function()
{

    var string = "";

    for (var i = 0; i < this.length; ++i)
        string += escape(this[i]);


    return string;

    function escape(c) {
        
        switch (c)
        {
        case "\0":
            return "\\0";
        case '\"':
            return "\\\"";
        case '\\':
            return "\\\\";
        case '\b':
            return "\\b";
        case '\f':
            return "\\f";
        case '\r':
            return "\\r";
        case '\n':
            return "\\n";
        case '\t':
            return "\\t";
        }
        
        return c;
        
    }
}

String.prototype.unescape = function()
{
    var string = "";

    for (var i = 0; i < this.length; ++i) {
        var c = this[i];
        if (c == '\\' && i < this.length) {
            var c2 = this[++i];
            string += unescape(c2);
        }
        else
            string += c;
    }
    
    return string;

    function unescape(c) {
        
        switch (c)
        {
        case '0':
            return "\0";
        case '"':
            return  '\"';
        case '\\':
            return "\\";
        case 'b':
            return "\b";
        case 'f':
            return "\f";
        case 'r':
            return "\r";
        case 'n':
            return "\n";
        case 't':
            return "\t";
        }
        
        return c;
        
    }
}


String.prototype.urlEncode = function() {
    
    var encoded =
        this
        .replaceAll(" ", "%20")
        .replaceAll("\\", "%2F")
        .replaceAll("&", "%26")
        .replaceAll(";", "%3B");
        
    return encoded;
}

String.prototype.urlDecode = function() {
    
    var decoded =
        this
        .replaceAll("%20", " ")
        .replaceAll("%2F", "\\")
        .replaceAll("%2f", "\\")
        .replaceAll("%26", "&")
        .replaceAll("%3B", ";")
        .replaceAll("%3b", ";");
        
    return decoded;
    
}
