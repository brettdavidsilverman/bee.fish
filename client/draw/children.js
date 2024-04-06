class Children extends Collection {

   constructor(input) {
      super(input);
   }

   async draw(context)
   {
      var children = await this.all();
     
      for ( var i = children.length - 1;
            i >= 0;
            --i )
      {

         var child = children[i];

         if (child instanceof Item) {
            await child.draw(context);
         }

      }

   }
   
   async transform(matrix)
   {
      var children = await this.all();
     
      for ( var i = 0;
            i <  children.length;
            ++i )
      {
         var child = children[i];
         child.transform(matrix);
      }

   }

   async hitTest(point)
   {
      var children = await this.all();

      var hit = null;

      for ( var i = children.length - 1;
            i >= 0;
            --i )
      {
         var child = children[i];

         if (child instanceof Item) {
            hit = await child.hitTest(
               point
            );
         }
         
         if (hit)
            return hit;
      }
      
      return null;
      
   }
   
   async findParent(dimensions)
   {
 
      var children = await this.all();
      
      for ( var i = 0;
            i < children.length;
            ++i )
      {
         var test = children[i];
         
         var parent = await test.findParent(
            dimensions
         );
         
         if (parent)
            return parent;
      }
      
      return null;
      
   }
  
   async findChildren(parent) {
   
      var foundChildren = new Map();
      
      var children = await this.all();
      
      children.forEach(
         child => {

            if ( child.isChild(
                    parent
                 ) )
            {
               foundChildren.set(child.key, child);
            }
        
         }
      );
      
      return foundChildren;
   }
   
   async hide() {
      this.forEach(
         child => child.object && child.object.hide()
      );
   }

   async findParent(dimensions)
   {
 
      var children = await this.all();
      
      for ( var i = 0;
            i < children.length;
            ++i )
      {
         var test = children[i];
         
         var parent = await test.findParent(
            dimensions
         );
         
         if (parent)
            return parent;
      }
      
      return null;
      
   }

   push(item) {
      var childPointer = this.getChildPointer(item);
      if (childPointer != undefined) {
         
         return super.push(childPointer);
      }
   }

   getChildPointer(item) {
      
      if (item == undefined)
         return undefined;

      var childPointer;

      if (item instanceof ChildPointer)
         childPointer = item;
      else if (item instanceof Pointer) {
         childPointer = new ChildPointer({parent: this.parent, pointer : item});
      }
      else if (item instanceof Id) {
         childPointer = new ChildPointer({parent: this.parent, pointer: {object: item}});
      }
      else if (typeof item == "string") {
         childPointer = new ChildPointer({parent: this.parent, pointer: {key: item}});
      }
      else if (typeof item == "object") {
         childPointer = new ChildPointer({parent: this.parent, pointer: item.pointer});
      }

      if (childPointer == undefined)
         throw new Error("Cant create ChildPointer from item.");


      return childPointer;


   }

/*
   async resize(canvas)
   {
 
      var children = await this.all();
      
      for ( var i = 0;
            i < children.length;
            ++i )
      {
         var test = children[i];

         test.resize(canvas);
      }
      
      return null;
      
   }
*/
}
