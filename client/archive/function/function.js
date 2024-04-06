var oldFunctionToString =
   Function.prototype.toString;
Function.prototype.toString =
   objectToString;

Function.prototype.toShorthand =
   function(shorthand) {
      var id = this["="];
      var string =
         oldFunctionToString.call(this);
      return {
         "=": id.toShorthand(shorthand),
         "=>": string
      }
   }

Function.fromStorage =
   function(object) {
      var string = object["=>"];
      var output = eval(
         "(" + string + ")"
      );
      Object.assign(output, object);
      delete output["=>"];
      return output;
   }
   
Function.isFunction =
   function(object)
   {
      if (object instanceof Function)
         return true;
      
      if ( (typeof(object) == "object") &&
           ("=>" in object) )
         return true;
        
     return false;
   }