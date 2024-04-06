Array.prototype.toString = objectToString;
Array.prototype.toShorthand = arrayToShorthand;
Array.prototype.save = saveObject;
Array.prototype.saveChildren = arraySaveChildren;
Array.fromStorage = arrayFromStorage;

Array.prototype.encode = encodeArray;
Array.decode = decodeArray;

function arrayToShorthand(
   shorthand)
{
      
   var array = this.encode(shorthand);
   
   // Add extra custom fields
   var custom = {};
   var object = this;
   var hasCustom = false;
   Object
      .keys(this)
      .slice(this.length)
      .forEach(
         (field) => {
            if (field != "=") {
               var element = object[field];
               custom[field] =
                  getPointer(element, shorthand);
               hasCustom = true;
            }
         }
      );
   
   if (!hasCustom)
      custom = undefined;
      
   var json = {
      "=": this["="]
              .toShorthand(shorthand),
      "[]": array,
      "{}": custom
   }
   
   return json;

 
}

function encodeArray(shorthand) {
   // create an array of pointers
   var array = new Array(this.length);
   
   for (var i = 0; i < this.length; ++i) {
      var element = this[i];
      array[i] =
         getPointer(element, shorthand);
   }
   
   return array;
}

function getPointer(element, shorthand) {
   
   var value;
   if ((element instanceof Object &&
       !(element instanceof Id)) ||
       Array.isArray(element))
   {
      // Get a pointer to element
      var pointer = new Pointer(element);
      value =
         pointer.toShorthand(shorthand);
   }
   else
      // Set value to unchanged
      value = element;
         
   return value;
}

function arrayFromStorage(input) {

   var id = new Id(input["="]);
   var data = input["[]"];
   var custom = input["{}"];
   
   if (Memory.map.has(id.key))
      return Memory.map.get(id.key);
      
   var type = id.type;
   
   var array = type.decode(data, id);
   
   Object.assign(array, custom);
  
   array["="] = id;
 
   return array;
}

function decodeArray(data, id) {

   var array;
   if (id.type.from instanceof Function)
      array = id.type.from(data);
   else
      array = new id.type(...data);
      
   Memory.map.set(id.key, array);
   
   array.forEach(
      function(element, index) {
         if (Pointer.isPointer(element))
         {
            var pointer =
               new Pointer(element);
           // element = pointer.fetch();
            array[index] = pointer;
         }
      }
   );
   

   
   return array;
}

function arraySaveChildren(map, promises) {
   
   // Save each array element,
   // converting objects to pointers
   for (var i = 0; i < this.length; ++i)
   {
      var value = this[i];
         
      if ( value instanceof Object &&
         !( value instanceof Id) )
      {
         value.save(map, promises);
         this[i] = new Pointer(value);
      }
         
   }

}

