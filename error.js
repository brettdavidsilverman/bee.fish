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

function Error(error, f)
{
    var where;
    if (typeof f == "string")
       where = f
    else if (typeof f == "function")
       where = f.name;
    else if (f)
       where = f;
    else
       where = "Unknown function";
       
    alert(error + "\r\n   " + where);
    
}
