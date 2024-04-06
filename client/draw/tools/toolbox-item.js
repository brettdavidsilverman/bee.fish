class ToolboxItem extends Item {
   width = 100;
   height = 100;
   topLeft;
   toolbox;

   constructor(input) {
      super(input);

      if (input == undefined)
         input = {}
      
      if (input.last == undefined)
         this.topLeft = new Point({x: 10, y: 10});
      else {
         var width = input.toolbox.width;
         var x = input.last.topLeft.x + input.last.width + 10;
         var y = input.last.topLeft.y;
         if ((x + this.width + 5) > width) {
            x = 10;
            y += input.last.height + 10;
         }

         this.topLeft = new Point({x, y});     
      }
      
      this.dimensions =
         new Dimensions(
            {
               min: this.topLeft,
               max: new Point(
                  {
                     x: this.topLeft.x + this.width,
                     y: this.topLeft.y + this.height
                  }
               )
            }
         );
      
      this.toolbox = input.toolbox;

   }


   async draw(context) {
      
      var rectangle = new Rectangle(this);
      rectangle.fillStyle = "rgba(256,256,0, 0.25)";
      await rectangle.draw(context);
      await super.draw(context);
   }

   get selection() {
      return this.toolbox.parent.selection;
   }

   set selection(value) {
      this.toolbox.parent.selection = value;
   }
  
   remove() {
      this.release();
   }

   getNextIndex() {
      return null;
   }
}