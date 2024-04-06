class Points extends Array {

   constructor(...input) {
      super(...input);
      this.forEach(
         (element, index, array) => {
         
            var point = new Point(element);
            array[index] = point;            
         }
      );
   }
   
   draw(context)
   {
      context.beginPath();
      
      var point = this[0];
      
      if (this.length == 1) {
         
         context.arc(
            point.x,
            point.y,
            1/ 2,
            0,
            Math.PI * 2
         );
         context.fillStyle = this.strokeStyle;
         
         context.fill();
         context.stroke();
         
         return;
      }
      
      context.moveTo(
         point.x,
         point.y
      );

      this.forEach(
         point => {
            context.lineTo(
               point.x,
               point.y
            )
         }
      );
      
      context.stroke();
     

   }

   matrixTransform(matrix) {
      return this.map(
         point => point.matrixTransform(matrix)
      );
   }
  
}
