class Pointer extends Id
{

   constructor(input)
   {
      super(getSuperInput(input));

      function getSuperInput(input) {
         if (input.key) {
            var id = new Id(input.key);
            return id;
         }

         if (input.id)
            return input.id;

         return null;
      }

      //object = undefined;
      //key = undefined;
      if (this.got == undefined)
         this.got = false;

      if (input instanceof Pointer)
      {
         Object.assign(this, input);
      }
      else if (typeof input == "string")
         this.key = input;
      else if (input.object != undefined)
      {
         var object = input.object;
         
         if (object.key)
            this.key = object.key;
         else
            this.key =
               object.key =
               new Id(
                  {name: object.constructor.name}
               ).key;
               
         this.object = object;
         this.got = true;
         Pointer.map.set(this.key, this.object);
         console.log("Pointer:" + Pointer.map.size);
      }
      else if (input.key){
         this.key = input.key;
      }
   }

   getItem(input)
   {
      if (this.got)
      {
         return this.object;
      }

      if (Pointer.map.has(this.key)) {
         var object = Pointer.map.get(this.key);
         this.object = object;
         this.got = true;
         return Promise.resolve(this.object);
      }

      var id = Id.fromKey(this.key);
      var self = this;

      var promise = id.getItem(input)
      .then(
         function(object) {
            self.object = object;
            self.got = true;
      
            Pointer.map.set(self.key, self.object);
            return object;
         }
      )

      console.log("Fetch:" + Pointer.map.size);

      return promise;
   }

   toJSON()
   {
      return {
         id: super.toJSON(),
         key: this.key
      }
   }

   toString()
   {
      return JSON.stringify(
         this, null, "   "
      );
   }
   
   equals(pointer)
   {
      return pointer.key == this.key;
   }
   

   release() {
      delete this.object;
      this.got = false;
      Pointer.map.delete(this.key);
      console.log("~" + Pointer.map.size);
      
   }

}

Pointer.map = new Map();

