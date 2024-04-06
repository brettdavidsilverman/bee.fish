Object.prototype.toString = objectToString;

// Function to convert an object to
// a string.
// Returns the full object as a json
// formatted string.
// References are replaced
// with pointers.
function objectToString(
   shorthand = Shorthand.HUMAN)
{
  
   Object.map = new Map();
   
   var object =
      this.toShorthand(shorthand);
   
   var indent;
   
   if ( ( shorthand & Shorthand.HUMAN ) ||
        ( shorthand & Shorthand.FULL ) )
      indent = "   ";
      
   var string = JSON.stringify(
      object,
      null,
      indent
   );
   
   Object.map.clear();
   
   return string;
}

// Create an object copy of input
// that is only base level deep.
Object.prototype.toShorthand =
function(shorthand) {

   var input = this;
   var output = {};
   
   addKey("=");
   
   Object
      .keys(this)
      .forEach(
         (key) => addKey(key)
      );
   
   return output;
   
   async function addKey(key) {
         
      var value = input[key];
      
      if (value instanceof Promise)
         value = await value;
         
      if (value instanceof Object)
      {
      
      
         
         if ( key == "=") {
            // Id field
            var id = value;
            output["="] =
               id.toShorthand(shorthand);
            
            return;
         }
         
         if ( value instanceof Function ) {
            output[key] =
               value.toShorthand(
                  Shorthand.FULL | shorthand
               );

            return;
         }
         
         if ( shorthand === Shorthand.FULL ) {
   
            if (!Object.map.has(value)) {
            
               Object.map.set(value, true);
               output[key] =
                  value.toShorthand(shorthand);
                  
               return;
            }
            
         }
         
         // Replace object with a
         // pointer.
         if (value instanceof Pointer)
            pointer = value;
         else
            pointer = new Pointer(value);
                  
         output[key] = 
            pointer.toShorthand(shorthand);
            
      }
      else
         output[key] = value;
   }

}




