class Collection extends Array {

   id;
   parent;

   constructor(input) {
      super();

      if (input == undefined)
         input = {}

      this.parent = input.parent;
            
      if (input.id == undefined)
         this.id = new Id({name: this.constructor.name});
      else
         this.id = new Id(input.id);

      var self = this;
      if (input.keys != undefined) {        
         input.keys.forEach(
            key => self.push(key)
         );
      }
      else if (input instanceof Array) {
         input.forEach(
            key => self.push(key)
         );
      }

   }

   push(item) {
      var pointer = this.getPointer(item);
      if (pointer != undefined)
         return super.push(pointer);
   }

   getPointer(item) {
      
      if (item == undefined)
         return undefined;

      var pointer;

      if (item instanceof Pointer)
         pointer = item;
      else if (item instanceof Id) {
         pointer = new Pointer({object: item});
      }
      else if (typeof item == "string") {
         pointer = new Pointer({key: item});
      }
      else if (typeof item == "object") {
         pointer = new Pointer(item);
      }

      if (pointer == undefined)
         throw new Error("Cant create Pointer from item.");

      return pointer;

   }

   get keys() {
      var keys =
         this.filter(
            pointer => pointer != undefined
         ).map(
            pointer => pointer.key
         );
      return new Array(...keys);
   }

   toJSON() {
      return {
         id: this.id,
         keys: this.keys
      }
   }


   remove(item) {
      var index = this.findIndex((element) => element && (element.key == item.key));

      if (index >= 0) {
         this[index] = undefined;
      }
   }

   async all() {
      
      var pointers = this.filter(
         pointer => pointer != undefined
      );

      var items = pointers.map (
         item => item.fetch()
      );
      
      var fetched = await Promise.all(items);

      return fetched.filter(item => item != undefined);
      
   }
   


   async removeAll() {

      var items = await this.all();
      items.forEach(
         item =>
            item.remove()
      );
      this.length = 0;
   }

   async release() {
      this.forEach(
         pointer => pointer.release()
      )
   }

}
