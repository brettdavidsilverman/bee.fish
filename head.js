window.onerror =
function (message, source, lineno, colno, error)
{

   
   if (!error)
      error = {}

   var stack = "";
   if (error.stack)
      stack = String(error.stack);

   if (message instanceof Object)
      message = JSON.stringify(message, null, "   ");
   
   var string = 
      [
         message,
         source,
         "line: " + lineno,
         "col: " + colno,
         stack
      ].join("\n");
      
   if (alert.silent)
      console.log(string);
   else
      alert(string);
   
};


var hostname =
   window.location.hostname;
   
var port =
   window.location.port;



function require(src) {
   console.log("Must include '" + src + "'");
}

var module = {}

function writeln(pre, text)
{
   document.getElementById(pre)
      .appendChild(
         document.createTextNode(
            text + "\r\n"
         )
      );
}

Object.prototype.toString = function() {
   
   return JSON.stringify(this, null, "   ");
}

