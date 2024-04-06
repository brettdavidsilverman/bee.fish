class Dimensions {

   constructor(input) {

      if (input == null)
         input = {}
      
      Object.assign(this, input);
   
      if (input.min == undefined)
         this.min = new Point(
            {
               x: Number.MAX_VALUE,
               y: Number.MAX_VALUE
            }
         );
      else
         this.min = new Point(input.min);
         
      if (input.max == undefined)
         this.max = new Point(
            {
               x: -Number.MAX_VALUE,
               y: -Number.MAX_VALUE
            }
         );
      else
         this.max = new Point(input.max);
   }
      
   static fromPoints(points) {
      
      var min = points[0];
      var max = points[0];
    
      points.forEach(
         (point) => {
            min = Point.min(min, point);
            max = Point.max(max, point);
         }
      );

      return new Dimensions(
         {
            min,
            max
         }
      );
   }

   isPointInside(point) {

      return (
         this.min.x <= point.x    &&
         point.x    <= this.max.x &&
         this.min.y <= point.y    &&
         point.y    <= this.max.y
      );
      
   }
   
   contains(dimensions) {
   
      return (
         this.min.x <= dimensions.min.x &&
         this.min.y <= dimensions.min.y &&
         this.max.x >= dimensions.max.x &&
         this.max.y >= dimensions.max.y
      );
   }
   
   intersects(dimensions) {
   
      return (
         this.max.x >= dimensions.min.x &&
         this.min.x <= dimensions.max.x &&
         this.max.y >= dimensions.min.y &&
         this.min.y <= dimensions.max.y
      );

   }
   
   points() {
      var dimensions = this;
      var min = dimensions.min;
      var max = dimensions.max;
      
      var points = new Points(
         // top left
         new Point(
            {
               x: min.x,
               y: min.y
            }
         ),
         
         // top right
         new Point(
            {
               x: max.x,
               y: min.y
            }
         ),
         
         // bottom right
         new Point(
            {
               x: max.x,
               y: max.y
            }
         ),
         
         // bottom left
         new Point(
            {
               x: min.x,
               y: max.y
            }
         )
      );
      
      return points;
   }
  
   get width() {
      return this.max.x - this.min.x;
   }
   
   get height() {
      return this.max.y - this.min.y;
   }
   

   get left() {
      return this.min.x;
   }
   
   get top() {
      return this.min.y;
   }

   get center() {
      return {
         x: this.left + this.width / 2,
         y: this.top + this.height / 2
      };
   }

   get topLeft() {

      var topLeft = new Point(
         {
            x: this.left,
            y: this.top
         }
      );
      
      return topLeft;
   }
   
   matrixTransform(matrix) {
   
      var min = this.min.matrixTransform(matrix);
      var max = this.max.matrixTransform(matrix);

      return new Dimensions(
         {
            min,
            max
         }
      );
   }
   

   toString() {
      return JSON.stringify(this, null, "   ");
   }
   
   static fromRectangle(input) {
   
      var topLeft = input.topLeft;
      if (!topLeft)
         topLeft = new Point(
            {
               x: 0,
               y: 0
            }
         );
      
      var width = input.width;
      if (!width)
         width = 0;
      
      var height = input.height;
      if (!height)
         height = 0;
      
      var min = new Point(
         {
            x: topLeft.x,
            y: topLeft.y - height
         }
      );
   
      var max = new Point(
         {
            x: topLeft.x + width,
            y: topLeft.y
         }
      );
   
      var dimensions = new Dimensions(
         {
            min: min,
            max: max
         }
      );

      return dimensions;
      
   }

   toJSON() {
      return {
         min: this.min.toJSON(false),
         max: this.max.toJSON(false)
      }
   }

   copy() {
      return new Dimensions(
         {
            min: new Point(this.min),
            max: new Point(this.max)
         }
      );
   }

   include(dimensions) {
      var min = Point.min(this.min, dimensions.min);
      var max = Point.max(this.max, dimensions.max);

      return new Dimensions(
         {
            min,
            max
         }
      );
      
   }
   
}