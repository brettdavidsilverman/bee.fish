class Layers {
   
   constructor(input) {
   
      Object.assign(this, input);
   
      if (!input.stack)
         this.stack = [];
         
   }
   
   async top() {

      var stack = await this.stack;
      var topPointer = stack[stack.length - 1];
      var promise;
      if (Pointer.isPointer(topPointer))
         return topPointer.fetch();
      else
         return Promise.resolve(topPointer);
   }
 
   async push(layer) {
      
      layer.layer = layer;

      await setupMatricies(layer);
      
      var stack = await this.stack;
      
      stack.push(layer);
      
      return layer;
      
      async function setupMatricies(layer) {
         
         
         layer.transformMatrix = new Matrix();
         layer.inverseTransformMatrix = new Matrix();
         
         var matrix = await canvas.initialMatrix();

         layer.matrix = matrix;

         layer.inverse = matrix.inverse();
            
      }
      

   }
   
   async pop() {

      if (this.length() == 0)
         return;

      var stack = await this.stack;
      return stack.pop();
      
   }
   
   async draw(context) {
      
      var stack = await this.stack;
      
      for ( var index = 0;
                index < stack.length;
                ++index) {
         var layerPointer = stack[index];
         var layer = await
            layerPointer.fetch();
         
         drawLayer(
            layer,
            (index + 1) / stack.length
         );
      }
   
      
      async function drawLayer(layer, blur) {

         context.globalAlpha = blur;
         
         var matrix = await layer.matrix;
         
         context.applyMatrix(matrix);

         context.dimensions =
            getDimensions(layer);
            
         layer.draw(context);

      }
   
      async function getDimensions(layer) {
         var canvas = await layer.canvas;

         var dimensions =
            Dimensions.fromRectangle(
               canvas
            );

         var inverseTransformMatrix =
            await layer.inverseTransformMatrix;
        
         dimensions.transform(
            inverseTransformMatrix
         );
         
         return dimensions;
      }
   }
   
   async length() {
      var stack = await this.stack;
      return stack.length;
   }
      
}


