class Rectangle extends Line {

   constructor(input) {
      super(input);
      if (!input.strokeStyle)
         this.strokeStyle = "black";
 
      if (!input.fillStyle)
         this.fillStyle = "yellow";

      this.dimensions = input.dimensions;

   }
   
   
   async draw(context) {

      

      var dimensions = this.dimensions;
      
      
      if (this.fillStyle) {
         context.fillStyle = this.fillStyle;
         context.fillRect(
            dimensions.min.x,
            dimensions.min.y,
            dimensions.width,
            dimensions.height
         );
            
      } 

      if (this.strokeStyle) {
         context.strokeStyle = this.strokeStyle;
         context.lineWidth = this.lineWidth / context.matrix.scale();
         context.strokeRect(
            dimensions.min.x,
            dimensions.min.y,
            dimensions.width,
            dimensions.height
         );
      }

      return true;
         
   }
   
   getNextIndex() {
      return null;
   }


}
