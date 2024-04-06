var Memory = {}
Memory.storage = new Storage(remoteStorage);
//Memory.storage = localStorage;
Object.prototype.save = saveObject;
Object.prototype.remove = removeObject;

// Save an object instance to storage
function saveObject(map = new Map(), promises = []) {
      
   var id = this["="];

   if (map.has(this))
      return Promise.resolve(id.key);
      
   map.set(this);

   // (Tell getters from fetch on demand
   // to return pointers)
   Memory.pointers = true;
   
 
   // Get the json representation
   // of this objects state
   var string = this.toString(
      Shorthand.COMPUTER
   );
   
  // console.log(string);
   
   Memory.pointers = false;
   
   // Store the json string
   var promise =
      Memory.storage.setItem(
         id.key,
         string
      );
      
   promises.push(promise);
   
   var object = this;
   
   if (Array.isArray(this))
   {
      this.saveChildren(map, promises);
   }
   else
   {
      // Save the children.
      Object.keys(this).forEach(
         (property) =>
            saveChild(property, promises)
      );
   }
   
   return Promise.all(promises);
   
   function saveChild(property, promises) {

      if (property == "=")
         return;
         
      var descriptor = Object
         .getOwnPropertyDescriptor(
            object,
            property
         );
         
      var value = descriptor.value;

      if ( value instanceof Object &&
           !(value instanceof Id)  &&
           !(value instanceof Function) )
      {
         value.save(map, promises);
         //alert("setFetchOnDemand: " + property)
         setFetchOnDemand(object, property);
      }
   }
   
}

// Fetch an objects string from storage,
// and recreate the original object
Memory.fetch = function(
   key,
   map = new Map()
   )
{

   Memory.map = map;
   if (!Memory.map)
      Memory.map = new Map();
      
   var promise;
   
   // See if the object has been 
   // fetched before.
   if (Memory.map.has(key))
   {
      promise = Promise.resolve(
         Memory.map.get(key)
      );
      
      return promise;
   }
   
   // get the json object from storage
   promise =
   Memory.storage.getItem(key).then(
      function(string)
      {
   
         if (string === null)
         {
            throw new _KeyNotFoundError(key);
         }
   
         // Convert the json string to
         // an object.
         var json = JSON.parse(
            string
         );
         
         return json;
      }
   ).then(
      function(json)
      {
   
         // Get the id as this has the
         // type information
   
         var id = new Id({key});
   
         json["="] = id;

         // Create the class function
         // from the ids name.
         var type = id.type;
         
         // Construct the object using
         // either the copy constructor,
         // or the custom function
         var object;

         if (type.fromStorage
            instanceof Function)
         {
      
            // Use custom function
            object = 
              type.fromStorage(json);
         }
         else {
         
            // Use copy constructor
            object = new type(json);
            
         }
  
         // Save the typed object to memory
         Memory.map.set(id.key, object);
         if (!Array.isArray(object))
         {
            // Replace pointers with
            // fetch on demand getters
            Object.keys(object).forEach(
               function(property) {
                  setFetchOnDemand(
                     object, property
                  );
               }
            );
        }
        else
        {
           // Replace structs to pointers
           for ( var i = 0;
                 i < object.length; 
                 ++i )
           {
              var value = object[i];
              
              if (Pointer.isPointer(value))
              {
                 object[i] =
                    new Pointer(value);
              }
           }
        }
        
      
        return object;
     }
  )
  .catch(
     error => {
        if (error instanceof _KeyNotFoundError)
           return null;
        else
           throw error;
     }
  );
  
  return promise;
   

}

// Create get (read) and set (write)
// functions as fetch on demand.
// The property is only fetched from
// memory when needed.
function setFetchOnDemand(object, property) {
 
 
   // Get the value.
   var descriptor =
      Object.getOwnPropertyDescriptor(
         object,
         property
      );
   

   if (descriptor.value == undefined) {

      // Property has not been accessed,
      // its already in fetch on demand
      // mode
      return;
   }
   
   var value = object[property];
   
   if ( Function.isFunction(value) )
   {
      // Create the function
      console.log("Memory.js setFetchOnDemand. The eval needs to be injection proof");
      
      newValue = eval(
         "(" + value["=>"] + ")"
      );
            
      newValue["="] = value["="];
      object[property] = newValue;
      return;
   }
   
   
   if (!Pointer.isPointer(value))
      return;
      
   // Create the typed pointer
   // object.
   var pointer = new Pointer(value);
         
   // Set the read/write
   // functions on the object.
   Object.defineProperty(
      object,
      property,
      {
         get: getter,
         set: setter,
         enumerable: true
      }
   );
            
   // Simply fetch the property
   // from memory and set it
   // back on the typed object.
   async function getter() {
 
      if (Memory.pointers)
         return pointer;
         
      var fetched = await pointer.fetch();

      this[property] =
         Promise.resolve(
            fetched
         );
            
      return this[property];
   }
            
   // Remove the getter/setter
   // and replace with the
   // fetched object.
   function setter(setValue) {
      Object.defineProperty(
         object,
         property,
         {
            value: setValue,
            writable: true
         }
      );
   }

}
function removeObject() {
   return Memory.storage.removeItem(
      this["="].key
   );
}

class _KeyNotFoundError
{
   key;
   
   constructor(key) {
      this.key = key;
   }
}


